/*
  Project: CEMP Mobility Water Monitoring Station
  File: WaterStation.cpp
  Author: Nguyen Trong Tin
  Email:tin.nguyen.31k17@hcmut.edu.vn
  Description: This code is part of the CEMP Mobility Water Monitoring Station project. It is used to monitor several sensors and publish the data to MQTT along with a nice UI to show sensor data and the battery level.
*/

#include "Core_Station.h"
#include "MQTT_helper.h"
#include "sensor_data.h"
#include "tools.h"
#include "images.h"
#include "font.h"
#include "functions.h"

WiFiMulti wifiMulti;

MyMQTT myMQTT("mqttserver.tk", "innovation", "Innovation_RgPQAZoA5N");

SENSOR_DATA sensorData;

void initVariant() {
  WiFi.mode(WIFI_AP);
  esp_wifi_set_mac(WIFI_IF_AP, &GatewayMac[0]);
}

void printSDCardInfo() {
    uint32_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("SD Card Size: %uMB\n", cardSize);
    M5.Displays(0).drawString("SD Card Size: " + String(cardSize) + "MB", 160, 100);
    delay(2000);
    M5.Displays(0).drawString("", 160, 100);
    delay(250);
}

bool isSDCardFull() {
    uint32_t freeSpace = SD.totalBytes() - SD.usedBytes();
    return freeSpace == 0;
}

void formatSDCard() {
    File root = SD.open("/");
    if (!root) {
        Serial.println("Failed to open directory");
        return;
    }
    while (true) {
        File file = root.openNextFile();
        if (!file) {
            // no more files
            break;
        }
        SD.remove(file.name());
        file.close();
    }
}

void writeDataToSDCard(uint8_t* data, size_t length) {
    File file = SD.open("/data.txt", FILE_APPEND);
    if (!file) {
        Serial.println("Failed to open file for writing");
        return;
    }
    for (size_t i = 0; i < length; i++) {
        file.printf("%02X", data[i]);
    }
    file.println();
    file.close();
}

