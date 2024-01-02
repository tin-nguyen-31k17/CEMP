/*
    Project: CEMP Mobility Water Monitoring Station
    Author: Nguyen Trong Tin
    Email:tin.nguyen.31k17@hcmut.edu.vn
    Description: This code is part of the CEMP Mobility Water Monitoring Station project.
*/

#include <Arduino.h>
#include <M5Atom.h>
#include <CircularBuffer.h>
#include <TinyGsmClient.h>
#include <time.h>
#include <sys/time.h>
#include <Wire.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>

uint8_t Atom_Mac[] = {0x02, 0x10, 0x11, 0x12, 0x13, 0x15};

#define UART_RX_PIN 32
#define UART_TX_PIN 26
#define UART_BAUDRATE 115200

#define SENSOR_COUNT 4

#define DELAY_BETWEEN_READINGS 5000
#define BACKUP_BUFFER_SIZE 32768

uint8_t backupBuffer[BACKUP_BUFFER_SIZE];

uint8_t receivedData[13];
float EC, pH, Temp, ORP, Lon, Lat, Day, Month, Year, Hour, Minute, Second;
String dateTime;
uint8_t relayStatus = 0;

String myTopic = "/innovation/watermonitoring";
SENSOR_DATA sensorData;

TinyGsm modem(SerialAT);

TinyGsmClient tcpClient(modem);

const char apn[]      = "m-wap";
const char gprsUser[] = "mms";
const char gprsPass[] = "";

int failedSendCount = 0;
int MAX_FAILED_SENDS =3;

bool isLTEInitialized = false;
MyMQTT myMQTT("mqttserver.tk", "innovation", "Innovation_RgPQAZoA5N", tcpClient, modem);