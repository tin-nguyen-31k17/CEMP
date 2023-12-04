class DeviceModel {
  String name;
  String color;
  bool isActive;
  String icon;
  double ec; // Electrical Conductivity
  double ph; // pH level
  double temp; // Temperature
  double orp; // Oxidation-Reduction Potential

  DeviceModel({
    required this.name,
    required this.color,
    required this.isActive,
    required this.icon,
    required this.ec,
    required this.ph,
    required this.temp,
    required this.orp,
  });

  // Add a factory constructor to create DeviceModel from JSON
  factory DeviceModel.fromJson(Map<String, dynamic> json) {
    return DeviceModel(
      name: json['name'] ?? 'default_name',
      color: json['color'] ?? 'default_color',
      isActive: json['isActive'] ?? false,
      icon: json['icon'] ?? 'default_icon',
      ec: json['ec']?.toDouble() ?? 0.0,
      ph: json['ph']?.toDouble() ?? 0.0,
      temp: json['temp']?.toDouble() ?? 0.0,
      orp: json['orp']?.toDouble() ?? 0.0,
    );
  }

  // Add a toJson method to convert DeviceModel to JSON
  Map<String, dynamic> toJson() {
    final Map<String, dynamic> data = {
      'name': name,
      'color': color,
      'isActive': isActive,
      'icon': icon,
      'ec': ec,
      'ph': ph,
      'temp': temp,
      'orp': orp,
    };
    return data;
  }
}
