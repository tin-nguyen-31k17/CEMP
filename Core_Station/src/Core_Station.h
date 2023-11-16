/*
  Project: CEMP Mobility Water Monitoring Station
  File: WaterStation.h
  Author: Nguyen Trong Tin
  Email:tin.nguyen.31k17@hcmut.edu.vn
  Description: This code is part of the CEMP Mobility Water Monitoring Station project. It is the header file of the WaterStation.cpp file.
*/

// Version
#define VERSION "1.1.3"
#define AUTHOR  "Tin Nguyen"
#define NAME    "WaterStation"

#define RANDOM 1  // Set to 1 for random view
#define CORE   1
#define CORE2  2
#define CORES3 3

#define SOFTAP_SSID "A-Automator"
#define SOFTAP_PASS "Cmbuilderx@X"
// #define SOFTAP_SSID "HPCCLAB"
// #define SOFTAP_PASS "hpccw1f1"
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

// Variables
uint16_t brightness      = BRIGHTNESS;
uint16_t batteryLevelOld = 1024;
boolean batteryCharginglOld = true;
float temperatureOffset;

uint8_t GatewayMac[] = {0x02, 0x10, 0x11, 0x12, 0x13, 0x14};
uint8_t receivedData[SENSOR_COUNT] = {0};
float sensorReadings[SENSOR_COUNT] = {0};
uint8_t Lon, Lat, Day, Month, Year, Hour, Minute, Second;
String dateTimeGPS;

CRGB m5goColor = CRGB::Blue;