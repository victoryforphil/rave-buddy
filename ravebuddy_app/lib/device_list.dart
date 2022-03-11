import 'dart:async';

import 'package:flutter/material.dart';
import 'package:flutter_blue/flutter_blue.dart';
import 'package:google_maps_flutter/google_maps_flutter.dart';
import 'package:provider/provider.dart';
import 'package:ravebuddy_app/models/RaveUnit.dart';

class PageDeviceList extends StatelessWidget {
  const PageDeviceList({Key? key}) : super(key: key);

  @override
  Widget build(BuildContext context) {
    var units = context.watch<RaveUnits>();
    List<RaveUnitState> devices = units.devices;
    return Scaffold(
        appBar: AppBar(
          // Here we take the value from the MyHomePage object that was created by
          // the App.build method, and use it to set our appbar title.
          title: Text("Connect to Rave Buddy"),
        ),
        body: Column(
          children: (devices.length > 0
              ? devices.map((device) {
                  return buildItme(device);
                }).toList()
              : [
                  Column(
                    children: [
                      CircularProgressIndicator(),
                      Text(
                        "Searching for nearby Rave Buddies...",
                        textAlign: TextAlign.center,
                      )
                    ],
                  )
                ]),
        ));
  }
}

Widget buildItme(RaveUnitState device) {
  const LatLng loc = const LatLng(45.521563, -122.677433);
  return Card(
    child: Column(
      children: [
        ListTile(
          title: Text("RaveBuddy #${device.id.toString()}"),
          trailing: Text((device.rssi + 100).toString()),
          subtitle: Text(
              "Timestamp: ${((device.timestamp / 10000) - 800).toString()}, Sats: ${device.gpsSats.toString()}"),
        ),
        Expanded(child: MapSample(location: loc)),
      ],
    ),
  );
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
