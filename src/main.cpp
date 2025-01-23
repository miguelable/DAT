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

// Includes para el primer programa
#include "LedEffects.h"
#include "hcsr04.h"
#include "mqttClient.h"
#include "websockets.h"
#include <Arduino.h>
#include <EEPROM.h>

// Includes para el segundo programa
#include "CRtspSession.h"
#include "OV2640Streamer.h"
#include "SimStreamer.h"
#include "wifikeys.h"
OV2640        cam;
WiFiServer    rtspServer(8554);
CStreamer*    streamer;
CRtspSession* session;
WiFiClient    camClient;

#define BUTTON_PIN 12 /*!< Pin number connected to the button. */
#define EEPROM_ADDRESS 0 // Dirección en la EEPROM donde se guardará el valor

unsigned long buttonPressStartTime = 0;
bool          buttonPressed        = false;
int           program_selector     = 0; // 0 = main program, 1 = RTSP program

HCSR04 sensor;

void activationCallback()
{
  if (!sendImageToServer()) {
    log_e("Error al enviar la imagen");
    setLedEffect(Warning);
    // esperar 10 segundos antes de activar el dispositivo otra vez
    vTaskDelay(10000);
  }
}

void readProgramSelector()
{
  EEPROM.begin(512);
  program_selector = EEPROM.read(EEPROM_ADDRESS);
  log_i("Program selector: %d", program_selector);
  if (program_selector != 0 && program_selector != 1) {
    log_e("Invalid program selector, using default value");
    program_selector = 0;
  }
}

void rtspTask(void* pvParameters)
{
  uint32_t        msecPerFrame = 100;
  static uint32_t lastimage    = millis();

  while (true) {
    if (session) {
      session->handleRequests(0); // we don't use a timeout here,
      // instead we send only if we have new enough frames

      uint32_t now = millis();
      if (now > lastimage + msecPerFrame || now < lastimage) { // handle clock rollover
        session->broadcastCurrentFrame(now);
        lastimage = now;

        // check if we are overrunning our max frame rate
        now = millis();
        if (now > lastimage + msecPerFrame)
          log_w("warning exceeding max frame rate of %d ms\n", now - lastimage);
      }

      if (session->m_stopped) {
        delete session;
        delete streamer;
        session  = NULL;
        streamer = NULL;
        // turn off the LED to show we are not streaming
        digitalWrite(FLASH_PIN, LOW);
      }
    }
    else {
      camClient = rtspServer.accept();

      if (camClient) {
        // streamer = new SimStreamer(&camClient, true);             // our streamer for UDP/TCP based RTP transport
        streamer = new OV2640Streamer(&camClient, cam);    // our streamer for UDP/TCP based RTP transport
        session  = new CRtspSession(&camClient, streamer); // our threads RTSP session and state
        // turn on the LED to show we are streaming
        digitalWrite(FLASH_PIN, HIGH);
      }
    }
    vTaskDelay(1);
  }
}

void setup()
{
  Serial.begin(115200);
  // Configurar pin de flash y botón
  pinMode(FLASH_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);

  // Leer el programa seleccionado
  readProgramSelector();

  if (program_selector == 0) {
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
  else if (program_selector == 1) {
    // init flash LED (active low)
    cam.init(esp32cam_aithinker_config);

    IPAddress ip;

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid2, password2);
    while (WiFi.status() != WL_CONNECTED) {
      delay(100);
    }
    ip = WiFi.localIP();
    Serial.println(F("WiFi connected"));
    Serial.println("");
    Serial.println(ip);
    rtspServer.begin();

    // Crear la tarea RTSP
    xTaskCreate(rtspTask, "RTSP Task", 8192, NULL, 1, NULL);
  }
  else {
    log_e("Invalid program selector");
  }
}

void loop()
{
  // Procesar mensajes MQTT entrantes
  mqtt.loop();

  if (digitalRead(BUTTON_PIN) == HIGH) {
    if (!buttonPressed) {
      buttonPressed        = true;
      buttonPressStartTime = millis();
    }
    else if (millis() - buttonPressStartTime > 3000) {
      log_i("Button pressed for more than 3 seconds, changing program...");
      EEPROM.write(EEPROM_ADDRESS, 1 - program_selector); // Cambiar el programa
      EEPROM.commit();                                    // Asegúrate de que los cambios se escriban en la EEPROM
      ESP.restart();
    }
  }
  else {
    if (buttonPressed && (millis() - buttonPressStartTime <= 3000)) {
      log_i("Button pressed for less than 3 seconds, restarting...");
      ESP.restart();
    }
    buttonPressed = false;
  }
  delay(1);
}
