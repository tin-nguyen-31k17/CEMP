/*
    Project: CEMP Mobility Water Monitoring Station
    Author: Nguyen Trong Tin
    Email:tin.nguyen.31k17@hcmut.edu.vn
    Description: This code is part of the CEMP Mobility Water Monitoring Station project.
*/

#ifndef INC_MQTT_HELPER_H_
#define INC_MQTT_HELPER_H_

#include "Atom_DTU_CAT1.h"
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <TinyGsmClient.h>

class MyMQTT {
private:
    String mqtt_server;
    String user;
    String password;
    WiFiClient espClient;
    TinyGsmClient& dtuLTEClient;
    TinyGsm& dtuLTE;
    PubSubClient client;

public:
    MyMQTT(String server, String user, String password, TinyGsmClient& dtuClient, TinyGsm& dtu)
        : mqtt_server(server), user(user), password(password), dtuLTEClient(dtuClient), dtuLTE(dtu), client(dtuLTEClient) {
        client.setBufferSize(2048);
    }

    void connectToMQTT();
    void subscribe(String);
    bool publish(String, String);
    void checkConnect();
    // void configureNetworkInterface();
    bool isValidJson(String);

private:
    void callback(char*, byte*, unsigned int);
    void reConnect();
};

#endif // INC_MQTT_HELPER_H_
