import 'package:google_maps_flutter/google_maps_flutter.dart';

class LocationService {
  late GoogleMapController mapController;

  void onMapCreated(GoogleMapController controller) {
    mapController = controller;
  }

  moveCamera() {
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
}
