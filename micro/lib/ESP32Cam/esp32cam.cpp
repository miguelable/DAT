/**
 * @file esp32cam.cpp
 * @author Miguel Ferrer
 * @brief  Implementación de la clase ESP32Cam
 * @version 0.1
 * @date 2025-01-14
 *
 Este archivo contiene la implementación de la clase ESP32Cam, que proporciona funciones para gestionar el módulo de
 cámara ESP32-CAM.
 * La clase utiliza la biblioteca de cámara ESP32 para interactuar con el módulo de cámara. Las funciones incluyen
 inicialización, captura de imágenes y recuperación de configuración.
 *
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "esp32cam.h"

bool ESP32Cam::init()
{
  _config.ledc_channel = LEDC_CHANNEL_0;
  _config.ledc_timer   = LEDC_TIMER_0;
  _config.pin_d0       = Y2_GPIO_NUM;
  _config.pin_d1       = Y3_GPIO_NUM;
  _config.pin_d2       = Y4_GPIO_NUM;
  _config.pin_d3       = Y5_GPIO_NUM;
  _config.pin_d4       = Y6_GPIO_NUM;
  _config.pin_d5       = Y7_GPIO_NUM;
  _config.pin_d6       = Y8_GPIO_NUM;
  _config.pin_d7       = Y9_GPIO_NUM;
  _config.pin_xclk     = XCLK_GPIO_NUM;
  _config.pin_pclk     = PCLK_GPIO_NUM;
  _config.pin_vsync    = VSYNC_GPIO_NUM;
  _config.pin_href     = HREF_GPIO_NUM;
  _config.pin_sccb_sda = SIOD_GPIO_NUM;
  _config.pin_sccb_scl = SIOC_GPIO_NUM;
  _config.pin_pwdn     = PWDN_GPIO_NUM;
  _config.pin_reset    = RESET_GPIO_NUM;
  _config.xclk_freq_hz = 20000000;
  _config.pixel_format = PIXFORMAT_JPEG;

  // Configuración basada en la disponibilidad de PSRAM
  if (psramFound()) {
    _config.frame_size   = FRAMESIZE_SXGA; // Reduce resolución para mayor fluidez
    _config.jpeg_quality = 8;              // Calidad media para menor tamaño de imagen
    _config.fb_count     = 2;
  }
  else {
    _config.frame_size   = FRAMESIZE_QVGA;
    _config.jpeg_quality = 12; // Mayor compresión si no hay PSRAM
    _config.fb_count     = 1;
  }

  // Inicializar cámara
  esp_err_t err = esp_camera_init(&_config);
  if (err != ESP_OK) {
    Serial.printf("Fallo al inicializar la cámara: 0x%x\n", err);
    return false;
  }
  return true;
}

camera_fb_t* ESP32Cam::getImage()
{
  _framebuffer = esp_camera_fb_get();
  if (!_framebuffer) {
    log_e("Error al capturar la imagen");
    return nullptr;
  }
  return _framebuffer;
}

void ESP32Cam::returnBuffer()
{
  esp_camera_fb_return(_framebuffer);
}

camera_config_t ESP32Cam::getConfig()
{
  return _config;
}