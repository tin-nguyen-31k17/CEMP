import 'package:flutter/material.dart';
import 'package:water_monitoring_dashboard/model/device_model.dart';
import 'package:water_monitoring_dashboard/pages/home/widgets/devices.dart';
import 'package:water_monitoring_dashboard/utils/mqtt_manager.dart';

class HomePage extends StatefulWidget {
  const HomePage({Key? key}) : super(key: key);

  @override
  _HomePageState createState() => _HomePageState();
}

class _HomePageState extends State<HomePage> {
  List<DeviceModel> devices = [];

  @override
  void initState() {
    super.initState();
    var mqttManager = MQTTManager(onMessageReceived: _updateDevices);
    mqttManager.initializeMQTTClient();
    mqttManager.connect();
    mqttManager.onMessageReceived = _updateDevices;

    devices = [
      DeviceModel(name: 'EC Sensor', color: Colors.blue, isActive: false, icon: 'assets/svg/IconEC.svg', value: 0.0),
      DeviceModel(name: 'pH Sensor', color: Colors.green, isActive: false, icon: 'assets/svg/IconpH.svg', value: 0.0),
      DeviceModel(name: 'ORP Sensor', color: Colors.red, isActive: false, icon: 'assets/svg/IconORP.svg', value: 0.0),
      DeviceModel(name: 'Temp Sensor', color: Colors.yellow, isActive: false, icon: 'assets/svg/IconTemp.svg', value: 0.0),
    ];
  }

  void _updateDevices(Map<String, dynamic> messageJson) {
    setState(() {
      for (var device in devices) {
        for (var sensor in messageJson['sensors']) {
          if (device.name == sensor['sensor_name']) {
            switch (sensor['sensor_id']) {
              case 'ec_0001':
                device.ec = double.parse(sensor['sensor_value']);
                device.value = device.ec;
                break;
              case 'ph_0001':
                device.ph = double.parse(sensor['sensor_value']);
                device.value = device.ph;
                break;
              case 'ORP_0001':
                device.orp = double.parse(sensor['sensor_value']);
                device.value = device.orp;
                break;
              case 'TEMP_0001':
                device.temp = double.parse(sensor['sensor_value']);
                device.value = device.temp;
                break;
              default:
                break;
            }
          }
        }
      }
    });
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      body: Container(
        width: MediaQuery.of(context).size.width,
        height: MediaQuery.of(context).size.height,
        decoration: const BoxDecoration(
          gradient: LinearGradient(
              begin: Alignment.topLeft,
              end: Alignment.bottomRight,
              colors: <Color>[Color(0xFFfce2e1), Colors.white]),
        ),
        child: Padding(
          padding: const EdgeInsets.fromLTRB(20, 15, 20, 0),
          child: SafeArea(
            child: Column(
              children: [
                const Row(
                  mainAxisAlignment: MainAxisAlignment.spaceBetween,
                  crossAxisAlignment: CrossAxisAlignment.center,
                  children: [
                    Text(
                      "Hi, Tin Nguyen",
                      style: TextStyle(
                          fontSize: 28,
                          color: Colors.black,
                          fontWeight: FontWeight.bold),
                    ),
                    CircleAvatar(
                        minRadius: 16,
                        backgroundImage: AssetImage("assets/images/user.webp"))
                  ],
                ),
                const SizedBox(
                  height: 30,
                ),
                Expanded(
                  child: Container(
                    width: MediaQuery.of(context).size.width,
                    decoration: const BoxDecoration(
                      borderRadius: BorderRadius.only(
                        topRight: Radius.circular(30.0),
                        topLeft: Radius.circular(30.0),
                      ),
                      color: Colors.white,
                    ),
                    child: Padding(
                      padding: const EdgeInsets.all(20.0),
                      child: Column(
                        crossAxisAlignment: CrossAxisAlignment.start,
                        children: [
                          const SizedBox(
                            height: 5,
                          ),
                          Row(
                            mainAxisAlignment: MainAxisAlignment.spaceBetween,
                            crossAxisAlignment: CrossAxisAlignment.start,
                            children: [
                              const Column(
                                crossAxisAlignment: CrossAxisAlignment.start,
                                children: [
                                  Text(
                                    "5 devices added",
                                    style: TextStyle(
                                        fontSize: 15,
                                        color: Colors.grey,
                                        fontWeight: FontWeight.normal),
                                  ),
                                  Text(
                                    "Remote SUV Boat",
                                    style: TextStyle(
                                        height: 1.1,
                                        fontSize: 17,
                                        color: Colors.black,
                                        fontWeight: FontWeight.w600),
                                  ),
                                ],
                              ),
                              Icon(
                                Icons.add_circle_outline,
                                color: Colors.grey[300],
                                size: 35,
                              )
                            ],
                          ),
                          const SizedBox(
                            height: 10,
                          ),
                          Expanded(
                            child: GridView.builder(
                                padding:
                                    const EdgeInsets.only(top: 10, bottom: 20),
                                gridDelegate:
                                    const SliverGridDelegateWithMaxCrossAxisExtent(
                                        maxCrossAxisExtent: 200,
                                        childAspectRatio: 3 / 4,
                                        crossAxisSpacing: 20,
                                        mainAxisSpacing: 20),
                                itemCount: devices.length,
                                itemBuilder: (BuildContext ctx, index) {
                                  return Devices(
                                    name: devices[index].name,
                                    svg: devices[index].icon,
                                    color: devices[index].color,
                                    isActive: devices[index].isActive,
                                    onChanged: (val) {
                                      setState(() {
                                        devices[index].isActive =
                                            !devices[index].isActive;
                                      });
                                    },
                                  );
                                }),
                          ),
                        ],
                      ),
                    ),
                  ),
                ),
              ],
            ),
          ),
        ),
      ),
    );
  }
}