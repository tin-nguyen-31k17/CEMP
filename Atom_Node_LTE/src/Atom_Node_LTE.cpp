/*
    Project: CEMP Mobility Water Monitoring Station
    Author: Nguyen Trong Tin
    Email:tin.nguyen.31k17@hcmut.edu.vn
    Description: This code is part of the CEMP Mobility Water Monitoring Station project.
*/

#include "MQTT_helper.h"
#include "Atom_DTU_CAT1.h"
#include "sensor_data.h"
#include "Atom_Node_LTE.h"

bool newDataReceived = false;

void initVariant() {
  WiFi.mode(WIFI_AP);
  esp_wifi_set_mac(WIFI_IF_AP, &Atom_Mac[0]);
}

void enterSleepMode(unsigned long duration) {
    esp_sleep_enable_timer_wakeup(duration * 1000);
    esp_light_sleep_start();
}

void InitNetwork(void) {
    unsigned long start = millis();
    SerialMon.println("Initializing modem...");
    while (!modem.init()) {
        SerialMon.println("waiting...." + String((millis() - start) / 1000) +
                          "s");
    };

    start = millis();
    SerialMon.println("Waiting for network...");
    while (!modem.waitForNetwork()) {
        SerialMon.println("waiting...." + String((millis() - start) / 1000) +
                          "s");
    }

    SerialMon.println("Waiting for GPRS connect...");
    if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
        SerialMon.println("waiting...." + String((millis() - start) / 1000) +
                          "s");
    }
    SerialMon.println("success");
}

void initDTULTE() {
  SerialAT.begin(SIM7680_BAUDRATE, SERIAL_8N1, ATOM_DTU_SIM7680_RX, ATOM_DTU_SIM7680_TX);
  InitNetwork();
  Serial.println("DTU LTE module initialized!");
}

void OnDataRecv(const uint8_t* mac_addr, const uint8_t* data, int data_len) {
  Serial.println("Data received!");
  if (data_len != 13) {
    Serial.println("Received data of incorrect length.");
  }
  char macStr[24];
  snprintf(macStr, sizeof(macStr), " %02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print("\nData received from: ");
  Serial.println(macStr);

  memcpy(receivedData, data, data_len);

  Serial.print("Decrypted Data: ");
  for (int i = 0; i < data_len; i++) {
    Serial.print("0x");
    Serial.print(receivedData[i], HEX);
    Serial.print(", ");
  }
  Serial.println();
  
  EC = receivedData[0];
  pH = receivedData[1];
  Temp = receivedData[3];
  ORP = receivedData[2];
  Lon = receivedData[4];
  Lat = receivedData[5];
  Day = receivedData[6];
  Month = receivedData[7];
  Year = receivedData[8];
  Hour = receivedData[9];
  Minute = receivedData[10];
  Second = receivedData[11];
  relayStatus = receivedData[12];

  newDataReceived = true; // Set flag to indicate new data received
}

void setup() {
  M5.begin();
  while (!Serial) {};
  Serial.println("\n\nStarting up...");

  initVariant();

  WiFi.mode(WIFI_AP);
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  Serial.print("Channel: ");
  Serial.println(WiFi.channel());
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("STA MAC: ");
  Serial.println(WiFi.macAddress());

  if (esp_now_init() == ESP_OK) {
    Serial.println("ESPNow Initialized!");
    delay(250);
  } else {
    Serial.println("ESPNow Init Failed!");
    delay(250);
    ESP.restart();
  }

  esp_err_t result = esp_now_register_recv_cb(OnDataRecv);
  if (!newDataReceived) {
    Serial.println("No new data received. Delaying...");
    delay(100);
    return;
  }
  if (result == ESP_OK) {
    Serial.println("ESPNow Receiver Callback Registered!");
  } else {
    Serial.println("Failed to register ESPNow Receiver Callback. Error: " + String(result));
  }

  Serial.println();
}

void loop() {
  while (!newDataReceived) {
    Serial.println("No new data received. Delaying...");
    delay(500);
    return;
  }
  
  initDTULTE();
  delay(1000);
  Serial.println("Connecting to MQTT server...");
  myMQTT.connectToMQTT();
  Serial.println("Connected to MQTT server!");
  myMQTT.subscribe(myTopic);
  Serial.println("Subscribed to topic: " + myTopic);

  String data_to_pub = sensorData.createWaterStationJSON(EC, pH, Temp, ORP, Lon, Lat, relayStatus);
  if (myMQTT.publish(myTopic, data_to_pub)) {
    Serial.println("### Message sent successfully via DTU LTE module! ###\n");
    failedSendCount = 0;
  } else {
    Serial.println("### Failed to send via DTU LTE. Entering sleep mode... ###\n");
    enterSleepMode(30 * 60 * 1000);
  }

  newDataReceived = false; // Reset flag after processing data
  delay(DELAY_BETWEEN_READINGS);
  M5.update();
}