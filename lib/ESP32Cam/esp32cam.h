/**
 * @file esp32cam.h
 * @author Miguel Ferrer
 * @brief Biblioteca para gestionar el módulo de cámara ESP32-CAM.
 * @version 0.1
 * @date 2025-01-14
 *
 * Este archivo contiene la declaración de la clase ESP32Cam, que proporciona funciones para inicializar el módulo de
 * cámara ESP32-CAM y capturar imágenes. La clase utiliza la biblioteca ESP32 Camera para interactuar con el módulo de
 * cámara.
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef ESP32CAM_H
#define ESP32CAM_H

#include "camConfig.h"
#include "esp_camera.h"
#include <Arduino.h>

/**
 * @brief Clase para gestionar el módulo de cámara ESP32-CAM.
 *
 * Esta clase proporciona funciones para inicializar el módulo de cámara y capturar imágenes.
 * La clase utiliza la biblioteca ESP32 Camera para interactuar con el módulo de cámara.
 */
class ESP32Cam
{
public:
  /**
   * @brief Constructor para el objeto ESP32Cam
   *
   */
  ESP32Cam(){};
  /**
   * @brief Destructor para el objeto ESP32Cam
   *
   */
  ~ESP32Cam(){};

  /**
   * @brief Inicializa la cámara ESP32 con la configuración especificada.
   *
   * Esta función configura los parámetros de configuración de la cámara, como las asignaciones de pines,
   * la frecuencia del reloj y el formato de píxeles. También ajusta el tamaño del marco, la calidad JPEG
   * y la cantidad de búferes de marco según la disponibilidad de PSRAM. Finalmente, inicializa la cámara
   * e imprime un mensaje de error si la inicialización falla.
   *
   * Detalles de configuración:
   * - Canal y temporizador LEDC
   * - Pines de datos (D0 a D7)
   * - Pines de reloj (XCLK, PCLK)
   * - Pines de sincronización (VSYNC, HREF)
   * - Pines SCCB (SDA, SCL)
   * - Pines de apagado y reinicio
   * - Frecuencia del reloj: 20 MHz
   * - Formato de píxeles: JPEG
   *
   * Disponibilidad de PSRAM:
   * - Si PSRAM está disponible:
   *   - Tamaño del marco: HD
   *   - Calidad JPEG: 10 (calidad media)
   *   - Cantidad de búferes de marco: 2
   * - Si PSRAM no está disponible:
   *   - Tamaño del marco: QVGA
   *   - Calidad JPEG: 12 (mayor compresión)
   *   - Cantidad de búferes de marco: 1
   *
   */
  bool init();

  /**
   * @brief Devuelve el búfer de marco al controlador de la cámara ESP32.
   *
   * Esta función libera el búfer de marco que se obtuvo previamente
   * del controlador de la cámara ESP32, haciéndolo disponible para uso futuro.
   */
  void returnBuffer();

  /**
   * @brief Captura una imagen usando la cámara ESP32 y devuelve el búfer de marco.
   *
   * Esta función captura una imagen usando el módulo de cámara ESP32 y la almacena
   * en el búfer de marco interno. Si la captura es exitosa, devuelve un puntero
   * al búfer de marco que contiene los datos de la imagen. Si la captura falla, registra un
   * mensaje de error y devuelve nullptr.
   *
   * @return camera_fb_t* Puntero al búfer de marco que contiene la imagen capturada,
   *                      o nullptr si la captura falló.
   */
  camera_fb_t* getImage();

  /**
   * @brief Recupera la configuración actual de la cámara.
   *
   * @return La configuración actual de la cámara como un objeto camera_config_t.
   */
  camera_config_t getConfig();

private:
  camera_config_t _config;      /*!< Configuración de la cámara ESP32. */
  camera_fb_t*    _framebuffer; /*!< Búfer de marco para almacenar imágenes capturadas. */
};

#endif // ESP32CAM_H