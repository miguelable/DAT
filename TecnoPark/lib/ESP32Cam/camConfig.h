/**
 * @file camConfig.h
 * @author Miguel Ferrer (mferrer@inbiot.es)
 * @brief Configuration file for the ESP32-CAM camera module.
 * @version 0.1
 * @date 2025-01-14
 *
 * This file contains the pin configuration for the ESP32-CAM camera module.
 * The configuration includes the pin assignments for the camera module's data, clock, and reset lines.
 * The pin assignments are based on the ESP32 GPIO numbers.
 * The configuration also includes the pixel format and clock frequency for the camera module.
 * The configuration file is used by the ESP32Cam class to initialize the camera module.
 * The configuration can be modified to match the pin assignments of the specific ESP32-CAM board.
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef CAMCONFIG_H
#define CAMCONFIG_H

// Pines de la cámara
#define PWDN_GPIO_NUM 32  /*!< Shutdown pin for the camera module. */
#define RESET_GPIO_NUM -1 /*!< Reset pin for the camera module. */
#define XCLK_GPIO_NUM 0   /*!< External clock pin for the camera module. */
#define SIOD_GPIO_NUM 26  /*!< SCCB data pin for the camera module. */
#define SIOC_GPIO_NUM 27  /*!< SCCB clock pin for the camera module. */

#define Y9_GPIO_NUM 35    /*!< Data pin D9 for the camera module. */
#define Y8_GPIO_NUM 34    /*!< Data pin D8 for the camera module. */
#define Y7_GPIO_NUM 39    /*!< Data pin D7 for the camera module. */
#define Y6_GPIO_NUM 36    /*!< Data pin D6 for the camera module. */
#define Y5_GPIO_NUM 21    /*!< Data pin D5 for the camera module. */
#define Y4_GPIO_NUM 19    /*!< Data pin D4 for the camera module. */
#define Y3_GPIO_NUM 18    /*!< Data pin D3 for the camera module. */
#define Y2_GPIO_NUM 5     /*!< Data pin D2 for the camera module. */
#define VSYNC_GPIO_NUM 25 /*!< Vertical synchronization pin for the camera module. */
#define HREF_GPIO_NUM 23  /*!< Horizontal synchronization pin for the camera module. */
#define PCLK_GPIO_NUM 22  /*!< Pixel clock pin for the camera module. */

#endif // CAMCONFIG_H