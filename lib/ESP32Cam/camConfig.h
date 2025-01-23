/**
 * @file camConfig.h
 * @author Miguel Ferrer
 * @brief Archivo de configuración para el módulo de cámara ESP32-CAM.
 * @version 0.1
 * @date 2025-01-14
 *
 * Este archivo contiene la configuración de pines para el módulo de cámara ESP32-CAM.
 * La configuración incluye las asignaciones de pines para las líneas de datos, reloj y reinicio del módulo de cámara.
 * Las asignaciones de pines se basan en los números GPIO del ESP32.
 * La configuración también incluye el formato de píxeles y la frecuencia del reloj para el módulo de cámara.
 * El archivo de configuración es utilizado por la clase ESP32Cam para inicializar el módulo de cámara.
 * La configuración se puede modificar para que coincida con las asignaciones de pines de la placa ESP32-CAM específica.
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef CAMCONFIG_H
#define CAMCONFIG_H

// Pines de la cámara
#define PWDN_GPIO_NUM 32  /*!< Pin de apagado para el módulo de cámara. */
#define RESET_GPIO_NUM -1 /*!< Pin de reinicio para el módulo de cámara. */
#define XCLK_GPIO_NUM 0   /*!< Pin de reloj externo para el módulo de cámara. */
#define SIOD_GPIO_NUM 26  /*!< Pin de datos SCCB para el módulo de cámara. */
#define SIOC_GPIO_NUM 27  /*!< Pin de reloj SCCB para el módulo de cámara. */

#define Y9_GPIO_NUM 35    /*!< Pin de datos D9 para el módulo de cámara. */
#define Y8_GPIO_NUM 34    /*!< Pin de datos D8 para el módulo de cámara. */
#define Y7_GPIO_NUM 39    /*!< Pin de datos D7 para el módulo de cámara. */
#define Y6_GPIO_NUM 36    /*!< Pin de datos D6 para el módulo de cámara. */
#define Y5_GPIO_NUM 21    /*!< Pin de datos D5 para el módulo de cámara. */
#define Y4_GPIO_NUM 19    /*!< Pin de datos D4 para el módulo de cámara. */
#define Y3_GPIO_NUM 18    /*!< Pin de datos D3 para el módulo de cámara. */
#define Y2_GPIO_NUM 5     /*!< Pin de datos D2 para el módulo de cámara. */
#define VSYNC_GPIO_NUM 25 /*!< Pin de sincronización vertical para el módulo de cámara. */
#define HREF_GPIO_NUM 23  /*!< Pin de sincronización horizontal para el módulo de cámara. */
#define PCLK_GPIO_NUM 22  /*!< Pin de reloj de píxeles para el módulo de cámara. */

#endif // CAMCONFIG_H