/*
  Project: CEMP Mobility Water Monitoring Station
  Author: Nguyen Trong Tin
  Email:tin.nguyen.31k17@hcmut.edu.vn
  Description: This code is part of the CEMP Mobility Water Monitoring Station project.
*/
import 'package:flutter/material.dart';
import 'gps_model.dart';

class DeviceModel {
  String name;
  Color color;
  bool isActive;
  String icon;
  double? value;
  String id;
  String unit;
  GPSModel? gps;

  DeviceModel({
    required this.name,
    required this.color,
    required this.isActive,
    required this.icon,
    required this.value,
    required this.id,
    required this.unit,
    this.gps,
  });
  static Color _parseHexColor(String hexColor) {
    hexColor = hexColor.toUpperCase().replaceAll("#", "");
    if (hexColor.length == 6) {
      hexColor = "FF$hexColor";
    }
    return Color(int.parse(hexColor, radix: 16));
  }

  factory DeviceModel.fromJson(Map<String, dynamic> json) {
    return DeviceModel(
      name: json['name'] ?? 'default_name',
      color: _parseHexColor(json['color'] ?? '#FFFFFF'),
      isActive: json['isActive'] ?? false,
      icon: json['icon'] ?? 'default_icon',
      value: json['value']?.toDouble(),
      id: json['id'] ?? 0,
      unit: json['unit'] ?? 'default_unit',
    );
  }
  Map<String, dynamic> toJson() {
    return {
      'name': name,
      'color': color.value.toRadixString(16),
      'isActive': isActive,
      'icon': icon,
      'value': value,
      'id': id,
    };
  }
}
