/*
  Project: CEMP Mobility Water Monitoring Station
  File: WaterStation.h
  Author: Nguyen Trong Tin
  Email:tin.nguyen.31k17@hcmut.edu.vn
  Description: This code is part of the CEMP Mobility Water Monitoring Station project. It is the header file of the WaterStation.cpp file.
*/

// Version
#define VERSION "1.0.1"
#define AUTHOR  "Tin Nguyen"
#define NAME    "WaterStation"

#define RANDOM 1  // Set to 1 for random view
#define CORE   1
#define CORE2  2
#define CORES3 3

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

// Preferences
Preferences preferences;

// LED
#define FASTLED_INTERNAL  // To disable pragma messages on compile
#define NUM_LEDS 10
CRGB leds[NUM_LEDS];

// Sensor SCD4x
const int16_t SCD_ADDRESS = 0x62;

// Variables
uint16_t brightness      = BRIGHTNESS;
uint16_t batteryLevelOld = 1024;

boolean batteryCharginglOld = true;

float temperatureOffset;

CRGB m5goColor = CRGB::Blue;