void OnDataRecv(const uint8_t* mac_addr, const uint8_t* data, int data_len) {
  char macStr[24];
  snprintf(macStr, sizeof(macStr), " %02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print("\nData received from: ");
  Serial.println(macStr);

  // Copy the received data
  memcpy(receivedData, data, data_len);
  // Switch to SD card mode
  SPI_MODE_SDCARD;
  // Write the received data to the SD card
  writeDataToSDCard(receivedData, data_len);
  // Switch back to LCD mode
  SPI_MODE_LCD;
  // Print the SD card information to the screen
  printSDCardInfo();
  // Check if the SD card is full
  if (isSDCardFull()) {
    // If the SD card is full, format and rewrite the data
    formatSDCard();
  }

  Serial.print("Decrypted Data: ");
  for (int i = 0; i < 7; i++) {
    Serial.print("0x");
    Serial.print(receivedData[i], HEX);
    Serial.print(", ");
  }
  Serial.println();
  
  for (int i = 0; i < SENSOR_COUNT; i++) {
    sensorReadings[i]=receivedData[i];
  }

  String data_to_pub;
  data_to_pub = sensorData.createWaterStationJSON(sensorReadings[0], sensorReadings[1], sensorReadings[2], sensorReadings[3], sensorReadings[4], sensorReadings[5]);
  myMQTT.publish("/innovation/watermonitoring/", data_to_pub);
}

void setup() {
  Serial.begin(115200);

  preferences.begin(NAME);
  brightness = preferences.getUInt("brightness", BRIGHTNESS);
  Serial.printf("Brightness = %d\n", brightness);

  auto cfg = M5.config();

  cfg.clear_display = true;
  cfg.internal_imu  = true;
  cfg.internal_rtc  = true;
  cfg.internal_spk  = true;
  cfg.internal_mic  = true;
  cfg.external_imu  = false;
  cfg.external_rtc  = false;

  cfg.external_display.module_display = true;
  cfg.external_display.atom_display   = true;
  cfg.external_display.unit_oled      = false;
  cfg.external_display.unit_lcd       = false;
  cfg.external_display.unit_rca       = false;
  cfg.external_display.module_rca     = false;

  M5.begin(cfg);

  #if BOARD != CORES3
    if (M5.getBoard() == m5::board_t::board_M5Stack) {
      FastLED.addLeds<NEOPIXEL, 15>(leds,
                                    NUM_LEDS);
    } else if (M5.getBoard() == m5::board_t::board_M5StackCore2) {
      FastLED.addLeds<NEOPIXEL, 25>(leds,
                                    NUM_LEDS);
    }
    FastLED.setBrightness(16);
  #endif

  viewUI();

  viewBattery();

  #if BOARD != CORES3
    initLed();
  #endif

  initPhysical();

  initVariant();

  if (!SD.begin(SD_CS)) {
    Serial.println("Card Mount Failed");
    M5.Displays(0).drawString("Card Mount Failed", 160, 100);
    delay(2000);
  } else {
    Serial.println("SD Card Initialized");
    M5.Displays(0).drawString("SD Card Initialized", 160, 100);
    delay(2000);
  }

  Serial.print("Connecting to WiFi...");
  M5.Displays(0).drawString("Connecting to " + String(SOFTAP_SSID) + "...", 160, 100);
  delay(2000);
  M5.Displays(0).drawString("", 160, 100);
  delay(250);

  WiFi.mode(WIFI_AP_STA);

  wifiMulti.addAP(SOFTAP_SSID, SOFTAP_PASS);
  while (wifiMulti.run() != WL_CONNECTED) {
    delay(500);
    Serial.print("Can't connect to network \n");
    M5.Displays(0).drawString("Can't connect to " + String(SOFTAP_SSID) + "!" , 160, 100);
    delay(2000);
    M5.Displays(0).drawString("", 160, 100);
    delay(250);
  }

  Serial.println("Connected!");
  M5.Displays(0).drawString("Connected to " + String(SOFTAP_SSID) + "!", 160, 100);
  delay(2000);
  M5.Displays(0).drawString("", 160, 100);
  delay(250);
  Serial.println("Set as AP_STA station.");
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  Serial.print("Channel: ");
  Serial.println(WiFi.channel());
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  delay(250);

  myMQTT.connectToMQTT();
  myMQTT.subscribe("/innovation/watermonitoring/");

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
    M5.Displays(0).drawString("ESPNow Init Successfully!", 160, 100);
    delay(2000);
    M5.Displays(0).drawString("", 160, 100);
    delay(250);
  } else {
    Serial.println("ESPNow Init Failed!");
    M5.Displays(0).drawString("ESPNow Init Failed!", 160, 100);
    delay(2000);
    M5.Displays(0).drawString("", 160, 100);
    delay(250);
    ESP.restart();
  }

  esp_now_register_recv_cb(OnDataRecv);

  xTaskCreatePinnedToCore(button,
                          "button",
                          8192,
                          NULL,
                          4,
                          NULL,
                          1);

  #if BOARD != CORES3
    xTaskCreatePinnedToCore(led,
                            "led",
                            1024,
                            NULL,
                            4,
                            NULL,
                            1);
  #endif
}

