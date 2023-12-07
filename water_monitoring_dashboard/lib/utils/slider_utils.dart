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

// Replace kMinDegree and kMaxDegree with functions that take a device ID and return the min and max values for that device
double kMinDegree(String deviceId) => deviceRanges[deviceId]?[0] ?? 0.0;
double kMaxDegree(String deviceId) => deviceRanges[deviceId]?[1] ?? 100.0;
