#ifndef ESP32CAM_H
#define ESP32CAM_H

#include "camConfig.h"
#include "esp_camera.h"
#include <Arduino.h>

class ESP32Cam
{
public:
  ESP32Cam();
  ~ESP32Cam();

  void init();
  void returnBuffer();

  camera_fb_t*    getImage();
  camera_config_t getConfig();

private:
  camera_config_t _config;
  camera_fb_t*    _framebuffer;
};

#endif // ESP32CAM_H