import 'package:flutter/material.dart';

class DeviceModel {
  String name;
  Color color;
  bool isActive;
  String icon;
  double? value;
  double? ec;
  double? ph;
  double? temp;
  double? orp;
  double? gps;

  DeviceModel({
    required this.name,
    required this.color,
    required this.isActive,
    required this.icon,
    required this.value,
    this.ec,
    this.ph,
    this.temp,
    this.orp,
    this.gps,
  });

  // Helper function to parse hex color
  static Color _parseHexColor(String hexColor) {
    hexColor = hexColor.toUpperCase().replaceAll("#", "");
    if (hexColor.length == 6) {
      hexColor = "FF$hexColor";
    }
    return Color(int.parse(hexColor, radix: 16));
  }

  // Factory constructor to create DeviceModel from JSON
  factory DeviceModel.fromJson(Map<String, dynamic> json) {
    return DeviceModel(
      name: json['name'] ?? 'default_name',
      color: _parseHexColor(json['color'] ?? '#FFFFFF'), // Use helper function
      isActive: json['isActive'] ?? false,
      icon: json['icon'] ?? 'default_icon',
      value: json['value']?.toDouble(),
      ec: json['ec']?.toDouble(),
      ph: json['ph']?.toDouble(),
      temp: json['temp']?.toDouble(),
      orp: json['orp']?.toDouble(),
      gps: json['gps']?.toDouble(),
    );
  }

  // Convert DeviceModel to JSON
  Map<String, dynamic> toJson() {
    return {
      'name': name,
      'color': color.value.toRadixString(16), // Convert Color to hex string
      'isActive': isActive,
      'icon': icon,
      'value': value,
      'ec': ec,
      'ph': ph,
      'temp': temp,
      'orp': orp,
      'gps': gps,
    };
  }
}
