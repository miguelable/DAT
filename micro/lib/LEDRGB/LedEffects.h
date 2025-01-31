/**
 * @file LedEffects.h
 * @author Miguel Ferrer
 * @brief Biblioteca para gestionar efectos LED en una tira LED RGB.
 * @version 0.1
 * @date 2025-01-14
 *
 * Esta biblioteca proporciona funciones para gestionar efectos LED en una tira LED RGB.
 * La biblioteca utiliza las bibliotecas NeoPixelBus y NeoPixelAnimator para controlar la tira LED.
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef LEDEFFECTS_H
#define LEDEFFECTS_H

#include <NeoPixelAnimator.h>
#include <NeoPixelBus.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

// Configuración
extern const uint16_t PixelCount;        /*!< Número de píxeles en la tira LED. */
extern const uint8_t  PixelPin;          /*!< Número de pin donde está conectada la tira LED. */
extern const uint8_t  AnimationChannels; /*!< Número de canales de animación. */

// Declaraciones de objetos
extern NeoPixelBus<NeoGrbFeature, NeoWs2812xMethod> strip;      /*!< Objeto para controlar la tira LED. */
extern NeoPixelAnimator                             animations; /*!< Objeto para gestionar animaciones. */

// Tarea y cola para gestionar las animaciones
extern TaskHandle_t  ledTask;     /*!< Tarea para gestionar efectos LED. */
extern QueueHandle_t effectQueue; /*!< Cola para enviar efectos LED. */

/**
 * @brief Enumeración de efectos LED.
 *
 * Esta enumeración define los diferentes efectos LED que se pueden aplicar a la tira LED.
 * Los efectos incluyen parpadeo, procesamiento, éxito, error, advertencia, cerrado, abierto, espera y revisión
 * necesaria. El efecto None se utiliza para limpiar la tira LED.
 *
 */
enum LedEffect
{
  None,          /*!< Sin efecto. */
  Clear,         /*!< Limpiar la tira LED. */
  Flashing,      /*!< Efecto de parpadeo con color cian. */
  Processing,    /*!< Efecto de procesamiento con color cian desvaneciéndose. */
  Success,       /*!< Efecto de éxito con color verde. */
  Error,         /*!< Efecto de error con color rojo. */
  Warning,       /*!< Efecto de advertencia con color naranja desvaneciéndose. */
  Closed,        /*!< Efecto de cerrado con color rojo desvaneciéndose. */
  Opened,        /*!< Efecto de abierto con color verde desvaneciéndose. */
  Waiting,       /*!< Efecto de espera con color gris desvaneciéndose. */
  RevisionNeeded /*!< Efecto de revisión necesaria con color amarillo. */
};

/**
 * @brief Inicializa la tira LED y configura la tarea y la cola para los efectos LED.
 *
 * Esta función realiza los siguientes pasos:
 * 1. Inicializa la tira LED y muestra el estado inicial.
 * 2. Crea una cola para los efectos LED con un máximo de 5 elementos.
 *    - Si la creación de la cola falla, imprime un mensaje de error y retorna.
 *    - Si la creación de la cola tiene éxito, imprime un mensaje de éxito.
 * 3. Crea una tarea para manejar los efectos LED.
 *    - Si la creación de la tarea falla, imprime un mensaje de error.
 *    - Si la creación de la tarea tiene éxito, imprime un mensaje de éxito.
 */
void setupLed();

/**
 * @brief Establece el efecto LED enviándolo a la cola de efectos.
 *
 * Esta función envía el efecto LED especificado a la cola de efectos si la cola no es nula.
 * El efecto será procesado por el consumidor de la cola.
 *
 * @param effect El efecto LED a establecer.
 */
void setLedEffect(LedEffect effect);

#endif // LEDEFFECTS_H