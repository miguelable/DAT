#include "mqttClient.h"
#include "LedEffects.h"
#include "websockets.h"

#define DEVICE_ID "3"

WiFiClientSecure mqttClient;
PubSubClient     mqtt(mqttClient);

// Topics to subscribe
const char* portaventuraWorldTopic = "portaventura/status";
const char* ferrarilandTopic       = "ferrariland/status";
const char* caribeAcuaticTopic     = "caribeAcuatic/status";
const char* deviceStatusTopic      = "status/device/" DEVICE_ID;
const char* deviceActivationTopic  = "activation";

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

// Callback para manejar mensajes MQTT
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
    } else {
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