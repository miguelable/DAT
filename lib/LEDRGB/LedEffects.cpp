/**
 * @file LedEffects.cpp
 * @author Miguel Ferrer (mferrer@inbiot.es
 * @brief  Implementation of LedEffects class
 * @version 0.1
 * @date 2025-01-14
 *
 * This file contains the implementation of the LedEffects class, which provides functions to manage LED effects on an
 * RGB LED strip. The class uses the NeoPixelBus and NeoPixelAnimator libraries to control the LED strip. The different
 * LED effects are defined in the LedEffect enumeration.
 * 1. None: No effect.
 * 2. Clear: Clear the LED strip.
 * 3. Flashing: Flashing effect with cyan color.
 * 4. Processing: Processing effect with cyan color fading in and out.
 * 5. Success: Success effect with green color.
 * 6. Error: Error effect with red color.
 * 7. Warning: Warning effect with orange color fading in and out.
 * 8. Closed: Closed effect with red color fading in and out.
 * 9. Opened: Opened effect with green color fading in and out.
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "LedEffects.h"
#include <Arduino.h>
#include <freertos/task.h>

// Configuración
const uint16_t PixelCount        = 1;
const uint8_t  PixelPin          = 13;
const uint8_t  AnimationChannels = 1;

// Objetos globales
NeoPixelBus<NeoGrbFeature, NeoWs2812xMethod> strip(PixelCount, PixelPin);   /*!< Controller of the LED strip. */
NeoPixelAnimator                             animations(AnimationChannels); /*!< Manager of animations. */

// Task y cola para animaciones
TaskHandle_t  ledTask;
QueueHandle_t effectQueue;

// Variables internas
struct MyAnimationState
/**
 * @brief Structure to represent the starting and ending colors for an LED effect.
 *
 * This structure contains two members:
 * - StartingColor: The initial color of the LED effect.
 * - EndingColor: The final color of the LED effect.
 */
{
  RgbColor StartingColor; /*!< The starting color of the LED effect. */
  RgbColor EndingColor;   /*!< The ending color of the LED effect. */
};
MyAnimationState animationState[AnimationChannels]; /*!< Array of animation states. */

// Función para actualizar animaciones
/**
 * @brief Updates the color of the LED strip based on the animation parameters.
 *
 * This function performs a linear blend between the starting and ending colors
 * of the animation state for the given animation parameter. It then sets the
 * color of each pixel in the LED strip to the resulting blended color.
 *
 * @param param The animation parameter containing the index and progress of the animation.
 */
void blendAnimUpdate(const AnimationParam& param)
{
  RgbColor updatedColor = RgbColor::LinearBlend(animationState[param.index].StartingColor,
                                                animationState[param.index].EndingColor, param.progress);
  for (uint16_t pixel = 0; pixel < PixelCount; pixel++) {
    strip.SetPixelColor(pixel, updatedColor);
  }
}

// Funciones de animación
/**
 * @brief Fades in an LED to a target color over a specified time.
 *
 * This function initiates an animation that gradually changes the color of an LED
 * from its current color to a specified target color over a given duration.
 *
 * @param time The duration of the fade-in effect in milliseconds.
 * @param target The target color to fade into, represented as an RgbColor object.
 */
void fadeIn(uint16_t time, RgbColor target)
{
  animationState[0].StartingColor = strip.GetPixelColor<RgbColor>(0);
  animationState[0].EndingColor   = target;
  animations.StartAnimation(0, time, blendAnimUpdate);
  while (animations.IsAnimating()) {
    animations.UpdateAnimations();
    strip.Show();
    vTaskDelay(1 / portTICK_PERIOD_MS);
  }
}

/**
 * @brief Gradually fades out the LED to off state over a specified time.
 *
 * This function reduces the brightness of the LED to zero over the duration
 * specified by the `time` parameter.
 *
 * @param time The duration in milliseconds over which the LED should fade out.
 */
void fadeOut(uint16_t time)
{
  fadeIn(time, RgbColor(0));
}

/**
 * @brief Executes a fade in and fade out effect on an LED with specified parameters.
 *
 * This function performs a fade in and fade out animation on an LED with the given target color.
 * It repeats the animation for a specified number of times, or indefinitely if repeat is set to 0.
 * The function also checks for new effects in the queue and exits the loop if a new effect is detected.
 *
 * @param time The duration of the fade in and fade out animations in milliseconds.
 * @param target The target color for the fade in animation.
 * @param wait The wait time in milliseconds between each repetition of the animation.
 * @param repeat The number of times to repeat the animation. If set to 0, the animation repeats indefinitely.
 */
void fadeInOut(uint16_t time, RgbColor target, uint16_t wait, uint16_t repeat)
{
  uint16_t currentRepeat = repeat; // Mantener un contador local del número de repeticiones
  do {
    // Comprobar si se envió un nuevo efecto durante el bucle
    LedEffect newEffect;
    if (xQueuePeek(effectQueue, &newEffect, 0) == pdPASS && newEffect != None) {
      // Si hay un nuevo efecto en la cola, salir del bucle
      break;
    }
    // Ejecutar las animaciones de fade in y fade out
    fadeIn(time, target);
    fadeOut(time);
    // Esperar antes de la próxima iteración
    vTaskDelay(wait / portTICK_PERIOD_MS);
    // Reducir el contador solo si repeat es mayor a 0
    if (repeat > 0) {
      currentRepeat--;
    }

  } while (repeat == 0 || currentRepeat > 0); // Repetir indefinidamente si repeat == 0
}

