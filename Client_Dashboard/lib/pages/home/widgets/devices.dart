/*
  Project: CEMP Mobility Water Monitoring Station
  Author: Nguyen Trong Tin
  Email:tin.nguyen.31k17@hcmut.edu.vn
  Description: This code is part of the CEMP Mobility Water Monitoring Station project.
*/

import 'package:animations/animations.dart';
import 'package:flutter/cupertino.dart';
import 'package:flutter/material.dart';
import 'package:water_monitoring_dashboard/pages/control_view/control_panel_page.dart';
import 'package:flutter_svg/svg.dart';
import 'package:provider/provider.dart';
import 'package:water_monitoring_dashboard/model/device_list_model.dart';

class Devices extends StatelessWidget {
  final String name;
  final String svg;
  final Color color;
  final bool isActive;
  final Function(bool) onChanged;
  final double value;
  final String id;
  final String unit;

  const Devices(
      {Key? key,
      required this.name,
      required this.svg,
      required this.color,
      required this.onChanged,
      required this.value,
      required this.isActive,
      required this.unit,
      required this.id})
      : super(key: key);

  @override
  Widget build(BuildContext context) {
    return Consumer<DeviceListModel>(
        builder: (context, deviceListModel, child) {
      return OpenContainer(
          transitionType: ContainerTransitionType.fadeThrough,
          transitionDuration: const Duration(milliseconds: 600),
          closedElevation: 0,
          openElevation: 0,
          openShape: const RoundedRectangleBorder(
              borderRadius: BorderRadius.all(Radius.circular(20.0))),
          closedShape: const RoundedRectangleBorder(
              borderRadius: BorderRadius.all(Radius.circular(20.0))),
          openBuilder: (BuildContext context, VoidCallback _) {
            return ControlPanelPage(
                tag: name,
                color: color,
                selectedDeviceIndex: deviceListModel.devices
                    .indexWhere((element) => element.id == id));
          },
          tappable: true,
          closedBuilder: (BuildContext _, VoidCallback openContainer) {
            return AnimatedContainer(
              duration: const Duration(milliseconds: 300),
              decoration: BoxDecoration(
                borderRadius: const BorderRadius.all(
                  Radius.circular(20.0),
                ),
                border: Border.all(
                  color: Colors.grey[300]!,
                  width: 1.0,
                ),
                color: isActive ? color : Colors.white,
              ),
              child: Padding(
                padding: const EdgeInsets.all(14.0),
                child: Column(
                  crossAxisAlignment: CrossAxisAlignment.start,
                  mainAxisAlignment: MainAxisAlignment.spaceBetween,
                  children: [
                    Column(
                      crossAxisAlignment: CrossAxisAlignment.start,
                      children: [
                        SvgPicture.asset(
                          svg,
                          color: isActive ? Colors.white : Colors.black,
                          height: 38,
                        ),
                        const SizedBox(
                          height: 14,
                        ),
                        SizedBox(
                          width: 65,
                          child: Text(
                            '$name: ${value.toStringAsFixed(1)} $unit',
                            style: TextStyle(
                                height: 1.2,
                                fontSize: 14,
                                color: isActive ? Colors.white : Colors.black,
                                fontWeight: FontWeight.w500),
                          ),
                        ),
                      ],
                    ),
                    Transform.scale(
                      alignment: Alignment.center,
                      scaleY: 0.8,
                      scaleX: 0.85,
                      child: CupertinoSwitch(
                        onChanged: onChanged,
                        value: isActive,
                        activeColor: isActive
                            ? Colors.white.withOpacity(0.4)
                            : Colors.black,
                        trackColor: Colors.black,
                      ),
                    ),
                  ],
                ),
              ),
            );
          });
    });
  }
}
