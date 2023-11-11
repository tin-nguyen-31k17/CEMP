/*
  Project: CEMP Mobility Water Monitoring Station
  File: WaterStation.cpp
  Author: Nguyen Trong Tin
  Email:tin.nguyen.31k17@hcmut.edu.vn
  Description: This code is part of the CEMP Mobility Water Monitoring Station project. It is used to monitor several sensors and publish the data to MQTT along with a nice UI to show sensor data and the battery level.
*/

#include "WaterStation.h"
#include "tools.h"
#include "images.h"
#include "font.h"
#include "functions.h"
#include "sensor_data.h"
#include "MQTT_helper.h"

// Class Wifi
WiFiMulti wifiMulti;

// Class MyMQTT
MyMQTT myMQTT("mqttserver.tk", "innovation", "Innovation_RgPQAZoA5N");

// Class data json already created
SENSOR_DATA sensorData;

void initVariant() {
  WiFi.mode(WIFI_AP);
  esp_wifi_set_mac(WIFI_IF_AP, &GatewayMac[0]);
}

void OnDataRecv(const uint8_t* mac_addr, const uint8_t* data, int data_len) {
  char macStr[24];
  snprintf(macStr, sizeof(macStr), " %02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print("\nData received from: ");
  Serial.println(macStr);

  // Copy the received data
  memcpy(receivedData, data, data_len);

  Serial.print("Decrypted Data: ");
  for (int i = 0; i < 7; i++) {
    Serial.print("0x");
    Serial.print(receivedData[i], HEX);
    Serial.print(", ");
  }
  Serial.println();
  
  // Process and publish the received data to MQTT
  for (int i = 0; i < SENSOR_COUNT; i++) {
    sensorReadings[i]=receivedData[i];
  }

  String data_to_pub;
  data_to_pub = sensorData.createWaterStationJSON(sensorReadings[0], sensorReadings[1], sensorReadings[2], sensorReadings[3], sensorReadings[4], sensorReadings[5]);
  myMQTT.publish("/innovation/watermonitoring/", data_to_pub);
}

// Setup
void setup() {
  // Init Serial
  Serial.begin(115200);

  // Init Preferences
  preferences.begin(NAME);
  brightness = preferences.getUInt("brightness", BRIGHTNESS);
  Serial.printf("Brightness = %d\n", brightness);

  auto cfg = M5.config();

  cfg.clear_display = true;   // default=true. clear the screen when begin.
  cfg.internal_imu  = true;   // default=true. use internal IMU.
  cfg.internal_rtc  = true;   // default=true. use internal RTC.
  cfg.internal_spk  = true;   // default=true. use internal speaker.
  cfg.internal_mic  = true;   // default=true. use internal microphone.
  cfg.external_imu  = false;  // default=false. use Unit Accel & Gyro.
  cfg.external_rtc  = false;  // default=false. use Unit RTC.

  cfg.external_display.module_display = true;   // default=true. use ModuleDisplay
  cfg.external_display.atom_display   = true;   // default=true. use AtomDisplay
  cfg.external_display.unit_oled      = false;  // default=true. use UnitOLED
  cfg.external_display.unit_lcd       = false;  // default=true. use UnitLCD
  cfg.external_display.unit_rca       = false;  // default=false. use UnitRCA VideoOutput
  cfg.external_display.module_rca     = false;  // default=false. use ModuleRCA VideoOutput

  M5.begin(cfg);

  // Init Leds
#if BOARD != CORES3
  if (M5.getBoard() == m5::board_t::board_M5Stack) {
    FastLED.addLeds<NEOPIXEL, 15>(leds,
                                  NUM_LEDS);  // GRB ordering is assumed
  } else if (M5.getBoard() == m5::board_t::board_M5StackCore2) {
    FastLED.addLeds<NEOPIXEL, 25>(leds,
                                  NUM_LEDS);  // GRB ordering is assumed
  }
  FastLED.setBrightness(16);
#endif

  // view UI
  viewUI();

  // view battery
  viewBattery();

  // Init Led
#if BOARD != CORES3
  initLed();
#endif

  // Init Physical
  initPhysical();

  // Init Variant
  initVariant();

  // Connect to WiFi
  Serial.print("Connecting to WiFi...");
  M5.Displays(0).drawString("Connecting to " + String(SOFTAP_SSID) + "...", 160, 100);
  delay(2000);
  M5.Displays(0).drawString("", 160, 100);
  delay(250);

  // Set device in AP mode to begin with
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

  // Print WiFi data
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

  myMQTT.connectToMQTT(); // Connect to MQTT server
  myMQTT.subscribe("/innovation/watermonitoring/");

  // Initialize ESP-Now
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

  // ESPNow is now initialized. Register a callback function for when data is received
  esp_now_register_recv_cb(OnDataRecv);

  // Multitasking task for retreive button
  xTaskCreatePinnedToCore(button,    // Function to implement the task
                          "button",  // Name of the task
                          8192,      // Stack size in words
                          NULL,      // Task input parameter
                          4,         // Priority of the task
                          NULL,      // Task handle
                          1);        // Core where the task should run

#if BOARD != CORES3
  xTaskCreatePinnedToCore(led,    // Function to implement the task
                          "led",  // Name of the task
                          1024,   // Stack size in words
                          NULL,   // Task input parameter
                          4,      // Priority of the task
                          NULL,   // Task handle
                          1);     // Core where the task should run
#endif
}

// Main loop
void loop() {
  // refresh UI each 5 seconds
  viewUI();
  viewBattery();

  float EC, pH, Temp, ORP, Lon, Lat;
  EC = receivedData[0];
  pH = receivedData[1];
  Temp = receivedData[3];
  ORP = receivedData[2];
  Lon = receivedData[4];
  Lat = receivedData[5];

  if (Lon == 0 && Lat == 0) {
    Lon = 106.99;
    Lat = 10.2;
  }

  // Display Long and Lat at the center of 240x320 screen
  M5.Displays(0).setFont(&arial6pt7b);
  M5.Displays(0).setTextColor(TFT_WHITE, TFT_SCREEN_BG);
  M5.Displays(0).setTextDatum(CC_DATUM);
  M5.Displays(0).setTextPadding(20);
  String longLat = "Long: " + String(float(Lon)) + " Lat: " + String(float(Lat));
  M5.Displays(0).drawString(longLat, 160, 94);

  // Render result to screen
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

  // Wait for next measurement
  delay(10000);
  myMQTT.checkConnect();
  M5.update();
}