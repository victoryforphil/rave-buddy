import 'package:flutter/material.dart';
import 'package:flutter_blue/flutter_blue.dart';
import 'package:ravebuddy_app/device_view.dart';

class PageDeviceList extends StatelessWidget {
  const PageDeviceList({Key? key}) : super(key: key);

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        // Here we take the value from the MyHomePage object that was created by
        // the App.build method, and use it to set our appbar title.
        title: Text("Connect to Rave Buddy"),
      ),
      body: DeviceList(),
      floatingActionButton: StreamBuilder<bool>(
        stream: FlutterBlue.instance.isScanning,
        initialData: false,
        builder: (c, snapshot) {
          if (snapshot.data!) {
            return FloatingActionButton(
              child: Icon(Icons.stop),
              onPressed: () => FlutterBlue.instance.stopScan(),
              backgroundColor: Colors.red,
            );
          } else {
            return FloatingActionButton(
                child: Icon(Icons.search),
                onPressed: () => FlutterBlue.instance
                    .startScan(timeout: Duration(seconds: 4)));
          }
        },
      ),
    );
  }
}

class DeviceList extends StatelessWidget {
  const DeviceList({Key? key}) : super(key: key);

  @override
  Widget build(BuildContext context) {
    return RefreshIndicator(
      onRefresh: () =>
          FlutterBlue.instance.startScan(timeout: Duration(seconds: 3)),
      child: ListView(
        physics: const BouncingScrollPhysics(
            parent: AlwaysScrollableScrollPhysics()),
        children: <Widget>[
          StreamBuilder<List<ScanResult>>(
            stream: FlutterBlue.instance.scanResults,
            initialData: [],
            builder: (context, snapshot) => Column(
                children: snapshot.data!
                    .where((d) => d.device.name.contains("RaveBuddy"))
                    .map((d) => DeviceItem(scanResult: d))
                    .toList()),
          ),
        ],
      ),
    );
  }
}

class DeviceItem extends StatefulWidget {
  final ScanResult scanResult;
  const DeviceItem({Key? key, required this.scanResult}) : super(key: key);

  @override
  _DeviceItemState createState() => _DeviceItemState();
}

class _DeviceItemState extends State<DeviceItem> {
  @override
  Widget build(BuildContext context) {
    return GestureDetector(
      onTap: () => {
        Navigator.push(
            context,
            MaterialPageRoute(
                builder: (context) =>
                    PageDeviceView(device: widget.scanResult.device)))
      },
      child: Card(
          child: ListTile(
        leading: Icon(Icons.add_link_sharp),
        title: Text(widget.scanResult.device.name),
        subtitle: Text(widget.scanResult.rssi.toString()),
      )),
    );
  }
}
