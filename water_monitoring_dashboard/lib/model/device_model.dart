import 'package:flutter/material.dart';
class DeviceModel {
  String name;
  Color color;
  bool isActive;
  String icon;
  double? value;
  int? id;
  DeviceModel({
    required this.name,
    required this.color,
    required this.isActive,
    required this.icon,
    required this.value,
    required this.id,
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
    );
  }
  Map<String, dynamic> toJson() {
    return {
      'name': name,
      'color': color.value.toRadixString(16),
      'isActive': isActive,
      'icon': icon,
      'value': value,
    };
  }
}