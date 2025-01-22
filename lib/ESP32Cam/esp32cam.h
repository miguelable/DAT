/**
 * @file esp32cam.h
 * @author Miguel Ferrer (mferrer@inbiot.es)
 * @brief Library for managing the ESP32-CAM camera module.
 * @version 0.1
 * @date 2025-01-14
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
 * @brief Class to manage the ESP32-CAM camera module.
 *
 * This class provides functions to initialize the camera module and capture images.
 * The class uses the ESP32 Camera library to interact with the camera module.
 */
class ESP32Cam
{
public:
  /**
   * @brief Construct a new ESP32Cam object
   *
   */
  ESP32Cam(){};
  /**
   * @brief Destroy the ESP32Cam object
   *
   */
  ~ESP32Cam(){};

  /**
   * @brief Initializes the ESP32 camera with the specified configuration.
   *
   * This function sets up the camera configuration parameters such as pin assignments,
   * clock frequency, and pixel format. It also adjusts the frame size, JPEG quality,
   * and frame buffer count based on the availability of PSRAM. Finally, it initializes
   * the camera and prints an error message if the initialization fails.
   *
   * Configuration details:
   * - LEDC channel and timer
   * - Data pins (D0 to D7)
   * - Clock pins (XCLK, PCLK)
   * - Synchronization pins (VSYNC, HREF)
   * - SCCB pins (SDA, SCL)
   * - Power down and reset pins
   * - Clock frequency: 20 MHz
   * - Pixel format: JPEG
   *
   * PSRAM availability:
   * - If PSRAM is available:
   *   - Frame size: HD
   *   - JPEG quality: 10 (medium quality)
   *   - Frame buffer count: 2
   * - If PSRAM is not available:
   *   - Frame size: QVGA
   *   - JPEG quality: 12 (higher compression)
   *   - Frame buffer count: 1
   *
   */
  bool init();

  /**
   * @brief Returns the frame buffer to the ESP32 camera driver.
   *
   * This function releases the frame buffer that was previously obtained
   * from the ESP32 camera driver, making it available for future use.
   */
  void returnBuffer();

  /**
   * @brief Captures an image using the ESP32 camera and returns the framebuffer.
   *
   * This function captures an image using the ESP32 camera module and stores it
   * in the internal framebuffer. If the capture is successful, it returns a pointer
   * to the framebuffer containing the image data. If the capture fails, it logs an
   * error message and returns nullptr.
   *
   * @return camera_fb_t* Pointer to the framebuffer containing the captured image,
   *                      or nullptr if the capture failed.
   */
  camera_fb_t* getImage();

  /**
   * @brief Retrieves the current camera configuration.
   *
   * @return The current camera configuration as a camera_config_t object.
   */
  camera_config_t getConfig();

private:
  camera_config_t _config;
  camera_fb_t*    _framebuffer; /*!< Frame buffer for storing captured images. */
};

#endif // ESP32CAM_H