#include <M5Atom.h>
#include <WiFi.h>
#include <esp_now.h>
#include "sensor_data.h"

#define WIFI_CHANNEL 1
#define SENSOR_COUNT 4

uint8_t Gateway_Mac[] = {0x02, 0x10, 0x11, 0x12, 0x13, 0x14};
volatile boolean messageSent;
uint8_t receivedData[7];
float sensorReadings[SENSOR_COUNT] = {0};

// Function to read data from a sensor and store it in an array
void readSensor(SENSOR_RS485& sensor, float& sensorReading, const uint8_t* sensorData) {
  Serial2.write(sensorData, 8);
  delay(1000);

  if (Serial2.available()) {
    Serial2.readBytes(receivedData, sizeof(receivedData));
    sensorReading = (int)receivedData[3] * 256 + (int)receivedData[4];
  }
}

void setup() {
  M5.begin(true, false, true);
  Serial2.begin(9600, SERIAL_8N1, 22, 19);
  while (!Serial) {};
  Serial.println("\n\nStarting up...");

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESPNow initialization failed!");
    delay(100);
  }
}

void loop() {
  SENSOR_RS485 data485;
  esp_now_peer_info_t gateway;
  memcpy(gateway.peer_addr, Gateway_Mac, 6);
  gateway.channel = WIFI_CHANNEL;
  gateway.encrypt = false; // No encryption
  esp_now_add_peer(&gateway);

  for (int i = 0; i < SENSOR_COUNT; i++) {
    switch (i) {
      case 0:
        Serial.println("Reading water EC sensor...");
        readSensor(data485, sensorReadings[i], data485.getDataWATER_EC());
        Serial.print("Water EC: ");
        break;
      case 1:
        Serial.println("Reading water PH sensor...");
        readSensor(data485, sensorReadings[i], data485.getDataWATER_PH());
        Serial.print("Water PH: ");
        break;
      case 2:
        Serial.println("Reading water ORP sensor...");
        readSensor(data485, sensorReadings[i], data485.getDataWATER_ORP());
        Serial.print("Water ORP: ");
        break;
      case 3:
        Serial.println("Reading water temperature sensor...");
        readSensor(data485, sensorReadings[i], data485.getDataWATER_TEMP());
        Serial.print("Water Temp: ");
        break;
    }
    Serial.println(sensorReadings[i]);
  }

  esp_now_register_send_cb([](const uint8_t* mac, esp_now_send_status_t sendStatus){
    // callback for message sent out
    messageSent = true; // flag message is sent out
    Serial.printf("Message sent out, sendStatus = %i\n", sendStatus);
  });

  messageSent = false;

  // Send message -----------------------------------
  const uint8_t *peer_addr = gateway.peer_addr;
  esp_err_t result = esp_now_send(peer_addr, (uint8_t *) &receivedData, sizeof(receivedData));
  
  Serial.print("Sending result: "); Serial.println(result);

  Serial.println();
  delay(5000);
  M5.update();
}
