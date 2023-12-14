import 'package:flutter/material.dart';
import 'package:water_monitoring_dashboard/widgets/custom_appbar.dart';
import 'package:water_monitoring_dashboard/model/device_model.dart';
import 'package:fl_chart/fl_chart.dart';

class TimeChartPage extends StatelessWidget {
  final String title;
  final DeviceModel device;

  TimeChartPage({Key? key, required this.title, required this.device})
      : super(key: key);

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      body: Container(
        decoration: BoxDecoration(
          gradient: LinearGradient(
              begin: Alignment.topCenter,
              end: Alignment.bottomCenter,
              colors: <Color>[
                Colors.white,
                Colors.blue.withOpacity(0.5),
                Colors.blue
              ]),
        ),
        child: SafeArea(
          child: Padding(
            padding: const EdgeInsets.fromLTRB(15, 15, 15, 0),
            child: Column(
              children: [
                CustomAppBar(title: title),
                const SizedBox(
                  height: 20,
                ),
                Expanded(
                  child: LineChart(
                    _createSampleData(),
                  ),
                ),
              ],
            ),
          ),
        ),
      ),
    );
  }

  LineChartData _createSampleData() {
    // TODO: Replace this with your actual data
    final data = [
      FlSpot(1, 5),
      FlSpot(2, 25),
      FlSpot(3, 100),
      FlSpot(4, 75),
    ];

    return LineChartData(
      lineBarsData: [
        LineChartBarData(
          spots: data,
          isCurved: true,
          colors: [Colors.blue],
          barWidth: 2,
          isStrokeCapRound: true,
          dotData: FlDotData(show: false),
          belowBarData: BarAreaData(show: false),
        )
      ],
    );
  }
}
