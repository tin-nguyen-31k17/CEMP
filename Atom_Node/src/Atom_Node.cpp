/*
    Project: CEMP Mobility Water Monitoring Station
    Author: Nguyen Trong Tin
    Email:tin.nguyen.31k17@hcmut.edu.vn
    Description: This code is part of the CEMP Mobility Water Monitoring Station project.
*/

#include "sensor_data.h"
#include "Atom_Node.h"

// Function to check if the message failed to send and back it up
void backupMessage(const uint8_t* data, size_t size) {
    if (backupIndex + size <= BACKUP_BUFFER_SIZE) {
        memcpy(backupBuffer + backupIndex, data, size);
        backupIndex += size;
        Serial.println("Message backed up successfully!");
    } else {
        Serial.println("Backup buffer full. Deleting the oldest data.");
        size_t bytesToDelete = backupIndex + size - BACKUP_BUFFER_SIZE;
        memmove(backupBuffer, backupBuffer + bytesToDelete, backupIndex - bytesToDelete);
        backupIndex -= bytesToDelete;

        // Add new data to the buffer
        memcpy(backupBuffer + backupIndex, data, size);
        backupIndex += size;

        Serial.println("Backup buffer after deletion: " + String(backupIndex) + " bytes");
    }
}

static void smartDelay(unsigned long ms) {
    unsigned long start = millis();
    do {
        while (ss.available()) gps.encode(ss.read());
    } while (millis() - start < ms);
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

// Function to read data from a sensor and store it in an array
void readSensor(SENSOR_RS485& sensor, float& sensorReading, const uint8_t* sensorData) {
 Serial2.write(sensorData, 8);
 delay(1000);

 if (Serial2.available()) {
   Serial2.readBytes(receivedData, sizeof(receivedData));
   sensorReading = decode_32bit(receivedData); // Use the decode_32bit function to parse the sensor data
 }
}

// Function to generate random sensor data
void generateRandomSensorData(float& sensorReading) {
  sensorReading = random(0, 100); // Random sensor reading for testing
}

// Function to generate random date-time, longitude, and latitude data
void generateRandDateTimeGPS() {
  // Generate random date-time
  Day = random(1, 31);
  Month = random(1, 13);
  Year = random(20, 30);
  Hour = random(0, 24);
  Minute = random(0, 60);
  Second = random(0, 60);

  // Generate random longitude and latitude
  Lon = random(0, 360);
  Lat = random(0, 180);

  // Format the date and time as a string
  dateTime = String(Day) + "/" + String(Month) + "/" + "20" + String(Year) + " " + String(Hour) + ":" + String(Minute) + ":" + String(Second);
}

void setup() {
  M5.begin();
  ss.begin(GPSBaud, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN); 
  while (!Serial) {};
  Serial.println("\n\nStarting up...");

  Serial.print("### Setting up ESP-NOW ###");
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESPNow initialization failed!");
    delay(100);
  } else {
    Serial.println("ESPNow initialization successfully!");
    delay(100);
  }
  Serial.println();
}

void loop() {
  SENSOR_RS485 data485;
  esp_now_peer_info_t gateway;
  memcpy(gateway.peer_addr, Gateway_Mac, 6);
  gateway.channel = WIFI_CHANNEL;
  gateway.encrypt = false; // No encryption
  esp_now_add_peer(&gateway);

  const uint8_t *peer_addr = gateway.peer_addr;

  Serial.print("### Reading All Sensor Data ###");
  for (int i = 0; i < SENSOR_COUNT; i++) {
    switch (i) {
      case 0:
        Serial.println("Reading water EC sensor...");
        // readSensor(data485, sensorReadings[i], data485.getDataWATER_EC());
        generateRandomSensorData(sensorReadings[i]);
        Serial.print("Water EC: ");
        break;
      case 1:
        Serial.println("Reading water PH sensor...");
        // readSensor(data485, sensorReadings[i], data485.getDataWATER_PH());
        generateRandomSensorData(sensorReadings[i]);
        Serial.print("Water PH: ");
        break;
      case 2:
        Serial.println("Reading water ORP sensor...");
        // readSensor(data485, sensorReadings[i], data485.getDataWATER_ORP());
        generateRandomSensorData(sensorReadings[i]);
        Serial.print("Water ORP: ");
        break;
      case 3:
        Serial.println("Reading water temperature sensor...");
        // readSensor(data485, sensorReadings[i], data485.getDataWATER_TEMP());
        generateRandomSensorData(sensorReadings[i]);
        Serial.print("Water Temp: ");
        break;
    }
    Serial.println(sensorReadings[i]);
  }
  Serial.println();

  Serial.print("### Reading Date-Time, Longitude, and Latitude ###\n");
  // Generate random date, time, longitude, and latitude
  generateRandDateTimeGPS();
  // Lon = gps.location.lng();
  Serial.print("Longitude: " + String(Lon) + ", ");
  // Lat = gps.location.lat();
  Serial.println("Latitude: " + String(Lat));
  // dateTime = String(gps.date.day()) + "/" + String(gps.date.month()) + "/" + String(gps.date.year()) + " " + String(gps.time.hour()) + ":" + String(gps.time.minute()) + ":" + String(gps.time.second());
  Serial.println("Date and Time of measurement: " + dateTime);
  Serial.println();

  smartDelay(1000);

  if (millis() > 5000 && gps.charsProcessed() < 10) Serial.println(F("No GPS data received: check wiring"));

  // Copy sensor readings to dataToSend
  for (int i = 0; i < SENSOR_COUNT; i++) {
    dataToSend[i] = (uint8_t)sensorReadings[i];
  }

  dataToSend[4] = Lon;
  dataToSend[5] = Lat;
  dataToSend[6] = Day;
  dataToSend[7] = Month;
  dataToSend[8] = Year;
  dataToSend[9] = Hour;
  dataToSend[10] = Minute;
  dataToSend[11] = Second;

  Serial.print("### Sending Message ###\n");
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
      Serial.println("Message sent failed! Backing up the data.");
      backupMessage(dataToSend, sizeof(dataToSend));
      Serial.println();
    }
  });

  if (backupIndex > 0) {
      esp_err_t result = esp_now_send(peer_addr, backupBuffer, backupIndex);

      if (result == ESP_OK) {
          Serial.println("Backup message sent successfully!");
          Serial.println();
          backupIndex = 0; // Reset the backup buffer index after successful send
      } else {
          Serial.println("Backup message sending failed! Data will remain in the backup buffer.");
          Serial.println();
      }
  }

  messageSent = false;

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