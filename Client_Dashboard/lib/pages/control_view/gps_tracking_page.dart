import 'package:flutter/material.dart';
import 'package:google_maps_flutter/google_maps_flutter.dart';
import 'package:water_monitoring_dashboard/service/location_service.dart';
import 'package:water_monitoring_dashboard/model/gps_model.dart';
import 'package:water_monitoring_dashboard/model/device_model.dart';
import 'package:rainbow_color/rainbow_color.dart';
import 'package:animated_background/animated_background.dart';
import 'package:water_monitoring_dashboard/widgets/custom_appbar.dart';
import 'package:water_monitoring_dashboard/model/device_list_model.dart';
import 'package:water_monitoring_dashboard/utils/slider_utils.dart';
import 'package:provider/provider.dart';
import 'dart:math' show cos, sqrt, asin, sin, pow, pi;

double _calculateDistance(LatLng start, LatLng end) {
  const double radius = 6371;
  double lat1 = _degreesToRadians(start.latitude);
  double lon1 = _degreesToRadians(start.longitude);
  double lat2 = _degreesToRadians(end.latitude);
  double lon2 = _degreesToRadians(end.longitude);
  double dlat = lat2 - lat1;
  double dlon = lon2 - lon1;
  double a = pow(sin(dlat / 2), 2) +
      cos(lat1) * cos(lat2) * pow(sin(dlon / 2), 2);
  double c = 2 * asin(sqrt(a));
  return radius * c;
}

double _degreesToRadians(double degrees) {
  return degrees * (pi / 180);
}


double _distanceInKm = 0;

class GPSTrackingPage extends StatefulWidget {
  final List<DeviceModel> deviceData;
  final int selectedDeviceIndex;

  GPSTrackingPage({Key? key, required this.deviceData, required this.selectedDeviceIndex}) : super(key: key); // Add selectedDeviceIndex to constructor

  @override
  _GPSTrackingPageState createState() => _GPSTrackingPageState();
}

class _GPSTrackingPageState extends State<GPSTrackingPage> with TickerProviderStateMixin {
  GoogleMapController? mapController;
  Set<Marker> markers = {};
  Set<Polyline> _polylines = {};

  double progressVal = 0.0;
  int level = 1;

  @override
  void initState() {
    super.initState();
    progressVal = normalize(
      widget.deviceData[widget.selectedDeviceIndex].value?.toDouble() ?? 0,
      kMinDegree(widget.deviceData[widget.selectedDeviceIndex].id),
      kMaxDegree(widget.deviceData[widget.selectedDeviceIndex].id),
    );
  }

  void _onMapCreated(GoogleMapController controller) {
    if (!mounted) return;
    setState(() {
      mapController = controller;
    });
    _updateMarkers();
  }

  void _updateMarkers() {
    final originPosition = LatLng(10.772075, 106.6553269); // Declare outside of setState

    setState(() {
      markers.clear();
      final originMarker = Marker(
        markerId: const MarkerId('origin'),
        position: originPosition,
        infoWindow: const InfoWindow(
          title: 'Origin Location',
          snippet: 'Latitude: 10.772075, Longitude: 106.6553269',
        ),
        icon: BitmapDescriptor.defaultMarkerWithHue(BitmapDescriptor.hueBlue),
      );
      markers.add(originMarker);

      for (var device in widget.deviceData) {
        if (device.gps != null) {
          markers.add(Marker(
            markerId: MarkerId(device.id),
            position: LatLng(device.gps!.latitude, device.gps!.longitude),
            infoWindow: InfoWindow(
              title: device.name,
              snippet: 'Latitude: ${device.gps!.latitude}, Longitude: ${device.gps!.longitude}',
            ),
            icon: BitmapDescriptor.defaultMarkerWithHue(BitmapDescriptor.hueRed),
          ));
        }
      }
    });

    if (widget.deviceData.isNotEmpty && widget.deviceData.first.gps != null) {
      mapController?.animateCamera(
        CameraUpdate.newCameraPosition(
          CameraPosition(
            target: LatLng(widget.deviceData.first.gps!.latitude, widget.deviceData.first.gps!.longitude),
            zoom: 14.0,
          ),
        ),
      );

      final polyline = Polyline(
        polylineId: const PolylineId('route'),
        points: [
          originPosition,
          LatLng(widget.deviceData.first.gps!.latitude, widget.deviceData.first.gps!.longitude),
        ],
        color: Colors.blue,
        width: 5,
      );

      setState(() {
        _polylines.add(polyline);
      });

      // Calculate the distance
      final devicePosition = LatLng(widget.deviceData.first.gps!.latitude, widget.deviceData.first.gps!.longitude);
      _distanceInKm = _calculateDistance(originPosition, devicePosition);

      // Update the UI
      setState(() {});
    }
  }

  Widget _buildDistanceTextBox() {
    return Container(
      padding: EdgeInsets.all(8),
      margin: EdgeInsets.all(8),
      decoration: BoxDecoration(
        color: Colors.white,
        borderRadius: BorderRadius.circular(8),
        boxShadow: [
          BoxShadow(
            color: Colors.black26,
            blurRadius: 4,
            offset: Offset(0, 2),
          ),
        ],
      ),
      child: Text(
        'Distance to Gateway Station: ${_distanceInKm.toStringAsFixed(2)} km',
        style: TextStyle(fontSize: 18),
      ),
    );
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
            child: Column(
              children: [
                _buildDistanceTextBox(),
                Expanded(
                  child: GoogleMap(
                    onMapCreated: _onMapCreated,
                    initialCameraPosition: CameraPosition(
                      target: LatLng(10.7720803, 106.6553269),
                      zoom: 14.0,
                    ),
                    markers: markers,
                    polylines: _polylines,
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
