#include <TinyGPSPlus.h>
#include <M5Atom.h>
#include <WiFi.h>
#include <esp_now.h>
#include "sensor_data.h"

#define WIFI_CHANNEL 1
#define SENSOR_COUNT 8

#define GPS_RX_PIN 32
#define GPS_TX_PIN 26
#define DELAY_BETWEEN_READINGS 5000

static const uint32_t GPSBaud = 9600;

TinyGPSPlus gps;

HardwareSerial ss(1);

uint8_t Gateway_Mac[] = {0x02, 0x10, 0x11, 0x12, 0x13, 0x14};
volatile boolean messageSent;
uint8_t receivedData[9];
uint8_t dataToSend[SENSOR_COUNT] = {0}; // Array to send sensor readings
float sensorReadings[SENSOR_COUNT] = {0};
float Lon, Lat;

static void smartDelay(unsigned long ms) {
    unsigned long start = millis();
    do {
        while (ss.available()) gps.encode(ss.read());
    } while (millis() - start < ms);
}

static void printInt(unsigned long val, bool valid, int len) {
    char sz[32] = "Waiting... ";
    if (valid) sprintf(sz, "%ld", val);
    sz[len] = 0;
    for (int i = strlen(sz); i < len; ++i) sz[i] = ' ';
    if (len > 0) sz[len - 1] = ' ';
    Serial.print(sz);
    smartDelay(0);
}

static void printFloat(float val, bool valid, int len, int prec) {
    if (!valid) {
        while (len-- > 1) Serial.print('*');
        Serial.print(' ');
    } else {
        Serial.print(val, prec);
        int vi   = abs((int)val);
        int flen = prec + (val < 0.0 ? 2 : 1);  // . and -
        flen += vi >= 1000 ? 4 : vi >= 100 ? 3 : vi >= 10 ? 2 : 1;
        for (int i = flen; i < len; ++i) Serial.print(' ');
    }
    smartDelay(0);
}

static void printChar(char *val, int len) {
    Serial.print(val);
    int slen = strlen(val);
    for (int i = slen; i < len; ++i) Serial.print(' ');
    smartDelay(0);
}

static void printStr(const char *str, int len) {
    int slen = strlen(str);
    for (int i = 0; i < len; ++i) Serial.print(i < slen ? str[i] : ' ');
    smartDelay(0);
}

static void printDateTime(TinyGPSDate &d, TinyGPSTime &t) {
    if (!d.isValid()) {
        Serial.print(F("No Valid Date"));
    } else {
        char sz[32];
        sprintf(sz, "%02d/%02d/%02d ", d.month(), d.day(), d.year());
        Serial.print(sz);
    }

    if (!t.isValid()) {
        Serial.print(F("No Valid Time"));
    } else {
        char sz[32];
        sprintf(sz, "%02d:%02d:%02d ", t.hour(), t.minute(), t.second());
        Serial.print(sz);
    }

    printInt(d.age(), d.isValid(), 5);
    smartDelay(0);
}

static void PrintGPS() {
    printInt(gps.satellites.value(), gps.satellites.isValid(), 5);
    printInt(gps.hdop.value(), gps.hdop.isValid(), 5);
    printFloat(gps.location.lat(), gps.location.isValid(), 11, 6);
    printFloat(gps.location.lng(), gps.location.isValid(), 12, 6);
    printInt(gps.location.age(), gps.location.isValid(), 5);
    printDateTime(gps.date, gps.time);
    printFloat(gps.altitude.meters(), gps.altitude.isValid(), 7, 2);
    printFloat(gps.course.deg(), gps.course.isValid(), 7, 2);
    printFloat(gps.speed.kmph(), gps.speed.isValid(), 6, 2);
    printStr(
        gps.course.isValid() ? TinyGPSPlus::cardinal(gps.course.deg()) : "*** ",
        6);
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
  ss.begin(GPSBaud, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN); 
  while (!Serial) {};
  Serial.println("\n\nStarting up...");

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESPNow initialization failed!");
    delay(100);
  } else {
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

    Lon = gps.location.lng();
    Serial.println("Longitude: " + String(Lon));
    Lat = gps.location.lat();
    Serial.println("Latitude: " + String(Lat));
    String DateTime = String(gps.date.month()) + "/" + String(gps.date.day()) + "/" + String(gps.date.year()) + " " + String(gps.time.hour()) + ":" + String(gps.time.minute()) + ":" + String(gps.time.second());
    Serial.println("Date and Time of measurement: " + DateTime);

    smartDelay(1000);

    if (millis() > 5000 && gps.charsProcessed() < 10)
        Serial.println(F("No GPS data received: check wiring"));
  }

  // Copy sensor readings to dataToSend
  for (int i = 0; i < SENSOR_COUNT; i++) {
    dataToSend[i] = (uint8_t)sensorReadings[i];
  }
  dataToSend[4] = (uint8_t)Lon;
  dataToSend[5] = (uint8_t)Lat;

  Serial.print("Data to be send: ");
  for (int i = 0; i < SENSOR_COUNT; i++) {
    Serial.print("0x");
    Serial.print(dataToSend[i], HEX);
    Serial.print(", ");
  }
  Serial.println();

  esp_now_register_send_cb([](const uint8_t* mac, esp_now_send_status_t sendStatus){
    messageSent = true;
    if (sendStatus == ESP_NOW_SEND_SUCCESS) {
      Serial.println("Message sent successfully!");
      Serial.println();
    } else {
      Serial.println("Message sent failed!");
      Serial.println();
    }
  });

  messageSent = false;

  const uint8_t *peer_addr = gateway.peer_addr;
  esp_err_t result = esp_now_send(peer_addr, dataToSend, sizeof(dataToSend));

  if (result == ESP_OK) {
    Serial.println("Success");
  } else {
    Serial.print("Sending result: ");
    Serial.println(result);
  }

  Serial.println();

  delay(DELAY_BETWEEN_READINGS);
  M5.update();
}