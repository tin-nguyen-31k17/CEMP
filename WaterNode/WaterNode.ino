//#include <M5Stack.h>
#include <M5Atom.h>
#include <WiFi.h>
#include <esp_now.h>
#include <AES.h>
#include "sensor_data.h"
#include "GPSAnalyse.h"

#define WIFI_CHANNEL 1
#define SENSOR_COUNT 4

float Lat;
float Lon;
String Utc;

uint8_t Gateway_Mac[] = {0x02, 0x10, 0x11, 0x12, 0x13, 0x14};
volatile boolean messageSent;
uint8_t receivedData[9];
uint8_t dataToSend[SENSOR_COUNT * 2]; // Array to send sensor readings
float sensorReadings[SENSOR_COUNT] = {0};
HardwareSerial GPSRaw(1);
static const uint32_t GPSBaud = 9600;
GPSAnalyse GPS;

// Define encryption key (16 bytes)
byte encryptionKey[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};

// Function to encrypt data
void encryptData(uint8_t* data, size_t dataLength) {
  AES aes;
  aes.set_key(encryptionKey, sizeof(encryptionKey));
  for (int i = 0; i < dataLength; i += N_BLOCK) {
    aes.encrypt(data + i, data + i);
  }
}

// Function to read GPS data
void readGPS() {
  if (Serial.available()) {
    int c = Serial.read();
    Serial.write(c);
    Serial.printf("Latitude= %.5f \r\n", Lat);
    Serial.printf("Longitude= %.5f \r\n", Lon);
    Serial.printf("DATA= %s \r\n", Utc.c_str());
  }
  if (GPSRaw.available()) {
    int c = GPSRaw.read();
    Serial.write(c);
    Serial.printf("Latitude= %.5f \r\n", Lat);
    Serial.printf("Longitude= %.5f \r\n", Lon);
    Serial.printf("DATA= %s \r\n", Utc.c_str());
  }
  GPS.upDate();
}


// Function to read data from a sensor and store it in an array
//void readSensor(SENSOR_RS485& sensor, float& sensorReading, const uint8_t* sensorData) {
//  Serial2.write(sensorData, 8);
//  delay(1000);
//
//  if (Serial2.available()) {
//    Serial2.readBytes(receivedData, sizeof(receivedData));
//    sensorReading = decode_32bit(receivedData); // Use the decode_32bit function to parse the sensor data
//  }
//}

// For test without real sensor
void generateRandomSensorData(float& sensorReading) {
  sensorReading = random(0, 100);
}

// Function to decode 32-bit sensor data
float decode_32bit(uint8_t receivedData[9]) {
  int A = int(receivedData[5]);
  int B = int(receivedData[6]);
  int C = int(receivedData[3]);
  int D = int(receivedData[4]);
  long long int my_32_bit = A << 24 | B << 16 | C << 8 | D;

  uint8_t sign_bit = (my_32_bit >> 31) & 0x01;
  float exponent = 0;
  for (int j = 30; j > 22; j--) {
    exponent += pow(2, (j - 23)) * ((my_32_bit >> j) & 0x01);
  }

  float mantissa = 0;
  int power_count = -1;
  for (int j = 22; j >= 0; j--) {
    mantissa += pow(2, power_count) * ((my_32_bit >> j) & 0x01);
    power_count--;
  }
  mantissa += 1;
  return pow(-1, sign_bit) * pow(2, exponent - 127) * mantissa;
}

void setup() {
  M5.begin();
  GPSRaw.begin(9600, SERIAL_8N1, 32, 26);
  GPS.setTaskName("GPS");
  GPS.setTaskPriority(2);
  GPS.setSerialPtr(GPSRaw);
  GPS.start();
  while (!Serial) {};
  Serial.println("\n\nStarting up...");

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESPNow initialization failed!");
    delay(100);
  }
  else {
    Serial.println("ESPNow initialization successfully!");
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

  // Read GPS data
  readGPS();

  Serial.print("Latitude: ");
  Serial.println(Lat, 6);
  Serial.print("Longitude: ");
  Serial.println(Lon, 6);

  // Check for valid GPS fix before accessing location data
//  if (GPS.location.isValid()) {
//    // Get latitude and longitude from GPS
//    float latitude, longitude;
//    getGPSData(latitude, longitude);
//
//    Serial.print("Latitude: ");
//    Serial.println(latitude, 6);
//    Serial.print("Longitude: ");
//    Serial.println(longitude, 6);
//  } else {
//    Serial.println("Waiting for valid GPS fix...");
//  }
  for (int i = 0; i < SENSOR_COUNT; i++) {
    switch (i) {
      case 0:
        Serial.println("Reading water EC sensor...");
//        readSensor(data485, sensorReadings[i], data485.getDataWATER_EC());
         generateRandomSensorData(sensorReadings[i]);
        Serial.print("Water EC: ");
        break;
      case 1:
        Serial.println("Reading water PH sensor...");
//        readSensor(data485, sensorReadings[i], data485.getDataWATER_PH());
         generateRandomSensorData(sensorReadings[i]);
        Serial.print("Water PH: ");
        break;
      case 2:
        Serial.println("Reading water ORP sensor...");
//        readSensor(data485, sensorReadings[i], data485.getDataWATER_ORP());
         generateRandomSensorData(sensorReadings[i]);
        Serial.print("Water ORP: ");
        break;
      case 3:
        Serial.println("Reading water temperature sensor...");
//        readSensor(data485, sensorReadings[i], data485.getDataWATER_TEMP());
         generateRandomSensorData(sensorReadings[i]);
        Serial.print("Water Temp: ");
        break;
    }
    Serial.println(sensorReadings[i]);
  }

  // Copy sensor readings to dataToSend
  for (int i = 0; i < SENSOR_COUNT; i++) {
  // dataToSend[index] = (uint8_t)(sensorReadings[i] / 256); // High byte
  // dataToSend[index + 1] = (uint8_t)((int)sensorReadings[i] % 256); // Low byte
    dataToSend[i] = (uint8_t)sensorReadings[i];
  }

  Serial.print("Data to be send: ");
  for (int i = 0; i < 7; i++) {
    Serial.print("0x");
    Serial.print(dataToSend[i], HEX);
    Serial.print(", ");
  }
  Serial.println();

  encryptData(dataToSend, sizeof(dataToSend));

  esp_now_register_send_cb([](const uint8_t* mac, esp_now_send_status_t sendStatus){
    // callback for message sent out
    messageSent = true; // flag message is sent out
    if (sendStatus == ESP_NOW_SEND_SUCCESS) {
      Serial.println("Message sent successfully!");
    } else {
      Serial.println("Message sent failed!");
    }
  });

  messageSent = false;

  // Send message -----------------------------------
  const uint8_t *peer_addr = gateway.peer_addr;
  esp_err_t result = esp_now_send(peer_addr, dataToSend, sizeof(dataToSend));

  if (result == ESP_OK) {
    Serial.println("Success");
  } else {
    Serial.print("Sending result: ");
    Serial.println(result);
  }

  Serial.println();
  delay(5000);
  M5.update();
}
