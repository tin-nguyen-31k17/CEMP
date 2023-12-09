/*
  Project: CEMP Mobility Water Monitoring Station
  Author: Nguyen Trong Tin
  Email:tin.nguyen.31k17@hcmut.edu.vn
  Description: This code is part of the CEMP Mobility Water Monitoring Station project.
*/

import 'package:flutter/material.dart';
import 'package:water_monitoring_dashboard/widgets/transparent_card.dart';

class WarningWidget extends StatelessWidget {
  final int level;
  final Function(int) changeLevel;

  const WarningWidget(
      {Key? key, required this.level, required this.changeLevel})
      : super(key: key);

  @override
  Widget build(BuildContext context) {
    return TransparentCard(
      child: Column(
        crossAxisAlignment: CrossAxisAlignment.start,
        children: [
          const Text(
            "Warning Level",
            style: TextStyle(
                fontSize: 15, color: Colors.white, fontWeight: FontWeight.w500),
          ),
          const SizedBox(
            height: 5,
          ),
          Row(
            mainAxisAlignment: MainAxisAlignment.spaceBetween,
            children: [
              _button(1, level == 1),
              _button(2, level == 2),
              _button(3, level == 3),
            ],
          ),
        ],
      ),
    );
  }

  ElevatedButton _button(int level, bool isActive) {
    return ElevatedButton(
      style: ElevatedButton.styleFrom(
          foregroundColor: isActive ? Colors.black : Colors.white,
          backgroundColor: isActive ? Colors.white : Colors.transparent,
          minimumSize: const Size(38, 38),
          padding: EdgeInsets.zero,
          shape: const CircleBorder(),
          // padding: const EdgeInsets.symmetric(horizontal: 10, vertical: 5),
          // shape: RoundedRectangleBorder(
          //     borderRadius: BorderRadius.circular(9),
          //     side: BorderSide(
          //         color:
          //             isActive ? Colors.white : Colors.white.withOpacity(0.4),
          //         width: 1)),
          side: BorderSide(color: Colors.white.withOpacity(0.4)),
          elevation: 0),
      onPressed: () => changeLevel(level),
      child: Text(
        level.toString(),
        // level == 1
        //     ? 'Safe'
        //     : level == 2
        //         ? 'Warning'
        //         : 'Danger',
        style: TextStyle(
            fontSize: 13,
            color: isActive ? Colors.black : Colors.white,
            fontWeight: FontWeight.w500),
      ),
    );
  }
}
