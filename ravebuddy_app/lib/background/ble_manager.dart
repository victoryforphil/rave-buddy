import 'dart:async';

import 'package:flutter_blue/flutter_blue.dart';

class BLEManager {
  final FlutterBlue _flutterBlue = FlutterBlue.instance;

  void start() {
    print("BLE/start: Starting");
    //peroidicCheck();
    Timer timer = Timer.periodic(Duration(seconds: 6), (timer) {
      startScan();
    });
  }

  void stop() {}

  void startScan() {
    print("BLE/startScan: Starting scan");
    _flutterBlue.scan(timeout: Duration(seconds: 5)).listen((scanResult) {
      onDeviceConnected(scanResult.device);
    });
  }

  void onDeviceConnected(BluetoothDevice device) {
    print('BLE/onDeviceConnected Connected to the device ${device.name}');
  }
}
