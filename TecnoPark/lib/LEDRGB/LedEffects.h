/**
 * @file LedEffects.h
 * @author Miguel Ferrer (mferrer@inbiot.es)
 * @brief  Library for managing LED effects on an RGB LED strip.
 * @version 0.1
 * @date 2025-01-14
 *
 * This library provides functions to manage LED effects on an RGB LED strip.
 * The library uses the NeoPixelBus and NeoPixelAnimator libraries to control the LED strip.
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
extern const uint16_t PixelCount;        /*!< Number of pixels in the LED strip. */
extern const uint8_t  PixelPin;          /*!< Pin number where the LED strip is connected. */
extern const uint8_t  AnimationChannels; /*!< Number of animation channels. */

// Declaraciones de objetos
extern NeoPixelBus<NeoGrbFeature, NeoWs2812xMethod> strip;      /*!< Object to control the LED strip. */
extern NeoPixelAnimator                             animations; /*!< Object to manage animations. */

// Task y cola para gestionar las animaciones
extern TaskHandle_t  ledTask;     /*!< Task to manage LED effects. */
extern QueueHandle_t effectQueue; /*!< Queue to send LED effects. */

/**
 * @brief Enumeration of LED effects.
 *
 * This enumeration defines the different LED effects that can be applied to the LED strip.
 * The effects include flashing, processing, success, error, warning, closed, opened, waiting, and revision needed.
 * The None effect is used to clear the LED strip.
 *
 */
enum LedEffect
{
  None,          /*!< No effect. */
  Clear,         /*!< Clear the LED strip. */
  Flashing,      /*!< Flashing effect with cyan color. */
  Processing,    /*!< Processing effect with cyan color fading in and out. */
  Success,       /*!< Success effect with green color. */
  Error,         /*!< Error effect with red color. */
  Warning,       /*!< Warning effect with orange color fading in and out. */
  Closed,        /*!< Closed effect with red color fading in and out. */
  Opened,        /*!< Opened effect with green color fading in and out. */
  Waiting,       /*!< Waiting effect with gray color fading in and out. */
  RevisionNeeded /*!< Revision needed effect with yellow color. */
};

/**
 * @brief Initializes the LED strip and sets up the task and queue for LED effects.
 *
 * This function performs the following steps:
 * 1. Initializes the LED strip and displays the initial state.
 * 2. Creates a queue for LED effects with a maximum of 5 elements.
 *    - If the queue creation fails, it prints an error message and returns.
 *    - If the queue creation succeeds, it prints a success message.
 * 3. Creates a task to handle LED effects.
 *    - If the task creation fails, it prints an error message.
 *    - If the task creation succeeds, it prints a success message.
 */
void setupLed();

/**
 * @brief Sets the LED effect by sending it to the effect queue.
 *
 * This function sends the specified LED effect to the effect queue if the queue is not null.
 * The effect will be processed by the consumer of the queue.
 *
 * @param effect The LED effect to be set.
 */
void setLedEffect(LedEffect effect);

#endif // LEDEFFECTS_H
