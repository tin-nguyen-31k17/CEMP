import 'dart:convert';
import 'package:mqtt_client/mqtt_client.dart';
import 'package:mqtt_client/mqtt_server_client.dart';
import 'package:water_monitoring_dashboard/model/device_model.dart';

class MQTTManager {
  final String serverUri = "mqttserver.tk";
  final int port = 1883;
  final String clientId = "innovation";
  final String username = "innovation";
  final String password = "Innovation_RgPQAZoA5N";
  final List<String> topics = ["/innovation/watermonitoring/"];

  late MqttServerClient _client;
  Function(Map<String, dynamic>)? onMessageReceived;

  MQTTManager({this.onMessageReceived}) {
    _client = MqttServerClient.withPort(serverUri, clientId, port);
  }
  
  List<DeviceModel>? get devices => null;

  Future<void> initializeMQTTClient() async {
    _client.logging(on: true);
    _client.keepAlivePeriod = 20;
    _client.onDisconnected = _onDisconnected;
    _client.onConnected = _onConnected;
    _client.onSubscribed = _onSubscribed;
    _client.updates?.listen((List<MqttReceivedMessage<MqttMessage>> c) {
      final MqttPublishMessage recMess = c[0].payload as MqttPublishMessage;
      final String message = MqttPublishPayload.bytesToStringAsString(recMess.payload.message);

      _handleIncomingMessage(message, devices!);
    });
    _client.updates?.listen(_onMessage);

    final connMessage = MqttConnectMessage()
        .authenticateAs(username, password)
        .keepAliveFor(60)
        .withWillTopic('willtopic')
        .withWillMessage('My Will message')
        .startClean()
        .withWillQos(MqttQos.atLeastOnce);

    _client.connectionMessage = connMessage;
  }

  Future<void> connect() async {
    try {
      await _client.connect();
    } on Exception catch (e) {
      print('Exception: $e');
      _client.disconnect();
    }
  }

  void _onConnected() {
    print('Connected to MQTT broker');
    _subscribeToTopic();
  }

  void _onDisconnected() {
    print('Disconnected from MQTT broker');
  }

  void _onSubscribed(String topic) {
    print('Subscribed to topic: $topic');
  }

  void _subscribeToTopic() {
    for (String topic in topics) {
      _client.subscribe(topic, MqttQos.atLeastOnce);
    }
  }

  void _onMessage(List<MqttReceivedMessage<MqttMessage>> c) {
    final MqttPublishMessage recMess = c[0].payload as MqttPublishMessage;
    final String message = MqttPublishPayload.bytesToStringAsString(recMess.payload.message);

    Map<String, dynamic> messageJson;
    try {
      messageJson = json.decode(message);
      print('MQTT message: $message');
    } catch (e) {
      print('Error decoding incoming message: $e');
      return;
    }
    if (onMessageReceived != null) {
      onMessageReceived!(messageJson);
    }
  }

  void _handleIncomingMessage(String message, List<DeviceModel> devices) {
    try {
      final Map<String, dynamic> data = json.decode(message);
      final String stationId = data['station_id'];
      final String stationName = data['station_name'];
      final double gpsLongitude = double.parse(data['gps_longitude']);
      final double gpsLatitude = double.parse(data['gps_latitude']);

      final List<dynamic> sensors = data['sensors'];
      for (var sensorData in sensors) {
        final String sensorId = sensorData['sensor_id'];
        final String sensorName = sensorData['sensor_name'];
        final double sensorValue = double.parse(sensorData['sensor_value']);
        final String sensorUnit = sensorData['sensor_unit'];

        for (var device in devices) {
          if (device.name == sensorName) {
            switch (sensorId) {
              case 'ec_0001':
                device.ec = sensorValue;
                break;
              case 'ph_0001':
                device.ph = sensorValue;
                break;
              case 'ORP_0001':
                device.orp = sensorValue;
                break;
              case 'TEMP_0001':
                device.temp = sensorValue;
                break;
              default:
                break;
            }
          }
        }
      }
    } catch (e) {
      print('Error handling incoming message: $e');
    }
  }

  void disconnect() {
    _client.disconnect();
  }
}