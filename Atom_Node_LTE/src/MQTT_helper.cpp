/*
  Project: CEMP Mobility Water Monitoring Station
  Author: Nguyen Trong Tin
  Email:tin.nguyen.31k17@hcmut.edu.vn
  Description: This code is part of the CEMP Mobility Water Monitoring Station project.
*/

#include "MQTT_helper.h"

void MyMQTT::connectToMQTT() {
    if (!WiFi.isConnected()) {
        Serial.println("WiFi not connected. Unable to connect to MQTT.");
        return;
    }

    client.setServer(mqtt_server.c_str(), 1883);
    client.setKeepAlive(60);

    reConnect();
    client.setCallback([this](char* topic, byte* payload, unsigned int length) {
        this->callback(topic, payload, length);
    });
}

void MyMQTT::reConnect() {
    while (!client.connected()) {
        Serial.print("Attempting MQTT connection...");

        IPAddress serverIP(103, 116, 104, 204);
        client.setServer(serverIP, 1883);

        String clientId = "DTU_SIM7680_Client-" + String(random(0xffff), HEX);
        if (client.connect(clientId.c_str(), user.c_str(), password.c_str())) {
            Serial.println("Connected to MQTT server.");
        } else {
            Serial.print("Failed to connect, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            delay(5000);
        }
    }
}

void MyMQTT::subscribe(String feedName) {
    String topic = feedName;
    client.subscribe(topic.c_str());
}

bool MyMQTT::isValidJson(String json) {
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, json);
    if (error) {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
        return false;
    }
    return true;
}

bool MyMQTT::publish(String feedName, String message) {
    if (!client.connected()) {
        Serial.println("Client not connected. Attempting to reconnect...");
        reConnect();
    }

    if (client.connected()) {
        String topic = feedName;
        String jsonMessage = "{\"data\":" + message + "}";
        Serial.print("Publishing to topic: ");
        Serial.println(topic);
        Serial.print("Payload size: ");
        Serial.println(jsonMessage.length());

        Serial.print("Status: ");
        if (client.publish(topic.c_str(), jsonMessage.c_str(), true)) {
            Serial.println("Success!");
            return true;
        } else {
            Serial.print("Failed with state: ");
            Serial.println(client.state());
            return false;
        }
    } else {
        Serial.println("Unable to reconnect to MQTT broker.");
        return false;
    }
}

void MyMQTT::checkConnect(){
    if (!client.connected()) {
        Serial.println("MQTT Connection lost!");
        reConnect();
    }
    client.loop();
}

void MyMQTT::callback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    for (int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
    }
    Serial.println();
}