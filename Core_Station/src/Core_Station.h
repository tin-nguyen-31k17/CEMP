/*
  Project: CEMP Mobility Water Monitoring Station
  File: WaterStation.h
  Author: Nguyen Trong Tin
  Email:tin.nguyen.31k17@hcmut.edu.vn
  Description: This code is part of the CEMP Mobility Water Monitoring Station project. It is the header file of the WaterStation.cpp file.
*/

#define VERSION "1.1.3"
#define AUTHOR  "Tin Nguyen"
#define NAME    "WaterStation"

#define RANDOM 1  // Set to 1 for random view
#define CORE   1
#define CORE2  2
#define CORES3 3

// #define SOFTAP_SSID "A-Automator"
// #define SOFTAP_PASS "Cmbuilderx@X"
// #define SOFTAP_SSID "HPCCLAB"
// #define SOFTAP_PASS "hpccw1f1"
// #define SOFTAP_SSID "Trustin iPhone"
// #define SOFTAP_PASS "tin1752545"
// #define SOFTAP_SSID "EMC23-Automation"
// #define SOFTAP_PASS "123456789x@X"
#define SOFTAP_SSID "cce_hocvien"
#define SOFTAP_PASS "0902449198"
#define SENSOR_COUNT 12

#define TFT_SCREEN_BG M5.Displays(0).color565(32, 32, 32)

// Dependencies
#include <Preferences.h>
#include <LittleFS.h>
#include <SD.h>
#include <Wire.h>
#include <FastLED.h>
#include <M5ModuleDisplay.h>
#include <M5Unified.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <AES.h>

// Preferences
Preferences preferences;

// LED
#define FASTLED_INTERNAL  // To disable pragma messages on compile
#define NUM_LEDS 10
CRGB leds[NUM_LEDS];
CRGB m5goColor = CRGB::Blue;

// SD Card
#define SD_CS 4
#define SPI_MODE_LCD { pinMode(TFT_DC, OUTPUT); digitalWrite(TFT_DC, HIGH); }
#define SPI_MODE_SDCARD { pinMode(TFT_DC, INPUT); }

// Variables
uint16_t brightness      = BRIGHTNESS;
uint16_t batteryLevelOld = 1024;
boolean batteryCharginglOld = true;
float temperatureOffset;
uint8_t Lon, Lat, Day, Month, Year, Hour, Minute, Second;
String dateTimeGPS;
SENSOR_DATA sensorData;
bool messageReceived = false;

// ESP-NOW
uint8_t GatewayMac[] = {0x02, 0x10, 0x11, 0x12, 0x13, 0x14};
uint8_t receivedData[SENSOR_COUNT] = {0};
float sensorReadings[SENSOR_COUNT] = {0};

// WiFi
WiFiMulti wifiMulti;

// MQTT Server
MyMQTT myMQTT("mqttserver.tk", "innovation", "Innovation_RgPQAZoA5N");
String myTopic = "/innovation/watermonitoring";

