#ifndef MQTTCLIENT_H
#define MQTTCLIENT_H

#include "secrets.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

#define MQTTPORT 8883

extern WiFiClientSecure mqttClient;
extern PubSubClient     mqtt;

// Function to connect to the MQTT broker
void setupMQTT();

#endif // MQTTCLIENT_H