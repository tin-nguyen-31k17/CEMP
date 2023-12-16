import 'package:google_maps_flutter/google_maps_flutter.dart';

class LocationService {
  GoogleMapController? mapController;

  void onMapCreated(GoogleMapController controller) {
    mapController = controller;
    moveCamera(10.7720803, 106.6553269);
  }

  void moveCamera(double latitude, double longitude) {
    if (mapController == null) {
      print('Error: mapController is not initialized.');
      return;
    }

    mapController!.animateCamera(
      CameraUpdate.newCameraPosition(
        CameraPosition(
          target: LatLng(latitude, longitude),
          zoom: 14.0,
        ),
      ),
    );
  }
}
