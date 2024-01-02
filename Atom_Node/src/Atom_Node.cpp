#include "sensor_data.h"
#include "Atom_Node.h"

void enterSleepMode(unsigned long duration) {
    esp_sleep_enable_timer_wakeup(duration * 1000);
    esp_light_sleep_start();
}

void onEspNowReceive(const uint8_t *senderMac, const uint8_t *incomingData, int len) {
  Serial.print("Received ESP-NOW message from: ");
  for (int i = 0; i < 6; ++i) {
    Serial.printf("%02X", senderMac[i]);
    if (i < 5) Serial.print(":");
  }
  Serial.println();

  if (len == sizeof(uint8_t)) {
    uint8_t relayCommand = *incomingData;
    Serial.print("Relay Command Received: ");
    Serial.println(relayCommand);

    if (relayCommand == 0) {
      digitalWrite(RELAY_PIN, LOW);
      Serial.println("Relay turned OFF via ESP-NOW message.");
    }
  } else {
    Serial.print("Unexpected message length: ");
    Serial.println(len);
  }
}

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

void readSensor(SENSOR_RS485& sensor, float& sensorReading, const uint8_t* sensorData) {
  RS485.begin(9600, SERIAL_8N1, RS485_RX, RS485_TX);
  RS485.write(sensorData, 8);
  delay(1000);

  if (RS485.available() > 0) {
    Serial.println("Reading response from sensor...");
    RS485.readBytes(receivedData, sizeof(receivedData));
    sensorReading = decode_32bit(receivedData);
  } else {
    Serial.println("No response from sensor!");
  }
}

void OnDataSent(const uint8_t* mac_addr, esp_now_send_status_t status) {
  Serial.print("Last Packet Send Status: ");
  if (status == ESP_NOW_SEND_SUCCESS) {
    Serial.println("Delivery success");
    espNowSendSuccess = true;
  } else {
    Serial.println("Delivery fail");
    espNowSendSuccess = false;
  }
}

void setup() {
  M5.begin();
  ss.begin(GPSBaud, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN); 
  pinMode(RELAY_PIN, OUTPUT);
  while (!Serial) {};
  Serial.println("\n\nStarting up...");

  Serial.print("### Setting up ESP-NOW ###\n");
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESPNow initialization failed!\n");
    delay(100);
  } else {
    Serial.println("ESPNow initialization successfully!");
    delay(100);
  }

  esp_now_register_recv_cb(onEspNowReceive);
  // Register OnDataSent callback
  esp_now_register_send_cb(OnDataSent);

  Serial.println();
}

void loop() {
  SENSOR_RS485 data485;
  // Add Gateway_Mac as a peer
  esp_now_peer_info_t gateway;
  memcpy(gateway.peer_addr, Gateway_Mac, 6);
  gateway.channel = WIFI_CHANNEL;
  gateway.encrypt = false;
  esp_now_add_peer(&gateway);

  // Add Atom_Mac as a peer
  esp_now_peer_info_t atom;
  memcpy(atom.peer_addr, Atom_Mac, 6);
  atom.channel = WIFI_CHANNEL;
  atom.encrypt = false;
  esp_now_add_peer(&atom);

  const uint8_t* targetMac = (failedSendCount >= MAX_FAILED_SENDS) ? atom.peer_addr : gateway.peer_addr;

  if (relayStatus == 1) {
    digitalWrite(RELAY_PIN, HIGH);
    Serial.println("### Relay turned on! Water is being pumped... ###\n");
    delay(10000);
    digitalWrite(RELAY_PIN, LOW);
    Serial.println("### Relay turned off! ###\n");
    Serial.println("### Wait 5 seconds to stabilize water flow... ###\n");
    delay(5000);

    Serial.print("### Reading All Sensor Data ###");
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
    Serial.println();
  } else {
    Serial.println("### Relay is off. Skipping sensor readings. ###\n");
    delay(10000);

  }
  Serial.print("### Reading Date-Time, Longitude, and Latitude ###\n");
  Lon = gps.location.lng();
  Serial.print("Longitude: " + String(Lon) + ", ");
  Lat = gps.location.lat();
  Serial.println("Latitude: " + String(Lat));
  dateTime = String(gps.date.day()) + "/" + String(gps.date.month()) + "/" + String(gps.date.year()) + " " + String(gps.time.hour()) + ":" + String(gps.time.minute()) + ":" + String(gps.time.second());
  Serial.println("Date and Time of measurement: " + dateTime);
  Serial.println();

  smartDelay(1000);

  if (millis() > 5000 && gps.charsProcessed() < 10) Serial.println(F("No GPS data received: check wiring\n"));
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
  dataToSend[12] = relayStatus;

  Serial.print("### Sending Message ###\n");
  for (int i = 0; i < 13; i++) {
    Serial.print("0x");
    Serial.print(dataToSend[i], HEX);
    Serial.print(", ");
  }
  Serial.println();
  
  esp_err_t result = esp_now_send(targetMac, dataToSend, sizeof(dataToSend));
  Serial.println("Sending message to:" + String(targetMac[0], HEX) + ":" + String(targetMac[1], HEX) + ":" + String(targetMac[2], HEX) + ":" + String(targetMac[3], HEX) + ":" + String(targetMac[4], HEX) + ":" + String(targetMac[5], HEX));

  if (result == ESP_OK) {
      Serial.println("Message queued for sending");
      unsigned long startTime = millis();
      while (!espNowSendSuccess && millis() - startTime < 500) {
        // Wait for up to 1000ms for the send confirmation
      }

      if (espNowSendSuccess) {
        Serial.println("### Message sent successfully to " + String(targetMac[0], HEX) + ":" + String(targetMac[1], HEX) + ":" + String(targetMac[2], HEX) + ":" + String(targetMac[3], HEX) + ":" + String(targetMac[4], HEX) + ":" + String(targetMac[5], HEX) + " ###\n");
        messageSent = true;
        failedSendCount = 0;
      } else {
        Serial.println("### Message sending failed! Data will be backed up. ###\n");
        messageSent = false;
        failedSendCount++;
        Serial.println("Failed send count: " + String(failedSendCount));
        backupMessage(dataToSend, sizeof(dataToSend));
      }
    } else {
      Serial.println("Message not queued for sending");
    }

  if (failedSendCount >= MAX_FAILED_SENDS+10) {
    Serial.println("### Max failed sends reached. Entering sleep mode. ###\n");
    enterSleepMode(60);
  }

  if (backupIndex > 0) {
    esp_err_t backupResult = esp_now_send(targetMac, backupBuffer, backupIndex);
    if (backupResult == ESP_OK) {
      Serial.println("Backup message sent successfully!");
      backupIndex = 0;
    } else {
      Serial.println("Backup message sending failed! Data will remain in the backup buffer.");
    }
  }
  Serial.println();

  delay(DELAY_BETWEEN_READINGS);
  M5.update();
}