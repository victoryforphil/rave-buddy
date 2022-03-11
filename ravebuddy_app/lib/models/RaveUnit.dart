// Copyright 2019 The Flutter team. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import 'dart:collection';

import 'package:flutter/foundation.dart';

class RaveUnitState {
  int id;
  int rssi;
  double gpsLat;
  double gpsLon;
  int gpsSats;
  int timestamp;
  RaveUnitState(this.id, this.gpsLat, this.gpsLon, this.gpsSats, this.timestamp,
      this.rssi);
}

class RaveUnits extends ChangeNotifier {
  /// Internal, private state of the cart. Stores the ids of each item.
  final Map<int, RaveUnitState> _devices = <int, RaveUnitState>{};

  void onUpdate(RaveUnitState state) {
    int id = state.id;
    if (id == -1) {
      return;
    }
    _devices[id] = state;
    // This line tells [Model] that it should rebuild the widgets that
    // depend on it.
    notifyListeners();
  }

  get devices => _devices.values.toList();
}