// Implementación del efecto
/**
 * @brief Handles different LED effects based on the provided LedEffect enum.
 *
 * @param effect The LedEffect enum value that determines which LED effect to execute.
 *
 * The function supports the following effects:
 * - Clear: Fades out the LED over 500 milliseconds.
 * - Flashing: Fades in the LED with cyan color (0, 255, 255) over 500 milliseconds.
 * - Processing: Fades in and out the LED with cyan color (0, 255, 255) over 1000 milliseconds, with 200 milliseconds
 * on and 0 milliseconds off.
 * - Success: Fades in the LED with green color (0, 255, 0) over 500 milliseconds, waits for 3000 milliseconds, then
 * fades out over 500 milliseconds.
 * - Error: Fades in the LED with red color (255, 0, 0) over 500 milliseconds, waits for 3000 milliseconds, then fades
 * out over 500 milliseconds.
 * - Warning: Fades in and out the LED with orange color (255, 128, 0) over 500 milliseconds, with 200 milliseconds on
 * and 0 milliseconds off.
 * - Closed: Fades in and out the LED with red color (255, 0, 0) over 3000 milliseconds, with 1000 milliseconds on and
 * 0 milliseconds off.
 * - Opened: Fades in and out the LED with green color (0, 255, 0) over 3000 milliseconds, with 100 milliseconds on
 * and 0 milliseconds off.
 * - Waiting: Fades in and out the LED with gray color (128, 128, 128) over 1000 milliseconds, with 200 milliseconds
 * on and 0 milliseconds off.
 * - RevisionNeeded: Fades in the LED with yellow color (255, 255, 0) over 500 milliseconds.
 */
void handleEffect(LedEffect effect)
{
  switch (effect) {
    case Clear:
      fadeOut(500);
      break;
    case Flashing:
      fadeIn(500, RgbColor(0, 255, 255));
      break;
    case Processing:
      fadeInOut(1000, RgbColor(0, 255, 255), 200, 0);
      break;
    case Success:
      fadeIn(500, RgbColor(0, 255, 0));
      vTaskDelay(3000 / portTICK_PERIOD_MS);
      fadeOut(500);
      setLedEffect(Waiting);
      break;
    case Error:
      fadeIn(500, RgbColor(255, 0, 0));
      vTaskDelay(3000 / portTICK_PERIOD_MS);
      fadeOut(500);
      setLedEffect(Waiting);
      break;
    case Warning:
      fadeInOut(500, RgbColor(255, 128, 0), 200, 0);
      break;
    case Closed:
      fadeInOut(3000, RgbColor(255, 0, 0), 1000, 0);
      break;
    case Opened:
      fadeInOut(3000, RgbColor(0, 255, 0), 100, 0);
      break;
    case Waiting:
      fadeInOut(1000, RgbColor(128, 128, 128), 200, 0);
      break;
    case RevisionNeeded:
      fadeIn(500, RgbColor(255, 255, 0));
      break;
  }
}

// Tarea principal de gestión de LED
/**
 * @brief Task function to handle LED effects.
 *
 * This function runs indefinitely, waiting for new LED effects to be received
 * from a queue. When a new effect is received, it processes the effect by
 * clearing the queue to avoid accumulation of old effects and then executing
 * the received effect.
 *
 * @param pvParameters Pointer to the parameters passed to the task (not used).
 */
void ledTaskFunction(void* pvParameters)
{
  LedEffect currentEffect = None;
  while (true) {
    // Esperar un nuevo efecto en la cola
    if (xQueueReceive(effectQueue, &currentEffect, portMAX_DELAY)) {
      // Si se recibió un nuevo efecto, procesarlo
      if (currentEffect != None) {
        // Limpia la cola para evitar que se acumulen efectos antiguos
        xQueueReset(effectQueue);
        // Ejecutar el efecto
        handleEffect(currentEffect);
      }
    }
  }
}

// Función para establecer un efecto
void setLedEffect(LedEffect effect)
{
  if (effectQueue != nullptr) {
    xQueueSend(effectQueue, &effect, 0);
  }
}

// Inicialización de LED y Task
void setupLed()
{
  strip.Begin();
  strip.Show();

  effectQueue = xQueueCreate(5, sizeof(LedEffect));
  if (effectQueue == nullptr) {
    log_e("No se pudo crear la cola");
    return;
  }
  else {
    log_d("Cola de efectos creada");
  }

  if (xTaskCreate(ledTaskFunction, "LED Task", 2048, nullptr, 1, &ledTask) != pdPASS) {
    log_e("No se pudo crear la tarea LED");
  }
  else {
    log_i("Tarea LED creada");
  }
}
