/*
  Project: CEMP Mobility Water Monitoring Station
  File: WaterGateway.ino
  Author: Nguyen Trong Tin
  Email:tin.nguyen.31k17@hcmut.edu.vn
  Description: This code is part of the CEMP Mobility Water Monitoring Station project. It runs on the gateway device and publish sensor data to the mqttserver.tk
*/

#include <M5Atom.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include "sensor_data.h"
#include "MQTT_helper.h"

#define SOFTAP_SSID "SSID"
#define SOFTAP_PASS "Password"
#define SENSOR_COUNT 4

uint8_t GatewayMac[] = {0x02, 0x10, 0x11, 0x12, 0x13, 0x14};
uint8_t receivedData[7];

// Class Wifi
WiFiMulti wifiMulti;

// Class MyMQTT
MyMQTT myMQTT("mqttserver.tk", "innovation", "Innovation_RgPQAZoA5N");

// Class data json already created
SENSOR_DATA sensorData;

float sensorReadings[SENSOR_COUNT] = {0};

void initVariant() {
  WiFi.mode(WIFI_AP);
  esp_wifi_set_mac(WIFI_IF_AP, &GatewayMac[0]);
}

void setup() {
  M5.begin(true, false, true);
  Serial2.begin(9600, SERIAL_8N1, 22, 19);

  initVariant();

  // Connect to WiFi
  Serial.print("Connecting to WiFi...");

  // Set device in AP mode to begin with
  WiFi.mode(WIFI_AP_STA);

  wifiMulti.addAP(SOFTAP_SSID, SOFTAP_PASS);
  while (wifiMulti.run() != WL_CONNECTED) {
    delay(500);
    Serial.print("Can't connect to network \n");
  }

  // Print WiFi data
  Serial.println("Connected!");
  Serial.println("Set as AP_STA station.");
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  Serial.print("Channel: ");
  Serial.println(WiFi.channel());
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  delay(1000);

  myMQTT.connectToMQTT(); // Connect to MQTT server
  myMQTT.subscribe("/innovation/watermonitoring/");

  // Initialize ESP-Now
  int channel = WiFi.channel();
  if (WiFi.softAP(SOFTAP_SSID, SOFTAP_PASS, channel, 1)) {
    Serial.println("AP Config Success. AP SSID: " + String(SOFTAP_SSID));
  } else {
    Serial.println("AP Config Failed!");
  }

  Serial.print("AP MAC: ");
  Serial.println(WiFi.softAPmacAddress());
  Serial.print("STA MAC: ");
  Serial.println(WiFi.macAddress());

  if (esp_now_init() == ESP_OK) {
    Serial.println("ESPNow Init Successfully!");
  } else {
    Serial.println("ESPNow Init Failed!");
    ESP.restart();
  }

  // ESPNow is now initialized. Register a callback function for when data is received
  esp_now_register_recv_cb(OnDataRecv);
}

void OnDataRecv(const uint8_t* mac_addr, const uint8_t* data, int data_len) {
  char macStr[24];
  snprintf(macStr, sizeof(macStr), " %02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print("\nData received from: ");
  Serial.println(macStr);

  for (int i = 0; i < data_len; i++) {
    receivedData[i] = data[i];
  }

  Serial.print("Data received: ");
  for (int i = 0; i < 7; i++) {
    Serial.print("0x");
    Serial.print(receivedData[i], HEX);
    Serial.print(", ");
  }
  Serial.println();

  int index = 0;
  for (int i = 0; i < SENSOR_COUNT; i++) {
    int highByte = receivedData[index++];
    int lowByte = receivedData[index++];
    sensorReadings[i] = (highByte * 256) + lowByte;
  }

  // Process and publish the received data to MQTT
  String data_to_pub;
  data_to_pub = sensorData.createWaterStationJSON(sensorReadings[0], sensorReadings[1], sensorReadings[2], sensorReadings[3]);
  myMQTT.publish("/innovation/watermonitoring/", data_to_pub);
}

void loop() {
  delay(5000);
  myMQTT.checkConnect();
  M5.update();
}