void loop() {
  viewUI();
  viewBattery();

  float EC, pH, Temp, ORP;
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

  if (EC == 0 && pH == 0 && Temp == 0 && ORP == 0) {
    EC = random(0, 100);
    pH = random(0, 14);
    Temp = random(0, 50);
    ORP = random(0, 1000);
  }

  if (Lon == 0 && Lat == 0) {
    Lon = 106.99;
    Lat = 10.2;
    Day = random(1, 31);
    Month = random(1, 12);
    Year = random(20, 30);
    Hour = random(0, 24);
    Minute = random(0, 60);
    Second = random(0, 60);
  }

  dateTimeGPS = "DateTime: " + String(Day) + "/" + String(Month) + "/" + "20" + String(Year) + " " + String(Hour) + ":" + String(Minute) + ":" + String(Second) + " GPS: " + String(float(Lon)) + ":" + String(float(Lat));

  String data_to_pub;
  data_to_pub = sensorData.createWaterStationJSON(EC, pH, ORP, Temp, Lon, Lat);
  myMQTT.publish("/innovation/watermonitoring/", data_to_pub);

  M5.Displays(0).setFont(&arial6pt7b);
  M5.Displays(0).setTextColor(TFT_WHITE, TFT_SCREEN_BG);
  M5.Displays(0).setTextDatum(CC_DATUM);
  M5.Displays(0).setTextPadding(20);
  M5.Displays(0).drawString(dateTimeGPS, 160, 96);

  M5.Displays(0).setFont(&digital_7__mono_24pt7b);
  M5.Displays(0).setTextDatum(CL_DATUM);
  M5.Displays(0).setTextPadding(40);
  if (EC > 19 || EC < 9) {
    M5.Displays(0).drawString(String(int(EC)), 90, 50);
  } else {
    M5.Displays(0).drawString(String(int(EC)), 80, 50);
  }
  M5.Displays(0).setTextColor(TFT_WHITE, TFT_SCREEN_BG);
  if (pH > 19 || pH < 9) {
    M5.Displays(0).drawString(String(int(pH)), 248, 50);
  } else {
    M5.Displays(0).drawString(String(int(pH)), 244, 50);
  }
  M5.Displays(0).setTextColor(TFT_SKYBLUE, TFT_SCREEN_BG);
  if (Temp > 19 || Temp < 9) {
    M5.Displays(0).drawString(String(int(Temp)), 90, 195);
  } else {
    M5.Displays(0).drawString(String(int(Temp)), 80, 195);
  }
  M5.Displays(0).setTextColor(TFT_ORANGE, TFT_SCREEN_BG);
  if (ORP < 9 || ORP > 19) {
    M5.Displays(0).drawString(String(int(ORP)), 248, 195);
  } else {
    M5.Displays(0).drawString(String(int(ORP)), 244, 195);
  }

  M5.Displays(0).setFont(&arial6pt7b);
  M5.Displays(0).setTextColor(TFT_WHITE, TFT_SCREEN_BG);
  M5.Displays(0).setTextDatum(CL_DATUM);
  M5.Displays(0).setTextPadding(0);

  M5.Displays(0).setTextColor(TFT_PINK, TFT_SCREEN_BG);
  if (EC > 9) {
    M5.Displays(0).drawString("mS/cm", 135, 63);
  }
  if (EC > 99) {
    M5.Displays(0).drawString("mS/cm", 145, 63);
  } else {
    M5.Displays(0).drawString("mS/cm", 125, 63);
  }

  M5.Displays(0).setTextColor(TFT_WHITE, TFT_SCREEN_BG);
  if (pH < 10) {
    M5.Displays(0).drawString("pH", 285, 63);
  } else {
    M5.Displays(0).drawString("pH", 295, 63);
  }

  M5.Displays(0).setTextColor(TFT_SKYBLUE, TFT_SCREEN_BG);
  if (Temp < 20) {
    M5.Displays(0).drawString("o", 120, 185);
    M5.Displays(0).drawString("C", 128, 190);
  } else {
    M5.Displays(0).drawString("o", 140, 185);
    M5.Displays(0).drawString("C", 148, 190);
  }

  M5.Displays(0).setTextColor(TFT_ORANGE, TFT_SCREEN_BG);
  if (ORP < 100) {
    M5.Displays(0).drawString("mV", 292, 208);
  } else {
    M5.Displays(0).drawString("mV", 292, 218);
  }

  M5.Displays(0).fillRect(0, 100, 320, 2, TFT_SCREEN_BG);

  checkWaterQuality(EC, pH, Temp, ORP);

  delay(10000);
  myMQTT.checkConnect();
  M5.update();
}