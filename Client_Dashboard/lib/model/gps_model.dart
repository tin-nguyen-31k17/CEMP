class GPSModel {
  final double longitude;
  final double latitude;

  GPSModel({required this.longitude, required this.latitude});

  factory GPSModel.fromJson(Map<String, dynamic> json) {
    return GPSModel(
      longitude: json['gps_longitude'],
      latitude: json['gps_latitude'],
    );
  }

  Map<String, dynamic> toJson() {
    return {
      'longitude': longitude,
      'latitude': latitude,
    };
  }
}
