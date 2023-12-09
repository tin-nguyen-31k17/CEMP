/*
  Project: CEMP Mobility Water Monitoring Station
  Author: Nguyen Trong Tin
  Email:tin.nguyen.31k17@hcmut.edu.vn
  Description: This code is part of the CEMP Mobility Water Monitoring Station project.
*/

import 'package:flutter/material.dart';
import 'package:flutter/foundation.dart';
import 'device_model.dart';

class DeviceListModel extends ChangeNotifier {
  List<DeviceModel> _devices = [];

  List<DeviceModel> get devices => _devices;

  set devices(List<DeviceModel> devices) {
    _devices = devices;
    notifyListeners();
  }
}
