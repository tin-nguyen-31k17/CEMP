/*
  Project: CEMP Mobility Water Monitoring Station
  Author: Nguyen Trong Tin
  Email:tin.nguyen.31k17@hcmut.edu.vn
  Description: This code is part of the CEMP Mobility Water Monitoring Station project.
*/

import 'dart:math';

double degToRad(num deg) => deg * (pi / 180.0);

double normalize(value, min, max) => ((value - min) / (max - min));

double angleRange(value, min, max) => (value * (max - min) + min);

double scaleValue(double value, double min, double max) {
  value = value.clamp(min, max);
  return (value - min) / (max - min);
}

const double kDiameter = 200;

Map<String, List<double>> deviceRanges = {
  'ec_0001': [0.0, 2000.0],
  'ph_0001': [0.0, 14.0],
  'ORP_0001': [-2000.0, 2000.0],
  'TEMP_0001': [0.0, 100.0],
};

Map<String, String> unit = {
  'ec_0001': 'mS/cm',
  'ph_0001': 'pH',
  'ORP_0001': 'mV',
  'TEMP_0001': '°C',
};

double kMinDegree(String deviceId) => deviceRanges[deviceId]?[0] ?? 0.0;
double kMaxDegree(String deviceId) => deviceRanges[deviceId]?[1] ?? 100.0;

String kUnit(String deviceId) => unit[deviceId] ?? 'default_unit';
String kDeviceName(String deviceId) => '${deviceId.split('_')[0]} Sensor';

double kUnitRange(String deviceId) => kMaxDegree(deviceId) - kMinDegree(deviceId);
