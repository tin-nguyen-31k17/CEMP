#include <TinyGPSPlus.h>
#include <Arduino.h>
#include <M5Atom.h>
#include <WiFi.h>
#include <esp_now.h>
#include <CircularBuffer.h>
#include <time.h>
#include <sys/time.h>
#include <Wire.h>

#define WIFI_CHANNEL 1
#define SENSOR_COUNT 4

#define RS485_RX 22
#define RS485_TX 19 

HardwareSerial RS485(2);

#define BACKUP_UART_RX 32
#define BACKUP_UART_TX 26
HardwareSerial BackupSerial(2);

#define GPS_RX_PIN 32
#define GPS_TX_PIN 26
HardwareSerial ss(1);

#define RELAY_PIN 26

#define DELAY_BETWEEN_READINGS 5000
#define BACKUP_BUFFER_SIZE 32768

static const uint32_t GPSBaud = 9600;

uint8_t backupBuffer[BACKUP_BUFFER_SIZE];
size_t backupIndex = 0;

TinyGPSPlus gps;

uint8_t Gateway_Mac[] = {0x02, 0x10, 0x11, 0x12, 0x13, 0x14};
uint8_t Atom_Mac[] = {0x02, 0x10, 0x11, 0x12, 0x13, 0x15};
volatile boolean messageSent;
volatile bool espNowSendSuccess = false;
uint8_t receivedData[9];
uint8_t dataToSend[13] = {0};
float sensorReadings[SENSOR_COUNT] = {0};
uint8_t Lon, Lat, Day, Month, Year, Hour, Minute, Second;
String dateTime;
uint8_t relayStatus = 1;

SENSOR_DATA sensorData;


int failedSendCount = 0;
int MAX_FAILED_SENDS =3;