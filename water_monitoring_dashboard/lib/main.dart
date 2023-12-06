import 'package:flutter/material.dart';
import 'package:provider/provider.dart';
import 'package:water_monitoring_dashboard/pages/home/home_page.dart';
import 'package:water_monitoring_dashboard/utils/mqtt_manager.dart';
import 'package:water_monitoring_dashboard/model/device_list_model.dart';
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
    _mqttManager = MQTTManager();
    _initializeMQTT();
  }
  Future<void> _initializeMQTT() async {
    await _mqttManager.initializeMQTTClient();
    await _mqttManager.connect();
  }
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
