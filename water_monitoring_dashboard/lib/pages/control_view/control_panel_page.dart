import 'package:animated_background/animated_background.dart';
import 'package:flutter/material.dart';
import 'package:water_monitoring_dashboard/pages/control_view/widgets/option_widget.dart';
import 'package:water_monitoring_dashboard/pages/control_view/options_enum.dart';
import 'package:water_monitoring_dashboard/pages/control_view/widgets/power_widget.dart';
import 'package:water_monitoring_dashboard/pages/control_view/widgets/slider/slider_widget.dart';
import 'package:water_monitoring_dashboard/pages/control_view/widgets/warning_widget.dart';
import 'package:water_monitoring_dashboard/pages/control_view/widgets/temp_widget.dart';
import 'package:water_monitoring_dashboard/utils/slider_utils.dart';
import 'package:water_monitoring_dashboard/widgets/custom_appbar.dart';
import 'package:rainbow_color/rainbow_color.dart';
import 'package:provider/provider.dart';
import 'package:water_monitoring_dashboard/model/device_list_model.dart';
import 'package:water_monitoring_dashboard/model/device_model.dart';

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
  Options option = Options.cooling;
  bool isActive = false;
  int level = 1;
  double value = 0.0;
  double progressVal = 0.49;
  List<DeviceModel> devices = [];

  var activeColor = Rainbow(spectrum: [
    const Color(0xFF33C0BA),
    const Color(0xFF1086D4),
    const Color(0xFF6D04E2),
    const Color(0xFFC421A0),
    const Color(0xFFE4262F)
  ], rangeStart: 0.0, rangeEnd: 1.0);

  @override
  void initState() {
    super.initState();
    devices = Provider.of<DeviceListModel>(context, listen: false).devices;
    value = devices[widget.selectedDeviceIndex].value ?? 0.0;
  }

  @override
  Widget build(BuildContext context) {
    return Consumer<DeviceListModel>(
      builder: (context, deviceList, child) {
        // Update the value when the device value changes
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
          icon: 'assets/svg/clock.svg',
          isSelected: option == Options.timer,
          onTap: () => setState(() {
            option = Options.timer;
          }),
          size: 32,
        ),
        OptionWidget(
          icon: 'assets/svg/snow.svg',
          isSelected: option == Options.cooling,
          onTap: () => setState(() {
            option = Options.cooling;
          }),
          size: 25,
        ),
        OptionWidget(
          icon: 'assets/svg/bright.svg',
          isSelected: option == Options.heat,
          onTap: () => setState(() {
            option = Options.heat;
          }),
          size: 35,
        ),
        OptionWidget(
          icon: 'assets/svg/drop.svg',
          isSelected: option == Options.dry,
          onTap: () => setState(() {
            option = Options.dry;
          }),
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
        TempWidget(
          selectedDeviceIndex: widget.selectedDeviceIndex,
          temp: value,
          changeTemp: (val) => setState(() {
            value = devices[widget.selectedDeviceIndex].value ?? 0.0;
            progressVal = normalize(val, kMinDegree, kMaxDegree);
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
