/*
  Project: CEMP Mobility Water Monitoring Station
  Author: Nguyen Trong Tin
  Email:tin.nguyen.31k17@hcmut.edu.vn
  Description: This code is part of the CEMP Mobility Water Monitoring Station project.
*/

import 'package:animated_background/animated_background.dart';
import 'package:flutter/material.dart';
import 'package:water_monitoring_dashboard/pages/control_view/widgets/option_widget.dart';
import 'package:water_monitoring_dashboard/pages/control_view/options_enum.dart';
import 'package:water_monitoring_dashboard/pages/control_view/widgets/power_widget.dart';
import 'package:water_monitoring_dashboard/pages/control_view/widgets/slider/slider_widget.dart';
import 'package:water_monitoring_dashboard/pages/control_view/widgets/warning_widget.dart';
import 'package:water_monitoring_dashboard/pages/control_view/widgets/threshold_widget.dart';
import 'package:water_monitoring_dashboard/utils/slider_utils.dart';
import 'package:water_monitoring_dashboard/widgets/custom_appbar.dart';
import 'package:rainbow_color/rainbow_color.dart';
import 'package:provider/provider.dart';
import 'package:water_monitoring_dashboard/model/device_list_model.dart';
import 'package:water_monitoring_dashboard/model/device_model.dart';
import 'package:water_monitoring_dashboard/pages/control_view/gps_tracking_page.dart';
import 'package:water_monitoring_dashboard/pages/control_view/graph_panel_page.dart';

class ControlPanelPage extends StatefulWidget {
  final String tag;
  final Color color;
  final int selectedDeviceIndex;

  const ControlPanelPage(
      {Key? key,
      required this.tag,
      required this.color,
      required this.selectedDeviceIndex})
      : super(key: key);
  @override
  _ControlPanelPageState createState() => _ControlPanelPageState();
}

