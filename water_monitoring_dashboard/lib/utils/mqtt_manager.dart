import 'dart:convert';
import 'package:mqtt_client/mqtt_client.dart';
import 'package:mqtt_client/mqtt_server_client.dart';

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

  Future<void> initializeMQTTClient() async {
    _client.logging(on: true);
    _client.keepAlivePeriod = 20;
    _client.onDisconnected = _onDisconnected;
    _client.onConnected = _onConnected;
    _client.onSubscribed = _onSubscribed;
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
    _client.subscribe(topics[0], MqttQos.atMostOnce);
    _client.updates!.listen((List<MqttReceivedMessage<MqttMessage>> c) {
      final recMess = c[0].payload as MqttPublishMessage;
      String message = MqttPublishPayload.bytesToStringAsString(recMess.payload.message);
      String decodeMessage = const Utf8Decoder().convert(message.codeUnits);
      print("MQTTClientWrapper:: Decoded message: $decodeMessage");
    });
  }

  void _onMessage(List<MqttReceivedMessage<MqttMessage>> c) {
    final recMess = c[0].payload as MqttPublishMessage;
    String message = MqttPublishPayload.bytesToStringAsString(recMess.payload.message);
    String decodeMessage = const Utf8Decoder().convert(message.codeUnits);
    print("MQTTClientWrapper:: Decoded message: $decodeMessage");
    Map<String, dynamic> messageJson = jsonDecode(decodeMessage);
    onMessageReceived!(messageJson);
  }
  void disconnect() {
    _client.unsubscribe(topics[0]);
    _client.disconnect();
  }
}