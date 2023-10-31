#include <M5Atom.h>
#include <WiFi.h>
#include <esp_now.h>
#include "sensor_data.h"

#define WIFI_CHANNEL        1

uint8_t Gateway_Mac[] = {0x02, 0x10, 0x11, 0x12, 0x13, 0x14};
volatile boolean messageSent;
uint8_t receivedData[7];
float water_EC = 0, water_PH = 0, water_ORP = 0, water_temp = 0;

void setup() {
  M5.begin(true, false, true);
  Serial2.begin(9600, SERIAL_8N1, 22, 19);
  while (!Serial) {};
  Serial.println("\n\nStarting up...");

  WiFi.mode(WIFI_STA); // Station mode for ESP-NOW sensor node
  WiFi.disconnect();
  
  Serial.printf("My HW mac: %s", WiFi.macAddress().c_str());
  Serial.println("");
  Serial.printf("Sending to MAC: %02x:%02x:%02x:%02x:%02x:%02x", Gateway_Mac[0], Gateway_Mac[1], Gateway_Mac[2], Gateway_Mac[3], Gateway_Mac[4], Gateway_Mac[5]);
  Serial.printf(", on channel: %i\n", WIFI_CHANNEL);

  // Initialize ESPNow ----------------------------
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESPNow initialization failed!");
    delay(100);
  }
}

void loop() {
  SENSOR_RS485 data485;

  Serial.println("Writing to water station with data...");
  Serial.println("Writing to water conductivity with data...");
  Serial2.write(data485.getDataWATER_EC(), 8);
  delay(1000);
  
  if (Serial2.available()) {    
    Serial2.readBytes(receivedData, sizeof(receivedData));
    for (int i = 0; i < 7; i++) {
      Serial.print("0x");
      Serial.print(receivedData[i], HEX);
      Serial.print(", ");
    }
    Serial.println();
    water_EC = (int)receivedData[3] * 256 + (int)receivedData[4];
    Serial.print("Water EC: ");
    Serial.println(water_EC);
  }

  esp_now_peer_info_t gateway;
  memcpy(gateway.peer_addr, Gateway_Mac, 6);
  gateway.channel = WIFI_CHANNEL;
  gateway.encrypt = false; // No encryption
  esp_now_add_peer(&gateway);

  esp_now_register_send_cb([](const uint8_t* mac, esp_now_send_status_t sendStatus){
    // callback for message sent out
    messageSent = true;         // flag message is sent out - we can now safely go to sleep ...
    #ifdef DEBUG_LOG
    Serial.printf("Message sent out, sendStatus = %i\n", sendStatus);
    #endif
  });

  messageSent = false;
  
  #ifdef DEBUG_LOG
  Serial.println("Sending Message Data: " + \
                  data_to_send \
                );
  #endif

  // Send message -----------------------------------
  const uint8_t *peer_addr = gateway.peer_addr;
  esp_err_t result=esp_now_send(peer_addr, (uint8_t *) &receivedData, sizeof(receivedData));
  
  #ifdef DEBUG_LOG
  Serial.print("Sending result: "); Serial.println(result);
  #endif

  Serial.println();
  delay(5000);
  M5.update();
}
