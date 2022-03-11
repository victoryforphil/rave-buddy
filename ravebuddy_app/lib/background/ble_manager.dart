import 'dart:async';
import 'dart:convert';

import 'package:flutter/material.dart';
import 'package:flutter_blue/flutter_blue.dart';
import 'package:provider/provider.dart';
import 'package:ravebuddy_app/models/RaveUnit.dart';

class BLEManager {
  final FlutterBlue _flutterBlue = FlutterBlue.instance;
  final BuildContext context;
  int _rssi = 0;
  BLEManager(this.context);
  void start() {
    print("BLE/start: Starting");
    //peroidicCheck();
    startScan();
    Timer timer = Timer.periodic(Duration(seconds: 5), (timer) {
      _flutterBlue.stopScan();
      startScan();
    });
  }

  void stop() {}

  Future<List<String>> getConnectedBlacklist() async {
    return await _flutterBlue.connectedDevices
        .then((value) => value.map((device) => device.id.toString()).toList());
  }

  void startScan() async {
    print("BLE/startScan: Starting scan");
    bool connected = false;
    _flutterBlue.scan(timeout: Duration(seconds: 1)).listen((scanResult) async {
      List<String> blackList = []; //
      //print("BLE/startScan: blackList: $blackList");
      if (scanResult.device.name.contains("RaveBuddy") && !connected) {
        onDeviceFound(scanResult.device);
        connected = true;
        _rssi = scanResult.rssi;
      }
    });
  }

  void onDeviceFound(BluetoothDevice device) async {
    print('BLE/onDeviceFound: Found device ${device.name}');
    await device.connect();
    device.discoverServices();
    device.services.listen((event) {
      for (BluetoothService service in event) {
        onServiceFound(service, device);
      }
    });

    await Future.delayed(Duration(seconds: 4));
    device.disconnect();
  }

  void onServiceFound(BluetoothService service, BluetoothDevice device) async {
    print('BLE/onServiceFound: Found service ${service.uuid.toString()}');

    RaveUnitState state = RaveUnitState(0, 0, 0, 0, 0, _rssi);

    await Future.delayed(Duration(milliseconds: 500));
    for (BluetoothCharacteristic characteristic in service.characteristics) {
      // wait 5s then read the value

      await characteristic.read().then((data) {
        String dataStr = "";
        try {
          dataStr = utf8.decode(data);
        } catch (e) {
          print("BLE/onServiceFound: Error decoding data");
          return;
        }
        if (dataStr.indexOf("=") == -1) {
          return;
        }
        String key = dataStr.substring(0, dataStr.indexOf("="));

        if (key == "ID") {
          int id = int.parse(dataStr.substring(dataStr.indexOf("=") + 1));
          print("BLE/onCharacteristicFound: ID: $id");
          state.id = id;
        }

        if (key == "GPS_LOC") {
          String gps = dataStr.substring(dataStr.indexOf("=") + 1);

          List<String> gpsList = gps.split(",");
          state.gpsLat = double.parse(gpsList[0]);
          state.gpsLon = double.parse(gpsList[1]);
          print("BLE/onCharacteristicFound: GPS_LAT: $gpsList");
        }

        if (key == "GPS_SATS") {
          int gpsSats = int.parse(dataStr.substring(dataStr.indexOf("=") + 1));
          print("BLE/onCharacteristicFound: GPS_SATS: $gpsSats");
          state.gpsSats = gpsSats;
        }

        if (key == "TIMESTAMP") {
          int timestamp =
              int.parse(dataStr.substring(dataStr.indexOf("=") + 1));
          print("BLE/onCharacteristicFound: TIMESTAMP: $timestamp");
          if (timestamp == 0) {
            return;
          }
          state.timestamp = timestamp;
        }
      });

      Provider.of<RaveUnits>(context, listen: false).onUpdate(state);
    }
  }
}
