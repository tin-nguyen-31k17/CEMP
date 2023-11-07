#include "WaterStation.h"
#include "tools.h"
#include "images.h"
#include "font.h"
#include "functions.h"
#include "sensor_data.h"
#include "MQTT_helper.h"

#define SOFTAP_SSID "A-Automator"
#define SOFTAP_PASS "Cmbuilderx@X"
#define SENSOR_COUNT 4

uint8_t GatewayMac[] = {0x02, 0x10, 0x11, 0x12, 0x13, 0x14};
uint8_t receivedData[7];

// Class Wifi
WiFiMulti wifiMulti;

// Class MyMQTT
MyMQTT myMQTT("mqttserver.tk", "innovation", "Innovation_RgPQAZoA5N");

// Class data json already created
SENSOR_DATA sensorData;

float sensorReadings[SENSOR_COUNT] = {0};

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

  for (int i = 0; i < data_len; i++) {
    receivedData[i] = data[i];
  }

  Serial.print("Data received: ");
  for (int i = 0; i < 7; i++) {
    Serial.print("0x");
    Serial.print(receivedData[i], HEX);
    Serial.print(", ");
  }
  Serial.println();

  // int index = 0;
  for (int i = 0; i < SENSOR_COUNT; i++) {
    // int highByte = receivedData[index++];
    // int lowByte = receivedData[index++];
    // sensorReadings[i] = (highByte * 256) + lowByte;
    sensorReadings[i]=receivedData[i];
  }

  // Process and publish the received data to MQTT
  String data_to_pub;
  data_to_pub = sensorData.createWaterStationJSON(sensorReadings[0], sensorReadings[1], sensorReadings[2], sensorReadings[3]);
  myMQTT.publish("/innovation/watermonitoring/", data_to_pub);
}

// Setup
void setup() {
  // Init M5
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

  // Init Preferences
  preferences.begin(NAME);
  brightness = preferences.getUInt("brightness", BRIGHTNESS);
  Serial.printf("Brightness = %d\n", brightness);

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

  // Get temperature offset
  // temperatureOffset = getTemperatureOffset();

  // Init Sensor
  initSensor();

  initVariant();

  // Connect to WiFi
  Serial.print("Connecting to WiFi...");

  // Set device in AP mode to begin with
  WiFi.mode(WIFI_AP_STA);

  wifiMulti.addAP(SOFTAP_SSID, SOFTAP_PASS);
  while (wifiMulti.run() != WL_CONNECTED) {
    delay(500);
    Serial.print("Can't connect to network \n");
  }

  // Print WiFi data
  Serial.println("Connected!");
  Serial.println("Set as AP_STA station.");
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  Serial.print("Channel: ");
  Serial.println(WiFi.channel());
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  delay(1000);

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
  } else {
    Serial.println("ESPNow Init Failed!");
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
  float EC, temperature, ORP;
  uint8_t data[12], counter;

  // view battery
  viewBattery();

  // Send read data command
  Wire.beginTransmission(SCD_ADDRESS);
  Wire.write(0xec);
  Wire.write(0x05);
  Wire.endTransmission();

  Wire.requestFrom(SCD_ADDRESS, 12);
  counter = 0;
  while (Wire.available()) {
    data[counter++] = Wire.read();
  }

  // Floating point conversion according to datasheet
  // co2 = (float)((uint16_t)data[0] << 8 | data[1]);
  // // Convert T in deg C
  // temperature = -45 + 175 * (float)((uint16_t)data[3] << 8 | data[4]) / 65535 - temperatureOffset;
  // // Convert RH in %
  // humidity = 100 * (float)((uint16_t)data[6] << 8 | data[7]) / 65535;

  EC = receivedData[0];
  temperature = receivedData[3];
  ORP = receivedData[2];

  // Serial.printf("co2 %02f, temperature %02f, temperature offset %02f, humidity %02f\n", co2, temperature,
  //               temperatureOffset, humidity);

  if (temperature > -10) {
    // View result
    M5.Displays(0).setFont(&digital_7__mono_24pt7b);
    M5.Displays(0).setTextDatum(CL_DATUM);

    M5.Displays(0).setTextPadding(40);
    M5.Displays(0).setTextColor(TFT_PINK, TFT_SCREEN_BG);
    M5.Displays(0).drawString(String(int(EC)), 90, 50);
    M5.Displays(0).setTextColor(TFT_WHITE, TFT_SCREEN_BG);
    M5.Displays(0).drawString(String(int(EC)), 250, 50);

    M5.Displays(0).setTextPadding(40);
    M5.Displays(0).setTextColor(TFT_SKYBLUE, TFT_SCREEN_BG);
    M5.Displays(0).drawString(String(int(temperature)), 90, 195);
    M5.Displays(0).setTextColor(TFT_ORANGE, TFT_SCREEN_BG);
    M5.Displays(0).drawString(String(int(ORP)), 250, 195);

    M5.Displays(0).setFont(&arial6pt7b);
    M5.Displays(0).setTextColor(TFT_WHITE, TFT_SCREEN_BG);
    M5.Displays(0).setTextDatum(CL_DATUM);
    M5.Displays(0).setTextPadding(20);

    M5.Displays(0).setTextColor(TFT_PINK, TFT_SCREEN_BG);
    if (EC < 1000) {
      M5.Displays(0).drawString("mS/cm", 125, 50);
    } else {
      M5.Displays(0).drawString("mS/cm", 145, 50);
    }

    M5.Displays(0).setTextColor(TFT_WHITE, TFT_SCREEN_BG);
    M5.Displays(0).drawString("pH", 280, 50);

    M5.Displays(0).setTextColor(TFT_SKYBLUE, TFT_SCREEN_BG);
    M5.Displays(0).drawString("o", 120, 190);
    M5.Displays(0).drawString("C", 128, 195);

    M5.Displays(0).setTextColor(TFT_ORANGE, TFT_SCREEN_BG);
    M5.Displays(0).drawString("mV", 280, 195);

    M5.Displays(0).fillRect(0, 100, 320, 2, TFT_SCREEN_BG);

    if (EC < 1000) {
      M5.Displays(0).fillRect(16 + 64 * 0 + 8 * 0, 100, 64, 2, TFT_WHITE);
      m5goColor = CRGB::Green;
    } else if (EC < 2000) {
      M5.Displays(0).fillRect(16 + 64 * 1 + 8 * 1, 100, 64, 2, TFT_WHITE);
      m5goColor = CRGB::Yellow;
    } else if (EC < 3000) {
      M5.Displays(0).fillRect(16 + 64 * 2 + 8 * 2, 100, 64, 2, TFT_WHITE);
      m5goColor = CRGB::Orange;
    } else {
      M5.Displays(0).fillRect(16 + 64 * 3 + 8 * 3, 100, 64, 2, TFT_WHITE);
      m5goColor = CRGB::Blue;
    }
  }

  // Wait for next measurement
  delay(5000);
  myMQTT.checkConnect();
  M5.update();
}
