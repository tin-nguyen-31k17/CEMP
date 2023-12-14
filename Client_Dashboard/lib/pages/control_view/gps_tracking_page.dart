import 'package:flutter/material.dart';
import 'package:google_maps_flutter/google_maps_flutter.dart';
import 'package:water_monitoring_dashboard/pages/control_view/widgets/option_widget.dart';
import 'package:water_monitoring_dashboard/pages/control_view/options_enum.dart';

class GPSTrackingPage extends StatefulWidget {
  @override
  _GPSTrackingPageState createState() => _GPSTrackingPageState();
}

class _GPSTrackingPageState extends State<GPSTrackingPage> {
  late GoogleMapController mapController;
  Options option = Options.gps;

  final LatLng _center = const LatLng(45.521563, -122.677433);

  void _onMapCreated(GoogleMapController controller) {
    mapController = controller;
    double longitude = 106.6553269; // Fixed longitude
    double latitude = 10.7720803; // Fixed latitude
    mapController.animateCamera(
      CameraUpdate.newCameraPosition(
        CameraPosition(
          target: LatLng(latitude, longitude),
          zoom: 14.0,
        ),
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
              MaterialPageRoute(builder: (context) => GPSTrackingPage()),
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
      extendBodyBehindAppBar: true,
      appBar: AppBar(
        backgroundColor: Colors.transparent,
        elevation: 0,
        centerTitle: true,
        title: const Text('GPS Tracking'),
      ),
      body: SafeArea(
        child: Padding(
          padding: const EdgeInsets.fromLTRB(15, 15, 15, 0),
          child: Column(
            children: [
              options(),
              const SizedBox(
                height: 20,
              ),
              Expanded(
                child: GoogleMap(
                  onMapCreated: _onMapCreated,
                  initialCameraPosition: CameraPosition(
                    target: _center,
                    zoom: 11.0,
                  ),
                ),
              ),
            ],
          ),
        ),
      ),
    );
  }
}
