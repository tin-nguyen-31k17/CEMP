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