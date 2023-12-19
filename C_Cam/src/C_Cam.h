/*
    Project: CEMP Mobility Water Monitoring Station
    Author: Nguyen Trong Tin
    Email:tin.nguyen.31k17@hcmut.edu.vn
    Description: This code is part of the CEMP Mobility Water Monitoring Station project.
*/

#include <M5StickCPlus.h>
#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <CircularBuffer.h>
#include <time.h>
#include <sys/time.h>
#include <Base64.h>

#define WIFI_CHANNEL 1
#define SENSOR_COUNT 4

#define DELAY_BETWEEN_READINGS 5000
#define BACKUP_BUFFER_SIZE 32768

static const uint32_t GPSBaud = 9600;

uint8_t backupBuffer[BACKUP_BUFFER_SIZE];
size_t backupIndex = 0;

uint8_t Gateway_Mac[] = {0x02, 0x10, 0x11, 0x12, 0x13, 0x14};
volatile boolean messageSent;
uint8_t receivedData[9];
uint8_t Lon, Lat, Day, Month, Year, Hour, Minute, Second;
String dateTime;

const char* ssid = "EMC23-Automation";
const char* passwd = "123456789x@X";
// const char* ssid = "Trustin iPhone";
// const char* passwd = "tin1752545";

MyMQTT myMQTT("mqttserver.tk", "innovation", "Innovation_RgPQAZoA5N");
String myTopic = "/innovation/watermonitoring";

int failedSendCount = 0;
int MAX_FAILED_SENDS =3;