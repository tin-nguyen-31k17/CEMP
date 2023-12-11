/*
    Project: CEMP Mobility Water Monitoring Station
    Author: Nguyen Trong Tin
    Email:tin.nguyen.31k17@hcmut.edu.vn
    Description: This code is part of the CEMP Mobility Water Monitoring Station project.
*/

#include <Arduino.h>

#define TINY_GSM_MODEM_SIM7600

#define SerialMon        Serial
#define MONITOR_BAUDRATE 115200

#define SerialAT         Serial1
#define SIM7680_BAUDRATE 115200

#define ATOM_DTU_SIM7680_RESET -1
#define ATOM_DTU_SIM7680_EN    12
#define ATOM_DTU_SIM7680_TX    22
#define ATOM_DTU_SIM7680_RX    19

#define ATOM_DTU_RS485_TX 23
#define ATOM_DTU_RS485_RX 33