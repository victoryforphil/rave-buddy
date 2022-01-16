import 'package:flutter/material.dart';
import 'package:flutter_blue/flutter_blue.dart';

class PageDeviceView extends StatefulWidget {
  final BluetoothDevice device;
  const PageDeviceView({Key? key, required this.device}) : super(key: key);

  @override
  _PageDeviceViewState createState() => _PageDeviceViewState();
}

class _PageDeviceViewState extends State<PageDeviceView> {
  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        // Here we take the value from the MyHomePage object that was created by
        // the App.build method, and use it to set our appbar title.
        title: Text(widget.device.name),
      ),
      body: Text(widget.device.id.toString()),
    );
  }
}
