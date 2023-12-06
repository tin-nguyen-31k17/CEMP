import 'package:flutter/material.dart';
import 'package:mqtt_client/mqtt_client.dart';
import 'package:provider/provider.dart';
import 'package:water_monitoring_dashboard/pages/home/home_page.dart';
import 'package:water_monitoring_dashboard/utils/mqtt_manager.dart';
import 'package:water_monitoring_dashboard/model/device_list_model.dart';
import 'package:water_monitoring_dashboard/model/device_model.dart';

void main() {
  runApp(
    ChangeNotifierProvider(
      create: (context) => DeviceListModel(),
      child: MyApp(),
    ),
  );
}

class MyApp extends StatefulWidget {
  const MyApp({Key? key}) : super(key: key);
  @override
  State<MyApp> createState() => _MyAppState();
}

class _MyAppState extends State<MyApp> {
  late MQTTManager _mqttManager;
  @override
  void initState() {
    super.initState();
    // _mqttManager = MQTTManager();
    // _initializeMQTT();
  }

  // Future<void> _initializeMQTT() async {
  //   await _mqttManager.initializeMQTTClient();
  //   await _mqttManager.connect();
  //   _mqttManager.onMessageReceived = _updateDevices;
  // }

  // void _updateDevices(Map<String, dynamic> messageJson) {
  //   if (messageJson.containsKey('sensors') && messageJson['sensors'] is List) {
  //     setState(() {
  //       for (var sensor in messageJson['sensors']) {
  //         var device = Provider.of<DeviceListModel>(context, listen: false)
  //             .devices
  //             .firstWhere(
  //               (d) => d.name == sensor['sensor_name'],
  //               orElse: () => DeviceModel(
  //                 name: 'Default Sensor',
  //                 color: Colors.grey,
  //                 isActive: false,
  //                 icon: 'default_icon',
  //                 value: 100.0,
  //                 id: 0,
  //               ),
  //             );
  //         device.value = double.parse(sensor['sensor_value']);
  //         device.isActive = true;
  //         Provider.of<DeviceListModel>(context, listen: false)
  //             .notifyListeners();
  //       }
  //     });
  //   } else {
  //     print("The 'sensors' key is missing or not a list.");
  //   }
  // }

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      debugShowCheckedModeBanner: false,
      title: 'CEMP IoT Water Monitoring Dashboard',
      theme: ThemeData(
        fontFamily: "Poppins",
        sliderTheme: const SliderThemeData(
          trackShape: RectangularSliderTrackShape(),
          trackHeight: 2.5,
          thumbShape: RoundSliderThumbShape(enabledThumbRadius: 8.0),
          overlayShape: RoundSliderOverlayShape(overlayRadius: 15.0),
        ),
      ),
      home: const HomePage(),
    );
  }
}