class _ControlPanelPageState extends State<ControlPanelPage>
  with TickerProviderStateMixin {
  Options option = Options.setting;
  bool isActive = false;
  int level = 1;
  double value = 0.0;
  double progressVal = 0.0;
  List<DeviceModel> devices = [];

  var activeColor = Rainbow(spectrum: [
    const Color(0xFF33C0BA),
    const Color(0xFF1086D4),
    const Color(0xFF6D04E2),
    const Color(0xFFC421A0),
    const Color(0xFFE4262F)
  ], rangeStart: 0.0, rangeEnd: 1.0);

  void onGPSOptionSelected() {
    final List<DeviceModel> deviceData =
        Provider.of<DeviceListModel>(context, listen: false).devices;
    if (deviceData[widget.selectedDeviceIndex].gps != null) {
      Navigator.push(
        context,
        MaterialPageRoute(
          builder: (context) => GPSTrackingPage(
            deviceData: deviceData,
            selectedDeviceIndex: widget.selectedDeviceIndex,
          ),
        ),
      );
    } else {
      ScaffoldMessenger.of(context).showSnackBar(
        SnackBar(content: Text('GPS data not available for the selected device')),
      );
    }
  }

  void onGraphOptionSelected() {
    final List<DeviceModel> deviceData =
        Provider.of<DeviceListModel>(context, listen: false).devices;
    if (deviceData[widget.selectedDeviceIndex].gps != null) {
      Navigator.push(
        context,
        MaterialPageRoute(
          builder: (context) => GraphPanelPage(
            deviceData: deviceData,
            selectedDeviceIndex: widget.selectedDeviceIndex,
            color: widget.color,
          ),
        ),
      );
    } else {
      ScaffoldMessenger.of(context).showSnackBar(
        SnackBar(content: Text('History data not available for the selected device')),
      );
    }
  }

  @override
  void initState() {
    super.initState();
    devices = Provider.of<DeviceListModel>(context, listen: false).devices;
    value = devices[widget.selectedDeviceIndex].value ?? 0.0;
    progressVal = normalize(
        value,
        kMinDegree(devices[widget.selectedDeviceIndex].id),
        kMaxDegree(devices[widget.selectedDeviceIndex].id));
  }

  @override
  Widget build(BuildContext context) {
    return Consumer<DeviceListModel>(
      builder: (context, deviceList, child) {
        value = devices[widget.selectedDeviceIndex].value ?? 0.0;
        return Scaffold(
          body: Container(
            width: MediaQuery.of(context).size.width,
            height: MediaQuery.of(context).size.height,
            decoration: BoxDecoration(
              gradient: LinearGradient(
                  begin: Alignment.topCenter,
                  end: Alignment.bottomCenter,
                  colors: <Color>[
                    Colors.white,
                    activeColor[progressVal].withOpacity(0.5),
                    activeColor[progressVal]
                  ]),
            ),
            child: AnimatedBackground(
              behaviour: RandomParticleBehaviour(
                  options: ParticleOptions(
                baseColor: const Color(0xFFFFFFFF),
                opacityChangeRate: 0.25,
                minOpacity: 0.1,
                maxOpacity: 0.3,
                spawnMinSpeed: level * 60.0,
                spawnMaxSpeed: level * 120,
                spawnMinRadius: 2.0,
                spawnMaxRadius: 5.0,
                particleCount: isActive ? level * 150 : 0,
              )),
              vsync: this,
              child: SafeArea(
                child: Padding(
                  padding: const EdgeInsets.fromLTRB(15, 15, 15, 0),
                  child: Column(
                    children: [
                      CustomAppBar(title: widget.tag),
                      const SizedBox(
                        height: 20,
                      ),
                      Expanded(
                        child: Column(
                          mainAxisAlignment: MainAxisAlignment.spaceBetween,
                          children: [
                            options(),
                            slider(),
                            controls(),
                          ],
                        ),
                      )
                    ],
                  ),
                ),
              ),
            ),
          ),
        );
      },
    );
  }

  Widget options() {
    return Row(
      mainAxisAlignment: MainAxisAlignment.spaceAround,
      children: [
        OptionWidget(
          icon: 'assets/svg/graph.svg',
          isSelected: option == Options.graph,
          onTap: () {
            onGraphOptionSelected();
          },
          size: 38,
        ),
        OptionWidget(
          icon: 'assets/svg/control.svg',
          isSelected: option == Options.setting,
          onTap: () => setState(() {
            option = Options.setting;
          }),
          size: 28,
        ),
        OptionWidget(
          icon: 'assets/svg/audit.svg',
          isSelected: option == Options.analyze,
          onTap: () => setState(() {
            option = Options.analyze;
          }),
          size: 34,
        ),
        OptionWidget(
          icon: 'assets/svg/gps.svg',
          isSelected: option == Options.gps,
          onTap: () {
            onGPSOptionSelected();
          },
          size: 28,
        ),
      ],
    );
  }

  Widget slider() {
    return SliderWidget(
      progressVal: progressVal,
      color: activeColor[progressVal],
      device: devices[widget.selectedDeviceIndex],
      onChange: (value) {
        setState(() {
          value = devices[widget.selectedDeviceIndex].value ?? 0.0;
          progressVal = normalize(value, kMinDegree, kMaxDegree);
        });
      },
    );
  }

  Widget controls() {
    return Column(
      children: [
        Row(
          mainAxisAlignment: MainAxisAlignment.spaceBetween,
          children: [
            Expanded(
              child: WarningWidget(
                  level: level,
                  changeLevel: (val) => setState(() {
                        level = val;
                      })),
            ),
            const SizedBox(
              width: 15,
            ),
            Expanded(
              child: PowerWidget(
                  isActive: isActive,
                  onChanged: (val) => setState(() {
                        isActive = val;
                      })),
            ),
          ],
        ),
        const SizedBox(
          height: 15,
        ),
        ThresholdWidget(
          selectedDeviceIndex: widget.selectedDeviceIndex,
          initialValue: devices[widget.selectedDeviceIndex].value ?? 0.0,
          changeThreshold: (value) => setState(() {
            devices[widget.selectedDeviceIndex].value = value;
            progressVal = normalize(value, kMinDegree(devices[widget.selectedDeviceIndex].id), kMaxDegree(devices[widget.selectedDeviceIndex].id));
          }),
          device: devices[widget.selectedDeviceIndex],
        ),
        const SizedBox(
          height: 15,
        ),
      ],
    );
  }
}
