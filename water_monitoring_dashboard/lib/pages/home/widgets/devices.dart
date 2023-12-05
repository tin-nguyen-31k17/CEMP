import 'package:animations/animations.dart';
import 'package:flutter/cupertino.dart';
import 'package:flutter/material.dart';
import 'package:water_monitoring_dashboard/pages/control_view/control_panel_page.dart';
import 'package:flutter_svg/svg.dart';

class Devices extends StatelessWidget {
  final String name;
  final String svg;
  final Color color;
  final bool isActive;
  final Function(bool) onChanged;

  const Devices(
      {Key? key,
      required this.name,
      required this.svg,
      required this.color,
      required this.onChanged,
      required this.isActive})
      : super(key: key);
      // get correcsponding value from the device, for example: EC Sensor -> ec, pH Sensor -> ph, etc.
      String get value { 
        switch (name) {
          case 'EC Sensor':
            return '0.0';
          case 'pH Sensor':
            return '0.0';
          case 'ORP Sensor':
            return '0.0';
          case 'Temp Sensor':
            return '0.0';
          default:
            return '0.0';
        }
      }

  @override
  Widget build(BuildContext context) {
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
              tag: name, color: color);
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
                width: 0.6,
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
                        height: 30,
                      ),
                      const SizedBox(
                        height: 14,
                      ),
                      SizedBox(
                        width: 65,
                        child: Text(
                          name,
                          style: TextStyle(
                              height: 1.2,
                              fontSize: 14,
                              color: isActive ? Colors.white : Colors.black,
                              fontWeight: FontWeight.w500),
                        ),
                      ),
                      Text(
                        'Value: $value', // Replace 'value' with the actual value of the device
                        style: TextStyle(
                            height: 1.2,
                            fontSize: 14,
                            color: isActive ? Colors.white : Colors.black,
                            fontWeight: FontWeight.w500),
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
  }
}
