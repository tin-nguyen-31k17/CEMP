import 'dart:async';
import 'dart:math' as math;
import 'package:flutter/material.dart';
import 'package:fl_chart/fl_chart.dart';
import 'package:intl/intl.dart';
import 'package:water_monitoring_dashboard/widgets/custom_appbar.dart';
import 'package:water_monitoring_dashboard/model/device_list_model.dart';
import 'package:water_monitoring_dashboard/model/device_model.dart';
import 'package:water_monitoring_dashboard/utils/slider_utils.dart';
import 'package:provider/provider.dart';
import 'package:rainbow_color/rainbow_color.dart';
import 'package:animated_background/animated_background.dart';

class GraphPanelPage extends StatefulWidget {
  final Color color;
  final int selectedDeviceIndex;
  final List<DeviceModel> deviceData;

  const GraphPanelPage({
    Key? key,
    required this.color,
    required this.selectedDeviceIndex,
    required this.deviceData,
  }) : super(key: key);

  @override
  _GraphPanelPageState createState() => _GraphPanelPageState();
}

class _GraphPanelPageState extends State<GraphPanelPage> with TickerProviderStateMixin {
  List<FlSpot> realTimeData = [];
  Timer? dataUpdateTimer;
  double progressVal = 0.0;
  int level = 1;

  @override
  void initState() {
    super.initState();
    startRealTimeDataUpdates();
    progressVal = normalize(
      widget.deviceData[widget.selectedDeviceIndex].value?.toDouble() ?? 0,
      kMinDegree(widget.deviceData[widget.selectedDeviceIndex].id),
      kMaxDegree(widget.deviceData[widget.selectedDeviceIndex].id),
    );
  }

  @override
  void dispose() {
    dataUpdateTimer?.cancel();
    super.dispose();
  }

  void startRealTimeDataUpdates() {
    dataUpdateTimer = Timer.periodic(Duration(seconds: 1), (timer) {
      final now = DateTime.now();
      final newPoint = FlSpot(
        now.millisecondsSinceEpoch.toDouble(),
        widget.deviceData[widget.selectedDeviceIndex].value?.toDouble() ?? 0,
      );
      setState(() {
        realTimeData.add(newPoint);
      });
    });
  }

  @override
  Widget build(BuildContext context) {
    final activeColor = Rainbow(spectrum: [
      const Color(0xFF33C0BA),
      const Color(0xFF1086D4),
      const Color(0xFF6D04E2),
      const Color(0xFFC421A0),
      const Color(0xFFE4262F),
    ], rangeStart: 0.0, rangeEnd: 1.0);

    return Scaffold(
      appBar: CustomAppBar(title: 'Real-Time Graph of ${widget.deviceData[widget.selectedDeviceIndex].name}'),
      body: AnimatedBackground(
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
            particleCount: 150,
          ),
        ),
        vsync: this,
        child: Container(
          decoration: BoxDecoration(
            gradient: LinearGradient(
              begin: Alignment.topCenter,
              end: Alignment.bottomCenter,
              colors: <Color>[
                Colors.white,
                activeColor[progressVal].withOpacity(0.5),
                activeColor[progressVal],
              ],
            ),
          ),
          child: SafeArea(
            child: Padding(
              padding: const EdgeInsets.all(16),
              child: realTimeData.isNotEmpty
                  ? LineChart(mainData())
                  : Center(child: Text("No data available. Waiting for real-time data...")),
            ),
          ),
        ),
      ),
    );
  }

  LineChartData mainData() {
    final isNotEmpty = realTimeData.isNotEmpty;
    final minX = isNotEmpty ? realTimeData.map((spot) => spot.x).reduce(math.min).toDouble() : 0;
    final maxX = isNotEmpty ? realTimeData.map((spot) => spot.x).reduce(math.max).toDouble() : 0;
    final minY = isNotEmpty ? realTimeData.map((spot) => spot.y).reduce(math.min).toDouble() : 0;
    final maxY = isNotEmpty ? realTimeData.map((spot) => spot.y).reduce(math.max).toDouble() : 0;
    return LineChartData(
      minX: minX as double?,
      maxX: maxX as double?,
      minY: minY as double?,
      maxY: maxY as double?,
      lineTouchData: LineTouchData(
        touchTooltipData: LineTouchTooltipData(
          tooltipBgColor: Colors.blueGrey.withOpacity(0.8),
        ),
        handleBuiltInTouches: true,
      ),
      gridData: FlGridData(
        show: true,
        drawVerticalLine: true,
        drawHorizontalLine: true,
        horizontalInterval: kUnitRange(widget.deviceData[widget.selectedDeviceIndex].id) / 5,
        verticalInterval: 1000 * 60 * 60 * 24 / 5,
      ),
      titlesData: FlTitlesData(
        show: true,
        bottomTitles: AxisTitles(
          sideTitles: SideTitles(
            showTitles: true,
            reservedSize: 30,
            interval: 1000 * 60 * 60 * 24,
            getTitlesWidget: (value, meta) {
              final date = DateTime.fromMillisecondsSinceEpoch(value.toInt());
              return SideTitleWidget(
                child: Text(DateFormat.Hms().format(date)),
                axisSide: meta.axisSide,
              );
            },
          ),
        ),
        leftTitles: AxisTitles(
          sideTitles: SideTitles(
            showTitles: true,
            reservedSize: 30,
            getTitlesWidget: leftTitleWidgets,
          ),
        ),
        topTitles: AxisTitles(sideTitles: SideTitles(showTitles: false)),
        rightTitles: AxisTitles(sideTitles: SideTitles(showTitles: false)),
      ),
      borderData: FlBorderData(
        show: true,
        border: Border.all(
          color: const Color(0xff37434d).withOpacity(0.1),
          width: 1,
        ),
      ),
      lineBarsData: [
        LineChartBarData(
          spots: realTimeData,
          isCurved: true,
          color: widget.color,
          barWidth: 2,
          dotData: FlDotData(show: false),
          belowBarData: BarAreaData(show: true, color: widget.color.withOpacity(0.3)),
        ),
      ],
    );
  }

  Widget bottomTitleWidgets(double value, TitleMeta meta) {
    final date = DateTime.fromMillisecondsSinceEpoch(value.toInt());
    final formattedDate = DateFormat('dd/MM').format(date);

    const style = TextStyle(
      color: Color(0xff68737d),
      fontWeight: FontWeight.bold,
      fontSize: 10,
    );
    return SideTitleWidget(
      axisSide: meta.axisSide,
      space: 4.0,
      child: Text(formattedDate, style: style),
    );
  }

  Widget leftTitleWidgets(double value, TitleMeta meta) {
    const style = TextStyle(
      color: Color(0xff67727d),
      fontWeight: FontWeight.bold,
      fontSize: 10,
    );
    return Text(value.toStringAsFixed(1), style: style);
  }
}
