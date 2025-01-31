/**
 * @file mqttClient.h
 * @author Miguel Ferrer
 * @brief Clase para gestionar la conexión MQTT.
 * @version 0.1
 * @date 2025-01-23
 *
 * Este archivo contiene la implementación de la clase MqttClient, que proporciona funciones para conectarse a un broker
 * MQTT. La clase utiliza las bibliotecas PubSubClient y WiFiClientSecure para establecer una conexión segura con el
 * broker MQTT. La clase también define las constantes de configuración del broker MQTT.
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef MQTTCLIENT_H
#define MQTTCLIENT_H

#include "secrets.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

#define MQTTPORT 8883 /*!< Puerto del servidor MQTT. */

extern WiFiClientSecure mqttClient; /*!< Cliente seguro para MQTT. */
extern PubSubClient     mqtt;       /*!< Cliente MQTT. */

/**
 * @brief Configura el cliente MQTT y se conecta al servidor MQTT.
 *
 * Esta función realiza los siguientes pasos:
 * 1. Recupera la dirección IP local y establece el último octeto en 1 para formar la IP del servidor.
 * 2. Configura el cliente MQTT con el certificado CA y lo establece en modo inseguro.
 * 3. Inicializa el cliente MQTT con la IP del servidor y el puerto.
 * 4. Establece la función de callback para manejar los mensajes MQTT entrantes.
 * 5. Se conecta al servidor MQTT.
 * 6. Publica un mensaje de inicio en el tema de activación del dispositivo si la conexión es exitosa.
 *
 * Si la conexión al servidor MQTT falla, se registra un mensaje de error.
 */
void setupMQTT();

#endif // MQTTCLIENT_H