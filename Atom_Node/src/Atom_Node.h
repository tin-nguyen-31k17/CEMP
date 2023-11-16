/*
    Project: CEMP Mobility Water Monitoring Station
    Author: Nguyen Trong Tin
    Email:tin.nguyen.31k17@hcmut.edu.vn
    Description: This code is part of the CEMP Mobility Water Monitoring Station project.
*/

#include <TinyGPSPlus.h>
#include <Arduino.h>
#include <M5Atom.h>
#include <WiFi.h>
#include <esp_now.h>
#include <CircularBuffer.h>

#define WIFI_CHANNEL 1
#define SENSOR_COUNT 4

#define GPS_RX_PIN 32
#define GPS_TX_PIN 26
#define DELAY_BETWEEN_READINGS 5000
#define BACKUP_BUFFER_SIZE 32768

static const uint32_t GPSBaud = 9600;

uint8_t backupBuffer[BACKUP_BUFFER_SIZE];
size_t backupIndex = 0;

TinyGPSPlus gps;

HardwareSerial ss(1);

uint8_t Gateway_Mac[] = {0x02, 0x10, 0x11, 0x12, 0x13, 0x14};
volatile boolean messageSent;
uint8_t receivedData[9];
uint8_t dataToSend[SENSOR_COUNT*3] = {0}; // Array to send sensor readings
float sensorReadings[SENSOR_COUNT] = {0};
uint8_t Lon, Lat, Day, Month, Year, Hour, Minute, Second;
String dateTime;
