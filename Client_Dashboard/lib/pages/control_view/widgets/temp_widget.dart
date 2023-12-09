/*
  Project: CEMP Mobility Water Monitoring Station
  Author: Nguyen Trong Tin
  Email:tin.nguyen.31k17@hcmut.edu.vn
  Description: This code is part of the CEMP Mobility Water Monitoring Station project.
*/
import 'package:flutter/material.dart';
import 'package:water_monitoring_dashboard/utils/slider_utils.dart';
import 'package:water_monitoring_dashboard/model/device_model.dart';
import 'package:water_monitoring_dashboard/widgets/transparent_card.dart';

class TempWidget extends StatelessWidget {
  final double temp;
  final Function(double) changeTemp;
  final DeviceModel device;
  final int selectedDeviceIndex;

  const TempWidget({
    Key? key,
    required this.temp,
    required this.changeTemp,
    required this.device,
    required this.selectedDeviceIndex,
  }) : super(key: key);

  @override
  Widget build(BuildContext context) {
    return Center(
      child: TransparentCard(
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          mainAxisAlignment: MainAxisAlignment.spaceBetween,
          children: [
            Text(
              kDeviceName(device.id),
              style: const TextStyle(
                  fontSize: 15,
                  color: Colors.white,
                  fontWeight: FontWeight.w500),
            ),
            const SizedBox(
              height: 5,
            ),
            Row(
              mainAxisAlignment: MainAxisAlignment.spaceBetween,
              children: [
                Text(
                  '${kMinDegree(device.id)} ${kUnit(device.id)}',
                  style: const TextStyle(color: Colors.white),
                ),
                Expanded(
                  child: Slider(
                      min: kMinDegree(device.id),
                      max: kMaxDegree(device.id),
                      value: device.value!,
                      activeColor: Colors.white,
                      inactiveColor: Colors.white30,
                      onChanged: changeTemp),
                ),
                Text(
                  '${kMaxDegree(device.id)} ${kUnit(device.id)}',
                  style: const TextStyle(color: Colors.white),
                ),
              ],
            ),
            const SizedBox(
              height: 12,
            ),
          ],
        ),
      ),
    );
  }
}
