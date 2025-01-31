/**
 * @file websockets.h
 * @author Miguel Ferrer
 * @brief  Librería para WebSocket y cámara ESP32.
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

#define SERVER_PORT 54472 /*!< Puerto del servidor WebSocket. */
#define FLASH_PIN 4       /*!< Pin del flash de la cámara. */

// Device status
extern bool deviceActivated; /*!< Estado de activación del dispositivo. */

extern TaskHandle_t sendDataTask; /*!< Manejador de la tarea de envío de datos. */

extern NTPClient timeClient; /*!< Cliente NTP para obtener la hora actual. */

/**
 * @brief Conecta a la red WiFi utilizando el SSID y la contraseña proporcionados.
 *
 * Esta función intenta conectarse a la red WiFi y espera hasta que la conexión se establezca.
 * Una vez conectado, registra la dirección IP del dispositivo y configura la dirección IP del servidor.
 * Además, inicializa el NTPClient y crea una tarea para actualizar la hora cada segundo.
 *
 * @note Esta función utiliza tareas y retardos de FreeRTOS.
 *
 */
void connectToWiFi();

/**
 * @brief Establece una conexión segura con el servidor.
 *
 * Esta función configura el cliente seguro con el certificado del servidor,
 * establece el cliente para operar en modo inseguro, e intenta conectarse
 * al servidor utilizando la dirección IP y el puerto especificados. Registra
 * un mensaje de error si la conexión falla, o un mensaje de éxito si la conexión
 * se establece.
 */
void connectToServer();

/**
 * @brief Inicializa la cámara y registra el resultado.
 *
 * Esta función intenta inicializar la cámara. Si la inicialización
 * es exitosa, registra un mensaje informativo. Si la inicialización
 * falla, registra un mensaje de error.
 */
void startCamera();

/**
 * @brief Envía una imagen capturada desde la cámara al servidor.
 *
 * Esta función realiza los siguientes pasos:
 * 1. Verifica si el dispositivo está activado. Si no lo está, registra un error y retorna true.
 * 2. Enciende la cámara y el flash, y comienza el efecto LED.
 * 3. Captura una imagen desde la cámara. Si la captura falla, registra un error, apaga el flash y retorna false.
 * 4. Convierte la imagen capturada a una cadena codificada en base64.
 * 5. Envía información del sensor y la marca temporal al servidor.
 * 6. Fragmenta la imagen codificada en base64 en partes más pequeñas y envía cada fragmento al servidor.
 *    Los fragmentos incluyen un ID único, el número de fragmento y el número total de fragmentos para luego
 *    ensamblar la imagen en el servidor.
 * 7. Devuelve el buffer de la imagen a la cámara.
 * 8. Cambia el efecto del LED a procesamiento.
 * 9. Apaga el flash.
 * 10. Desactiva el dispositivo para evitar enviar imágenes mientras se espera una respuesta.
 *
 * @return true si la imagen fue enviada exitosamente al servidor, false en caso contrario.
 */
bool sendImageToServer();

#endif // WEBSOCKETS_H