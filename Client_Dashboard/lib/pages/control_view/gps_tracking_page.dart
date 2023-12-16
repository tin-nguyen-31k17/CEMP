import 'package:flutter/material.dart';
import 'package:google_maps_flutter/google_maps_flutter.dart';
import 'package:water_monitoring_dashboard/pages/control_view/widgets/option_widget.dart';
import 'package:water_monitoring_dashboard/pages/control_view/options_enum.dart';
import 'package:water_monitoring_dashboard/service/location_service.dart';
import 'package:water_monitoring_dashboard/model/gps_model.dart';
import 'package:water_monitoring_dashboard/model/device_model.dart';
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

  GPSTrackingPage({Key? key, required this.deviceData}) : super(key: key);

  @override
  _GPSTrackingPageState createState() => _GPSTrackingPageState();
}

class _GPSTrackingPageState extends State<GPSTrackingPage> {
  Options option = Options.gps;
  GoogleMapController? mapController;
  Set<Marker> markers = {};
  Set<Polyline> _polylines = {};

  void _onMapCreated(GoogleMapController controller) {
    if (!mounted) return;
    setState(() {
      mapController = controller;
    });
    _updateMarkers();
  }

  void _updateMarkers() {
    final originPosition = LatLng(10.7720803, 106.6553269); // Declare outside of setState

    setState(() {
      markers.clear();
      final originMarker = Marker(
        markerId: const MarkerId('origin'),
        position: originPosition,
        infoWindow: const InfoWindow(
          title: 'Origin Location',
          snippet: 'Latitude: 10.7720803, Longitude: 106.6553269',
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
        'Distance to origin: ${_distanceInKm.toStringAsFixed(2)} km',
        style: TextStyle(fontSize: 18),
      ),
    );
  }

  Widget options() {
    return Row(
      mainAxisAlignment: MainAxisAlignment.spaceAround,
      children: [
        OptionWidget(
          icon: 'assets/svg/clock.svg',
          isSelected: option == Options.history,
          onTap: () => setState(() {
            option = Options.history;
          }),
          size: 32,
        ),
        OptionWidget(
          icon: 'assets/svg/control.svg',
          isSelected: option == Options.setting,
          onTap: () => setState(() {
            option = Options.setting;
            Navigator.pop(context);
          }),
          size: 25,
        ),
        OptionWidget(
          icon: 'assets/svg/audit.svg',
          isSelected: option == Options.analyze,
          onTap: () => setState(() {
            option = Options.analyze;
          }),
          size: 35,
        ),
        OptionWidget(
          icon: 'assets/svg/gps.svg',
          isSelected: option == Options.gps,
          onTap: () {
            Navigator.push(
              context,
              MaterialPageRoute(builder: (context) => GPSTrackingPage(deviceData: widget.deviceData)),
            );
          },
          size: 28,
        ),
      ],
    );
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text(
          'GPS Tracking',
          style: TextStyle(color: Colors.black),
        ),
        centerTitle: true,
        backgroundColor: Colors.white.withOpacity(0.5),
        elevation: 0,
      ),
      body: Column(
        children: [
          options(),
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
    );
  }
}
