import 'package:water_monitoring_dashboard/model/device_model.dart';

List<DeviceModel> devices = [
  DeviceModel(
    name: 'Device Name',
    color: "#ff5f5f",
    isActive: true,
    icon: 'Device Icon',
    ec: 1.0, // Example value for 'ec'
    ph: 7.0, // Example value for 'ph'
    temp: 25.0, // Example value for 'temp'
    orp: 250.0, // Example value for 'orp'
  ),
  DeviceModel(
    name: 'Smart Spotlight',
    isActive: true,
    color: "#ff5f5f",
    icon: 'assets/svg/light.svg',
    ec: 0.0, // Default value for 'ec'
    ph: 0.0, // Default value for 'ph'
    temp: 0.0, // Default value for 'temp'
    orp: 0.0, // Default value for 'orp'
  ),
  DeviceModel(
    name: 'Smart AC',
    isActive: true,
    color: "#7739ff",
    icon: 'assets/svg/ac.svg',
    ec: 0.0, // Default value for 'ec'
    ph: 0.0, // Default value for 'ph'
    temp: 0.0, // Default value for 'temp'
    orp: 0.0, // Default value for 'orp'
  ),
  DeviceModel(
    name: 'Smart TV',
    isActive: false,
    color: "#c9c306",
    icon: 'assets/svg/tv.svg',
    ec: 0.0, // Default value for 'ec'
    ph: 0.0, // Default value for 'ph'
    temp: 0.0, // Default value for 'temp'
    orp: 0.0, // Default value for 'orp'
  ),
  DeviceModel(
    name: 'Smart Sound',
    isActive: false,
    color: "#c207db",
    icon: 'assets/svg/speaker.svg',
    ec: 0.0, // Default value for 'ec'
    ph: 0.0, // Default value for 'ph'
    temp: 0.0, // Default value for 'temp'
    orp: 0.0, // Default value for 'orp'
  ),
];