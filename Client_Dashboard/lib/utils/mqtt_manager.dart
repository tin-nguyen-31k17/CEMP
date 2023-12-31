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
import 'package:water_monitoring_dashboard/utils/database_helper.dart';

class MQTTManager {
  final String serverUri = "mqttserver.tk";
  final int port = 1883;
  final String clientId = "innovation";
  final String username = "innovation";
  final String password = "Innovation_RgPQAZoA5N";
  final List<String> topics = ["/innovation/watermonitoring"];
  bool _isReconnectScheduled = false;

  late MqttServerClient _client;
  Function(Map<String, dynamic>)? onMessageReceived;
  final LocationService locationService;

  Function()? dataUpdateCallback;
  void setDataUpdateCallback(Function() callback) {
    dataUpdateCallback = callback;
  }

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
    if (!_isReconnectScheduled) {
      _isReconnectScheduled = true;
      Future.delayed(Duration(seconds: 5), () {
        _isReconnectScheduled = false;
        connect();
      });
    }
  }

  void _onSubscribed(String topic) {
    print('Subscribed to topic: $topic');
  }

  void _subscribeToTopic() {
    _client.subscribe(topics[0], MqttQos.atLeastOnce);
    _client.updates!.listen((List<MqttReceivedMessage<MqttMessage>> c) {
      final recMess = c[0].payload as MqttPublishMessage;
      final message = MqttPublishPayload.bytesToStringAsString(recMess.payload.message);
      final decodeMessage = const Utf8Decoder().convert(message.codeUnits);
      print("MQTTClientWrapper:: Decoded message: $decodeMessage");

      final messageJson = jsonDecode(decodeMessage);
      if (onMessageReceived != null) {
        onMessageReceived!(messageJson);
        if (messageJson['sensors'] is List) {
          List sensors = messageJson['sensors'];
          for (var sensor in sensors) {
            var sensorValue = sensor['sensor_value'];
            if (sensorValue != null) {
              var timestamp = DateTime.now().millisecondsSinceEpoch;
              var value = double.tryParse(sensorValue.toString()) ?? 0.0;
              DatabaseHelper.instance.insertSensorData(SensorData(
                timestamp: DateTime.fromMillisecondsSinceEpoch(timestamp),
                value: value,
                sensorId: sensor['sensor_id'],
              ));
              print("Data inserted: Timestamp - $timestamp, Value - $value");
            }
          }
        }
      }
    });
  }

  void disconnect() {
    _client.disconnect();
  }
}
