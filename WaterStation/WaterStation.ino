#include <M5Atom.h>
#include "sensor_data.h"
#include "wifi_setup.h"
#include "MQTT_helper.h"

// Class Wifi_esp32
Wifi_esp32 wifi;
// Class MyMQTT
MyMQTT myMQTT("mqttserver.tk", "innovation", "Innovation_RgPQAZoA5N");
// Class data json already created
SENSOR_DATA data;

void setup() {
  M5.begin(true, false, true);
  Serial2.begin(9600, SERIAL_8N1, 22, 19);
  wifi.setupWifi(); // Setup wifi
  myMQTT.connectToMQTT(); // Connect to MQTT server
  myMQTT.subscribe("/innovation/watermonitoring/"); // Subscribe to the feed

}

float water_EC = 0, water_PH = 0, water_ORP = 0, water_temp = 0;

void loop() {
  myMQTT.checkConnect();
  String data_to_pub;
  SENSOR_RS485 data485;

  Serial.println("Writing to water station with data...");
  
  Serial.println("Writing to water conductivity with data...");
  Serial2.write(data485.getDataWATER_EC(), 8);
  delay(1000);
  if (Serial2.available()) {    // If the serial port receives a message. 
    uint8_t receivedData[7];
    Serial2.readBytes(receivedData, sizeof(receivedData));  // Read the message.
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
  delay(1000);
  myMQTT.checkConnect();

  // data.setValueSOIL_STATION(soil_temperature,soil_humidity,soil_PH,soil_EC,soil_N,soil_P,soil_K);
  data_to_pub = data.createWaterStationJSON(water_EC, 69.0, 69.0, 69.0);
  myMQTT.publish("/innovation/watermonitoring/", data_to_pub); // Publish to feed
  Serial.println();
  delay(5000);
  myMQTT.checkConnect();
  M5.update();
}