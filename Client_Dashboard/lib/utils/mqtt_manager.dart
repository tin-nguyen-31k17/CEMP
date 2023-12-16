/*
  Project: CEMP Mobility Water Monitoring Station
  Author: Nguyen Trong Tin
  Email:tin.nguyen.31k17@hcmut.edu.vn
  Description: This code is part of the CEMP Mobility Water Monitoring Station project.
*/

import 'dart:convert';
import 'package:mqtt_client/mqtt_client.dart';
import 'package:mqtt_client/mqtt_server_client.dart';
import 'package:water_monitoring_dashboard/service/location_service.dart';
import 'package:water_monitoring_dashboard/model/gps_model.dart';

class MQTTManager {
  final String serverUri = "mqttserver.tk";
  final int port = 1883;
  final String clientId = "innovation";
  final String username = "innovation";
  final String password = "Innovation_RgPQAZoA5N";
  final List<String> topics = ["/innovation/watermonitoring"];

  late MqttServerClient _client;
  Function(Map<String, dynamic>)? onMessageReceived;
  final LocationService locationService;

  MQTTManager({this.onMessageReceived, required this.locationService}) {
    _client = MqttServerClient.withPort(serverUri, clientId, port);
  }

  Future<void> initializeMQTTClient() async {
    _client.logging(on: true);
    _client.keepAlivePeriod = 20;
    _client.onDisconnected = _onDisconnected;
    _client.onConnected = _onConnected;
    _client.onSubscribed = _onSubscribed;

    final connMessage = MqttConnectMessage()
        .authenticateAs(username, password)
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
    _client.subscribe(topics[0], MqttQos.atLeastOnce);
    _client.updates!.listen((List<MqttReceivedMessage<MqttMessage>> c) {
      final recMess = c[0].payload as MqttPublishMessage;
      String message = MqttPublishPayload.bytesToStringAsString(recMess.payload.message);
      String decodeMessage = const Utf8Decoder().convert(message.codeUnits);
      print("MQTTClientWrapper:: Decoded message: $decodeMessage");

      Map<String, dynamic> messageJson = jsonDecode(decodeMessage);
      var gpsData = GPSModel(
        longitude: double.parse(messageJson['gps_longitude']),
        latitude: double.parse(messageJson['gps_latitude']),
      );

      if (onMessageReceived != null) {
        onMessageReceived!(messageJson);
      }

      locationService.moveCamera(10.7720803, 106.6553269);
    });
  }

  void disconnect() {
    _client.disconnect();
  }
}
