/**
 * @file websockets.cpp
 * @author Miguel Ferrer
 * @brief   Librería para WebSocket y cámara ESP32.
 * @version 0.1
 * @date 2025-01-20
 *
 * Este archivo contiene la implementación de la librería para WebSocket y cámara ESP32.
 * La librería proporciona funciones para conectarse a una red WiFi, establecer una conexión
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "websockets.h"
#include "LedEffects.h"
#include "secrets.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <base64.h>

bool deviceActivated = false; /*!< Estado de activación del dispositivo. */

TaskHandle_t sendDataTask         = NULL; /*!< Manejador de la tarea de envío de datos. */
TaskHandle_t updateTimeTaskHandle = NULL; /*!< Manejador de la tarea de actualización de la hora. */

// Objeto UDP para NTP
WiFiUDP   ntpUDP;                                       /*!< Objeto UDP para NTP. */
NTPClient timeClient(ntpUDP, ntpServer, gmtOffset_sec); /*!<  Cliente NTP para obtener la hora actual. */

// Cliente seguro para WebSockets
WiFiClientSecure client; /*!< Cliente seguro para WebSockets. */

IPAddress serverIP; /*!< Dirección IP del servidor. */

ESP32Cam camera; /*!< Objeto de la cámara ESP32. */

/**
 * @brief Tarea para actualizar continuamente la hora utilizando el cliente de tiempo.
 *
 * Esta función ejecuta un bucle infinito donde actualiza el cliente de tiempo
 * y luego espera 1000 milisegundos (1 segundo) antes de repetir.
 *
 * @param parameter Un puntero a los parámetros pasados a la tarea (no utilizado en esta función).
 */
void updateTimeTask(void* parameter)
{
  while (true) {
    timeClient.update();
    // Serial.printf("Hora actual: %s\n", timeClient.getFormattedTime().c_str());
    vTaskDelay(1000);
  }
}

void connectToWiFi()
{
  // Conexión a la red WiFi
  log_v("Conectando a la red WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    vTaskDelay(500);
  }
  // Imprimir la dirección IP
  log_d("IP Dispositivo: %s\n", WiFi.localIP().toString().c_str());

  serverIP    = WiFi.localIP();
  serverIP[3] = 1;
  log_i("IP Servidor: %s\n", serverIP.toString().c_str());

  // Inicializar el objeto NTPClient
  timeClient.begin();
  // crear la tarea para actualizar la hora cada segundo
  if (xTaskCreate(updateTimeTask, "updateTimeTask", 2048, NULL, 1, &updateTimeTaskHandle) != pdPASS) {
    log_e("Tarea de actualización de hora fallida");
  }
  else {
    log_d("Tarea de actualización de hora creada");
  }
}

void connectToServer()
{
  // Configurar el cliente seguro
  client.setCACert(server_cert);
  client.setInsecure();
  // Inicializar el servidor
  if (!client.connect(serverIP, SERVER_PORT)) {
    log_e("Fallo al conectar al servidor");
  }
  else {
    log_i("Conectado al servidor");
  }
}

void startCamera()
{
  // Inicializar la cámara
  if (!camera.init()) {
    log_e("Error al inicializar la cámara");
  }
  else {
    log_i("Cámara inicializada correctamente");
  }
}

bool sendImageToServer()
{
  if (deviceActivated == false) {
    log_e("El dispositivo no esta activado, se requiere activación");
    return true;
  }
  log_i("Encendiendo encendiendo camara y flash");
  // Encender el flash
  digitalWrite(FLASH_PIN, HIGH);
  // Iniciar el efecto de LED
  setLedEffect(Flashing);
  camera_fb_t*  fb        = nullptr;
  unsigned long startTime = millis();

  while (true) {
    fb = camera.getImage();
    if (!fb) {
      log_e("Error al capturar la imagen");
      // Apagar el flash
      log_i("Apagando camara y flash");
      digitalWrite(FLASH_PIN, LOW);
      return false;
    }
    // Verificar si han pasado 2 segundos
    if (millis() - startTime >= 2000) {
      break;
    }
    // Devolver el buffer de la imagen si no es la que se enviará
    camera.returnBuffer();
  }

  // Convertir la imagen a base64
  String imageBase64 = base64::encode(fb->buf, fb->len);

  // Enviar información del sensor y timestamp
  String sensorJson = "{";
  sensorJson += "\"sensor\":\3,";                                           // Tipo de sensor
  sensorJson += "\"timestamp\":" + String(timeClient.getEpochTime()) + "}"; // Marca temporal

  if (client.connected()) {
    client.print(sensorJson); // Enviar información del sensor
    log_d("Información del sensor enviada: %s", sensorJson.c_str());
  }
  else {
    log_e("No conectado al servidor, no se pueden enviar datos");
    camera.returnBuffer();
    connectToServer();
    // Apagar el flash
    log_i("Apagando camara y flash");
    digitalWrite(FLASH_PIN, LOW);
    return false;
  }

  // Fragmentar la imagen en partes más pequeñas
  const size_t fragmentSize = 8192; // Tamaño del fragmento
  size_t       totalLength  = imageBase64.length();
  size_t       offset       = 0;

  // Generar un ID único para este conjunto de fragmentos
  String uniqueId = String(millis()); // ID único basado en el tiempo actual

  // Calcular el número total de fragmentos
  size_t totalFragments = (totalLength + fragmentSize - 1) / fragmentSize;

  while (offset < totalLength) {
    size_t length   = min(fragmentSize, totalLength - offset);
    String fragment = imageBase64.substring(offset, offset + length);

    // Crear un JSON para este fragmento con la información adicional
    String fragmentJson = "{";
    fragmentJson += "\"unique_id\":\"" + uniqueId + "\",";
    fragmentJson += "\"fragment_number\":" + String((offset / fragmentSize) + 1) + ",";
    fragmentJson += "\"total_fragments\":" + String(totalFragments) + ",";
    fragmentJson += "\"data\":\"" + fragment + "\"}";

    // Enviar el fragmento al servidor
    if (client.connected()) {
      client.print(fragmentJson);
      // log_d("Fragmento enviado: %s", fragmentJson.c_str());
    }
    else {
      log_e("No conectado al servidor, no se pueden enviar datos");
      camera.returnBuffer();
      connectToServer();
      // Apagar el flash
      log_i("Apagando camara y flash");
      digitalWrite(FLASH_PIN, LOW);
      return false;
    }

    offset += length;
    vTaskDelay(10); // Retraso para no saturar el servidor
  }

  // Devolver el buffer de la imagen
  camera.returnBuffer();

  // Cambiar el efecto del LED a procesamiento
  setLedEffect(Processing);

  // Apagar el flash
  log_i("Apagando camara y flash");
  digitalWrite(FLASH_PIN, LOW);

  // Cambiar el estado del dispositivo a no activado para evitar enviar imagnes
  // mientras se espera una respuesta
  deviceActivated = false;
  return true;
}
