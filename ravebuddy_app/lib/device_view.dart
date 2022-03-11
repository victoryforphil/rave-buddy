import 'dart:async';
import 'dart:convert';

import 'package:flutter/material.dart';
import 'package:flutter_blue/flutter_blue.dart';
import 'package:google_maps_flutter/google_maps_flutter.dart';

class PageDeviceView extends StatefulWidget {
  final BluetoothDevice device;

  const PageDeviceView({Key? key, required this.device}) : super(key: key);

  @override
  _PageDeviceViewState createState() => _PageDeviceViewState();
}

class _PageDeviceViewState extends State<PageDeviceView> {
  @override
  Widget build(BuildContext context) {
    widget.device.connect();
    widget.device.discoverServices();

    print("Device ${widget.device.name}");

    return Scaffold(
      appBar: AppBar(
        // Here we take the value from the MyHomePage object that was created by
        // the App.build method, and use it to set our appbar title.
        title: Text(widget.device.name),
      ),
      body: StreamBuilder<List<BluetoothService>>(
        stream: widget.device.services,
        initialData: [],
        builder: (c, snapshot) {
          return Column(
            children: snapshot.data!
                .map((ser) => ServiceWidget(service: ser))
                .toList(),
          );
        },
      ),
    );
  }
}

class ServiceWidget extends StatefulWidget {
  final BluetoothService service;
  String name = "";
  LatLng location = LatLng(0, 0);
  String time = "0000";
  int sats = 0;
  ServiceWidget({Key? key, required this.service}) : super(key: key);

  @override
  State<ServiceWidget> createState() => _ServiceWidgetState();
}

class _ServiceWidgetState extends State<ServiceWidget> {
  @override
  void initState() {
    super.initState();
    print("STATE INIT");

    // Peroidically check for updates every 3s
    for (BluetoothCharacteristic char in widget.service.characteristics) {
      char.setNotifyValue(true);
      char.value.listen((event) {
        String data = utf8.decode(event, allowMalformed: true);
        if (!data.contains("=")) {
          print("bad packet");
          return;
        }
        final prefix = data.substring(0, data.indexOf("="));

        if (prefix == "TIMESTAMP") {
          setState(() {
            widget.time =
                int.parse(data.substring(data.indexOf("=") + 1)).toString();
          });
        }

        if (prefix == "GPS_LOC") {
          final gps = data.substring(data.indexOf("=") + 1);
          final gpsList = gps.split(",");
          final lat = double.parse(gpsList[0]);
          final lon = double.parse(gpsList[1]);
          setState(() {
            widget.location = LatLng(lat, lon);
            print("GPS: $lat, $lon");
          });
        }

        if (prefix == "ID") {
          final name = data.substring(data.indexOf("=") + 1);
          setState(() {
            widget.name = "RaveBuddy #$name";
          });
        }
        if (prefix == "GPS_SATS") {
          final sat = data.substring(data.indexOf("=") + 1);
          widget.sats = int.parse(sat);
          setState(() {
            widget.sats = int.parse(sat);
          });
        }
      });
    }
  }

  @override
  Widget build(BuildContext context) {
    int sats = widget.sats;
    LatLng location = widget.location;
    String name = widget.name;
    String time = widget.time;
    return Expanded(
        child: Column(children: [
      ListTile(
        title: Text(name),
        subtitle: Text("Sats: $sats, Time: $time"),
        trailing: Icon(Icons.gps_fixed),
      ),
      Expanded(child: MapSample(location: location))
    ]));
  }
}

class MapSample extends StatefulWidget {
  final LatLng location;

  const MapSample({Key? key, required this.location}) : super(key: key);

  @override
  State<MapSample> createState() => MapSampleState();
}

class MapSampleState extends State<MapSample> {
  Completer<GoogleMapController> _controller = Completer();

  @override
  Widget build(BuildContext context) {
    final loc = widget.location;

    if (_controller.isCompleted) {
      _controller.future.then((value) async => value.animateCamera(
          CameraUpdate.newCameraPosition(
              CameraPosition(target: loc, zoom: await value.getZoomLevel()))));
    }
    final pos = CameraPosition(target: loc, zoom: 18.0, tilt: 50.0);
    return Scaffold(
        body: GoogleMap(
      mapType: MapType.hybrid,
      initialCameraPosition: pos,
      onMapCreated: (GoogleMapController controller) {
        _controller.complete(controller);
        //s controller.moveCamera(CameraUpdate.newLatLng(loc));
      },
      markers: {
        Marker(
          markerId: MarkerId("1"),
          position: loc,
        )
      },
    ));
  }
}
