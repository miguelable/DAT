/**
 * @file main.cpp
 * @author Miguel Ferrer (mferrer@inbiot.es)
 * @brief Archivo principal para el proyecto TecnoPark.
 * @version 0.1
 * @date 2025-01-14
 *
 * Este archivo contiene las funciones principales de configuración y bucle para el proyecto TecnoPark.
 * Inicializa el sensor ultrasónico HCSR04, configura los efectos LED y maneja la pulsación del botón
 * para alternar entre diferentes efectos LED.
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "LedEffects.h"
#include "hcsr04.h"
#include "mqttClient.h"
#include "websockets.h"
#include <Arduino.h>

#define BUTTON_PIN 12 /*!< Número de pin conectado al botón. */

HCSR04 sensor; /*!< Objeto del sensor HCSR04. */

/**
 * @brief Función de callback que se activa al activarse el sensor.
 *
 * Esta función intenta enviar una imagen al servidor. Si la imagen
 * no se puede enviar, registra un mensaje de error, establece el efecto LED
 * a un estado de advertencia y espera 10 segundos antes de permitir que el dispositivo
 * se active nuevamente.
 *
 * @note La función usa vTaskDelay para implementar la demora,
 * se utiliza dentro de una tarea de FreeRTOS.
 */
void activationCallback()
{
  if (!sendImageToServer()) {
    log_e("Error al enviar la imagen");
    setLedEffect(Warning);
    // esperar 10 segundos antes de activar el dispositivo otra vez
    vTaskDelay(10000);
  }
}

/**
 * @brief Función de configuración para inicializar los componentes de hardware y red.
 *
 * Esta función realiza las siguientes tareas:
 * - Inicializa la comunicación serial a 115200 baudios.
 * - Configura el pin del flash como salida y el pin del botón como entrada.
 * - Configura el LED y aplica el efecto 'Waiting'.
 * - Conecta a la red WiFi y al servidor.
 * - Inicializa el cliente MQTT.
 * - Inicia la cámara.
 * - Establece un callback para el sensor y lo inicializa.
 */
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

/**
 * @brief Función principal de bucle que procesa los mensajes MQTT entrantes y verifica el estado del botón.
 *
 * Esta función se llama repetidamente en un bucle infinito. Realiza las siguientes tareas:
 * - Procesa los mensajes MQTT entrantes llamando a la función mqtt.loop().
 * - Verifica el estado de un botón conectado a BUTTON_PIN. Si el botón está presionado (digitalRead(BUTTON_PIN)
 * devuelve HIGH), registra un mensaje indicando la pulsación del botón y luego reinicia el dispositivo usando
 * ESP.restart().
 * - Introduce una breve demora de 1 milisegundo para evitar que el bucle se ejecute demasiado rápido.
 */
void loop()
{
  // Procesar mensajes MQTT entrantes
  mqtt.loop();

  if (digitalRead(BUTTON_PIN) == HIGH) {
    log_i("Boton presionado");
    // reiniciar el dispositivo
    ESP.restart();
  }
  delay(1);
}
