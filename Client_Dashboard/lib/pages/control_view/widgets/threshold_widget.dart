/*
  Project: CEMP Mobility Water Monitoring Station
  Author: Nguyen Trong Tin
  Email:tin.nguyen.31k17@hcmut.edu.vn
  Description: This code is part of the CEMP Mobility Water Monitoring Station project.
*/
// lib/pages/control_view/widgets/threshold_widget.dart

import 'package:flutter/material.dart';
import 'package:water_monitoring_dashboard/utils/slider_utils.dart';
import 'package:water_monitoring_dashboard/model/device_model.dart';
import 'package:water_monitoring_dashboard/widgets/transparent_card.dart';

class ThresholdWidget extends StatefulWidget {
  final double initialValue;
  final Function(double) changeThreshold;
  final DeviceModel device;
  final int selectedDeviceIndex;

  ThresholdWidget({
    Key? key,
    required this.initialValue,
    required this.changeThreshold,
    required this.device,
    required this.selectedDeviceIndex,
  }) : super(key: key);

  @override
  _ThresholdWidgetState createState() => _ThresholdWidgetState();
}

class _ThresholdWidgetState extends State<ThresholdWidget> {
  late double _currentValue;

  @override
  void initState() {
    super.initState();
    _currentValue = widget.initialValue;
  }

  @override
  Widget build(BuildContext context) {
    return Center(
      child: TransparentCard(
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          mainAxisAlignment: MainAxisAlignment.spaceBetween,
          children: [
            Text(
              "Setting Threshold Warning for ${kDeviceName(widget.device.id)}",
              style: const TextStyle(
                  fontSize: 15,
                  color: Colors.white,
                  fontWeight: FontWeight.w500),
            ),
            const SizedBox(height: 5),
            Expanded(
              child: Row(
                mainAxisAlignment: MainAxisAlignment.spaceBetween,
                children: [
                  Text(
                    '${kMinDegree(widget.device.id)} ${kUnit(widget.device.id)}',
                    style: const TextStyle(color: Colors.white),
                  ),
                  Expanded(
                    child: Slider(
                      min: kMinDegree(widget.device.id),
                      max: kMaxDegree(widget.device.id),
                      value: _currentValue,
                      activeColor: Colors.white,
                      inactiveColor: Colors.white30,
                      onChanged: (value) {
                        setState(() {
                          _currentValue = value;
                          widget.changeThreshold(value);
                        });
                      },
                    ),
                  ),
                  Text(
                    '${kMaxDegree(widget.device.id)} ${kUnit(widget.device.id)}',
                    style: const TextStyle(color: Colors.white),
                  ),
                ],
              ),
            ),
            const SizedBox(height: 12),
          ],
        ),
      ),
    );
  }
}
