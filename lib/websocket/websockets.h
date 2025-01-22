/**
 * @file websockets.h
 * @author Miguel Ferrer (mferrer@inbiot.es)
 * @brief  WebSockets client library.
 * @version 0.1
 * @date 2025-01-20
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef WEBSOCKETS_H
#define WEBSOCKETS_H

#include "esp32cam.h"
#include <NTPClient.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <WiFiUdp.h>
#include <freertos/FreeRTOS.h>

#define SERVER_PORT 54472 /*!< Server port. */
#define FLASH_PIN 4       /*!< Pin number connected to the LED flash. */

// Device status
extern bool deviceActivated; /*!< Device activation status. */

extern TaskHandle_t sendDataTask; /*!< Task to send data to the server. */

extern NTPClient timeClient; /*!< NTP client. */

void connectToWiFi();
void connectToServer();
void startCamera();
bool sendImageToServer();

#endif // WEBSOCKETS_H