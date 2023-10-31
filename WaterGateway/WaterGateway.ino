#include <M5Atom.h>
#include "sensor_data.h"
#include "MQTT_helper.h"
#include <esp_now.h>
#include <esp_wifi.h>
#include <WiFi.h>
#include <WiFiMulti.h>

#define SOFTAP_SSID       "A-Automator"
#define SOFTAP_PASS       "Cmbuilderx@X"

uint8_t GatewayMac[] = {0x02, 0x10, 0x11, 0x12, 0x13, 0x14};
uint8_t receivedData[7];

// Class Wifi
WiFiMulti wifiMulti;

// Class MyMQTT
MyMQTT myMQTT("mqttserver.tk", "innovation", "Innovation_RgPQAZoA5N");

// Class data json already created

SENSOR_DATA sensorData;

float water_EC = 0, water_PH = 0, water_ORP = 0, water_temp = 0;

void initVariant() {
  WiFi.mode(WIFI_AP); 
  esp_wifi_set_mac(WIFI_IF_AP, &GatewayMac[0]);
}

void setup() {
  M5.begin(true, false, true);
  Serial2.begin(9600, SERIAL_8N1, 22, 19);

  initVariant();

  // Connect to WiFi ------------------------------
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
  Serial.print  ("SSID: "); Serial.println(WiFi.SSID());
  Serial.print  ("Channel: "); Serial.println(WiFi.channel());
  Serial.print  ("IP address: "); Serial.println(WiFi.localIP());
  delay(1000);

  myMQTT.connectToMQTT(); // Connect to MQTT server
  myMQTT.subscribe("/innovation/watermonitoring/");

  // Initialize ESP-Now ---------------------------

  // Config gateway AP - set SSID and channel
  int channel = WiFi.channel();
  if (WiFi.softAP(SOFTAP_SSID, SOFTAP_PASS, channel, 1)) {
    Serial.println("AP Config Success. AP SSID: " + String(SOFTAP_SSID));
  } else {
    Serial.println("AP Config Failed!");
  }

  // Print MAC addresses
  Serial.print("AP MAC: "); Serial.println(WiFi.softAPmacAddress());
  Serial.print("STA MAC: "); Serial.println(WiFi.macAddress());

  // Init ESP-Now
  if (esp_now_init() == ESP_OK) {
    Serial.println("ESPNow Init Successfully!");
  } else {
    Serial.println("ESPNow Init Failed!");
    ESP.restart();
  }
  // ESP-Now is now initialized. Register a callback function for when data is received
  esp_now_register_recv_cb(OnDataRecv);
}

// Callback function for processing received data via ESP-NOW
void OnDataRecv(const uint8_t* mac_addr, const uint8_t* data, int data_len) {
  char macStr[24];
  snprintf(macStr, sizeof(macStr), " %02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print("\nData received from: "); Serial.println(macStr);
  
  // Convert the received data back to a JSON string
  for (int i = 0; i < 7; i++) {
    receivedData[i] = data[i];
  }
  for (int i = 0; i <7 ; i++) {
    Serial.print("0x");
    Serial.print(receivedData[i], HEX);
    Serial.print(", ");
  }
  Serial.println();
  water_EC = (int)receivedData[3]*256 + (int)receivedData[4];
  Serial.print("Water EC: "); 
  Serial.println(water_EC);
}


void loop() {
  myMQTT.checkConnect();
  String data_to_pub;
  data_to_pub = sensorData.createWaterStationJSON(water_EC, 69.0, 69.0, 69.0);

  // Process and publish the received data to MQTT
  myMQTT.publish("/innovation/watermonitoring/", data_to_pub);

  delay(5000);
  myMQTT.checkConnect();
  M5.update();
}
