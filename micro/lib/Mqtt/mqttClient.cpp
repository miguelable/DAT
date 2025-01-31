/**
 * @file mqttClient.cpp
 * @author Miguel Ferrer
 * @brief  Clase para gestionar la conexión MQTT.
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
#include "mqttClient.h"
#include "LedEffects.h"
#include "websockets.h"

#define DEVICE_ID "3" /*!< ID del dispositivo. */

WiFiClientSecure mqttClient;       /*!< Cliente seguro para MQTT. */
PubSubClient     mqtt(mqttClient); /*!< Cliente MQTT. */

// Topics to subscribe
const char* portaventuraWorldTopic = "portaventura/status";      /*!< Tema de PortAventura World. */
const char* ferrarilandTopic       = "ferrariland/status";       /*!< Tema de Ferrari Land. */
const char* caribeAcuaticTopic     = "caribeAcuatic/status";     /*!< Tema de Caribe Acuatic. */
const char* deviceStatusTopic      = "status/device/" DEVICE_ID; /*!< Tema de estado del dispositivo. */
const char* deviceActivationTopic  = "activation";               /*!< Tema de activación del dispositivo. */

/**
 * @brief Conecta al servidor MQTT y se suscribe a varios temas.
 *
 * Esta función intenta conectarse al servidor MQTT utilizando el ID de cliente "ESP32Client".
 * Si la conexión es exitosa, se suscribe a los siguientes temas:
 * - portaventuraWorldTopic
 * - ferrarilandTopic
 * - caribeAcuaticTopic
 * - deviceStatusTopic
 *
 * Para cada suscripción, registra si la suscripción fue exitosa o si hubo un error.
 * Si la conexión al servidor MQTT falla, registra el estado de error y reintenta después de un retraso de 5 segundos.
 */
void connectToMQTT()
{
  while (!mqtt.connected()) {
    if (mqtt.connect("ESP32Client")) {
      log_i("Conectado al servidor MQTT");
      if (mqtt.subscribe(portaventuraWorldTopic)) {
        log_i("Subscrito a PortAventura World");
      }
      else {
        log_e("Error al subscribirse a PortAventura World");
      }
      if (mqtt.subscribe(ferrarilandTopic)) {
        log_i("Subscrito a Ferrari Land");
      }
      else {
        log_e("Error al subscribirse a Ferrari Land");
      }
      if (mqtt.subscribe(caribeAcuaticTopic)) {
        log_i("Subscrito a Caribe Acuatic");
      }
      else {
        log_e("Error al subscribirse a Caribe Acuatic");
      }
      if (mqtt.subscribe(deviceStatusTopic)) {
        log_i("Subscrito a Device Status");
      }
      else {
        log_e("Error al subscribirse a Device Status");
      }
    }
    else {
      Serial.print("Error al conectar al servidor MQTT: ");
      Serial.println(mqtt.state());
      delay(5000); // Wait before retrying
    }
  }
}

/**
 * @brief Función de callback para manejar mensajes MQTT entrantes.
 *
 * Esta función se llama cada vez que se recibe un mensaje en un tema MQTT suscrito.
 * Procesa la carga útil del mensaje, la deserializa en un documento JSON y realiza
 * acciones basadas en el contenido del mensaje.
 *
 * @param topic El tema en el que se recibió el mensaje.
 * @param payload La carga útil del mensaje.
 * @param length La longitud de la carga útil.
 *
 * La función espera que la carga útil sea una cadena JSON con la siguiente estructura:
 * {
 *   "device": "<device_id>",
 *   "current_status": "<status>"
 * }
 *
 * El <device_id> debe coincidir con la constante DEVICE_ID. El <status> puede ser uno de los siguientes:
 * - "Apagado": Apaga el dispositivo y establece el efecto LED en Cerrado.
 * - "Encendido": Enciende el dispositivo y establece el efecto LED en Abierto.
 * - "Error": Indica un error, establece el efecto LED en Error y reactiva el dispositivo después de 4 segundos.
 * - "Exito": Indica éxito, establece el efecto LED en Éxito y reactiva el dispositivo después de 4 segundos.
 *
 * Si el mensaje es inválido o el ID del dispositivo no coincide, se registra un error.
 */
void mqttCallback(char* topic, byte* payload, unsigned int length)
{
  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println("Mensaje recibido: " + message);

  DynamicJsonDocument doc(200);
  deserializeJson(doc, message);
  isr_log_d("Mensaje recibido: %s", message.c_str());

  // Si el mensaje es {"device":"3","current_status":"Apagado"} entonces poner el led en closed
  if (doc["device"] == DEVICE_ID) {
    if (doc["current_status"] == "Apagado") {
      log_i("Atraccion cerrada");
      setLedEffect(Closed);
      deviceActivated = false;
    }
    else if (doc["current_status"] == "Encendido") {
      log_i("Atraccion abierta");
      setLedEffect(Opened);
      deviceActivated = true;
    }
    else if (doc["current_status"] == "Error") {
      log_i("Acceso no permitido a la atraccion");
      setLedEffect(Error);
      // esperar 4 segundos para activar el dispositivo otra vez
      vTaskDelay(4000);
      deviceActivated = true;
    }
    else if (doc["current_status"] == "Exito") {
      log_i("Acceso permitido a la atraccion");
      setLedEffect(Success);
      // esperar 4 segundos para activar el dispositivo otra vez
      vTaskDelay(4000);
      deviceActivated = true;
    }
    else {
      log_e("Mensaje no valido");
    }
  }
  else {
    log_e("Mensaje no valido");
  }
}

void setupMQTT()
{
  IPAddress serverIP = WiFi.localIP();
  serverIP[3]        = 1;

  // Configurar el cliente seguro
  mqttClient.setCACert(ca_cert);
  mqttClient.setInsecure();
  // Inicializar el cliente MQTT
  mqtt.setServer(serverIP, MQTTPORT);
  mqtt.setCallback(mqttCallback);
  connectToMQTT();

  // Publicar un mensaje de inicio
  if (mqtt.connected()) {
    // publicar en el topic de activación
    String message = "{\"device\":\"" + String(DEVICE_ID) + "\",\"status\":\"on\"}";
    mqtt.publish(deviceActivationTopic, message.c_str());
    log_i("Mensaje publicado: %s", message.c_str());
  }
  else {
    log_e("No conectado al servidor MQTT, no se puede publicar el mensaje");
  }
}