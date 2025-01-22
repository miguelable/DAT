/**
 * @file main.cpp
 * @author Miguel Ferrer (mferrer@inbiot.es)
 * @brief Main file for the TecnoPark project.
 * @version 0.1
 * @date 2025-01-14
 *
 * This file contains the main setup and loop functions for the TecnoPark project.
 * It initializes the HCSR04 ultrasonic sensor, sets up the LED effects, and handles
 * the button press to cycle through different LED effects.
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "LedEffects.h"
#include "hcsr04.h"
#include "mqttClient.h"
#include "websockets.h"
#include <Arduino.h>

#define BUTTON_PIN 12 /*!< Pin number connected to the button. */

HCSR04 sensor;

bool responseReceived = false;

void activationCallback()
{
  if (!sendImageToServer()) {
    log_e("Error al enviar la imagen");
    setLedEffect(Warning);
    // esperar 10 segundos antes de activar el dispositivo otra vez
    vTaskDelay(10000);
  }
}

void setup()
{
  Serial.begin(115200);
  // Configurar pin de flash y botón
  pinMode(FLASH_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);
  setupLed();
  setLedEffect(Waiting);
  // Conectar a la red WiFi y al servidor
  connectToWiFi();
  connectToServer();
  // Inicializar el cliente MQTT
  setupMQTT();
  // Inicializar el servidor WebSockets
  startCamera();
  // Inicializar el sensor
  sensor.setCallback(15, activationCallback);
  sensor.init();
}

void loop()
{
  // Procesar mensajes MQTT entrantes
  mqtt.loop();

  if (digitalRead(BUTTON_PIN) == HIGH) {
    // effectIndex++;
    log_i("Button pressed");
    // reset the device
    ESP.restart();
  }
  delay(1);
}
