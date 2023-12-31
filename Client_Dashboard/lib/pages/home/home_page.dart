/*
  Project: CEMP Mobility Water Monitoring Station
  Author: Nguyen Trong Tin
  Email:tin.nguyen.31k17@hcmut.edu.vn
  Description: This code is part of the CEMP Mobility Water Monitoring Station project.
*/

import 'package:flutter/material.dart';
import 'package:water_monitoring_dashboard/model/device_model.dart';
import 'package:water_monitoring_dashboard/pages/home/widgets/devices.dart';
import 'package:water_monitoring_dashboard/utils/mqtt_manager.dart';
import 'package:water_monitoring_dashboard/model/device_list_model.dart';
import 'package:water_monitoring_dashboard/model/gps_model.dart';
import 'package:water_monitoring_dashboard/service/location_service.dart';
import 'package:provider/provider.dart';

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
    final locationService = LocationService();
    final mqttManager = MQTTManager(
      onMessageReceived: _updateDevices,
      locationService: locationService,
    );

    mqttManager.initializeMQTTClient().then((value) => mqttManager.connect());

    devices = [
      DeviceModel(
          name: 'EC Sensor',
          color: Colors.blue,
          isActive: false,
          icon: 'assets/svg/IconEC.svg',
          value: 0.0,
          id: 'EC_01',
          unit: 'mS/cm'),
      DeviceModel(
          name: 'pH Sensor',
          color: Colors.green,
          isActive: false,
          icon: 'assets/svg/IconpH.svg',
          value: 0.0,
          id: 'PH_01',
          unit: 'pH'),
      DeviceModel(
          name: 'Temp Sensor',
          color: Colors.grey,
          isActive: false,
          icon: 'assets/svg/IconTemp.svg',
          value: 0.0,
          id: 'TEMP_01',
          unit: '°C'),
      DeviceModel(
          name: 'ORP Sensor',
          color: Colors.red,
          isActive: false,
          icon: 'assets/svg/IconORP.svg',
          value: 0.0,
          id: 'ORP_01',
          unit: 'mV'),
    ];

    Future.delayed(Duration.zero, () {
      Provider.of<DeviceListModel>(context, listen: false).devices = devices;
    });
  }

  void _updateDevices(Map<String, dynamic> messageJson) {
    if (messageJson.containsKey('sensors') && messageJson['sensors'] is List) {
      setState(() {
        for (var sensor in messageJson['sensors']) {
          var device = devices.firstWhere(
            (d) => d.id == sensor['sensor_id'],
            orElse: () {
              print('Device not found for sensor: ${sensor['sensor_id']}');
              return DeviceModel(
                name: 'Default Sensor',
                color: Colors.grey,
                isActive: false,
                icon: 'default_icon',
                value: 0.0,
                id: 'default_id',
                unit: 'default_unit',
              );
            },
          );
          device.value = double.tryParse(sensor['sensor_value'].toString()) ?? 0.0;
          device.isActive = true;
          print("Updated device: ${device.name}, Value: ${device.value}");
        }

        GPSModel gpsModel = GPSModel(
          longitude: double.tryParse(messageJson['gps_longitude'].toString()) ?? 0.0,
          latitude: double.tryParse(messageJson['gps_latitude'].toString()) ?? 0.0,
        );

        for (var device in devices) {
          device.gps = gpsModel;
        }
      });
    } else {
      print("The 'sensors' key is missing or not a list.");
    }
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
                                    "4 devices added",
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
                                    value: devices[index].value ?? 0.0,
                                    isActive: devices[index].isActive,
                                    id: devices[index].id,
                                    unit: devices[index].unit,
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
