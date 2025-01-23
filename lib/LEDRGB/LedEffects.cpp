/**
 * @file LedEffects.cpp
 * @author Miguel Ferrer
 * @brief Implementación de la clase LedEffects
 * @version 0.1
 * @date 2025-01-14
 *
 * Este archivo contiene la implementación de la clase LedEffects, que proporciona funciones para gestionar efectos LED
 * en una tira LED RGB. La clase utiliza las bibliotecas NeoPixelBus y NeoPixelAnimator para controlar la tira LED. Los
 * diferentes efectos LED están definidos en la enumeración LedEffect.
 * 1. None: Sin efecto.
 * 2. Clear: Limpiar la tira LED.
 * 3. Flashing: Efecto de parpadeo con color cian.
 * 4. Processing: Efecto de procesamiento con color cian desvaneciéndose.
 * 5. Success: Efecto de éxito con color verde.
 * 6. Error: Efecto de error con color rojo.
 * 7. Warning: Efecto de advertencia con color naranja desvaneciéndose.
 * 8. Closed: Efecto de cerrado con color rojo desvaneciéndose.
 * 9. Opened: Efecto de abierto con color verde desvaneciéndose.
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "LedEffects.h"
#include <Arduino.h>
#include <freertos/task.h>

// Configuración
const uint16_t PixelCount        = 1;  /*!< Número de píxeles en la tira LED. */
const uint8_t  PixelPin          = 13; /*!< Número de pin donde está conectada la tira LED. */
const uint8_t  AnimationChannels = 1;  /*!< Número de canales de animación. */

// Objetos globales
NeoPixelBus<NeoGrbFeature, NeoWs2812xMethod> strip(PixelCount, PixelPin);   /*!< Controlador de la tira LED. */
NeoPixelAnimator                             animations(AnimationChannels); /*!< Gestor de animaciones. */

// Tarea y cola para animaciones
TaskHandle_t  ledTask;     /*!< Manejador de la tarea de efectos LED. */
QueueHandle_t effectQueue; /*!< Cola para enviar efectos LED. */

// Variables internas
struct MyAnimationState
{
  /**
   * @brief Estructura para representar los colores inicial y final de un efecto LED.
   *
   * Esta estructura contiene dos miembros:
   * - StartingColor: El color inicial del efecto LED.
   * - EndingColor: El color final del efecto LED.
   */
  RgbColor StartingColor; /*!< El color inicial del efecto LED. */
  RgbColor EndingColor;   /*!< El color final del efecto LED. */
};
MyAnimationState animationState[AnimationChannels]; /*!< Array de estados de animación. */

// Función para actualizar animaciones
/**
 * @brief Actualiza el color de la tira LED basado en los parámetros de animación.
 *
 * Esta función realiza una mezcla lineal entre los colores inicial y final
 * del estado de animación para el parámetro de animación dado. Luego establece el
 * color de cada píxel en la tira LED al color resultante de la mezcla.
 *
 * @param param El parámetro de animación que contiene el índice y el progreso de la animación.
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
 * @brief Desvanece un LED a un color objetivo durante un tiempo especificado.
 *
 * Esta función inicia una animación que cambia gradualmente el color de un LED
 * desde su color actual a un color objetivo especificado durante una duración dada.
 *
 * @param time La duración del efecto de desvanecimiento en milisegundos.
 * @param target El color objetivo al que se desvanecerá, representado como un objeto RgbColor.
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
 * @brief Desvanece gradualmente el LED hasta el estado apagado durante un tiempo especificado.
 *
 * Esta función reduce el brillo del LED a cero durante la duración
 * especificada por el parámetro `time`.
 *
 * @param time La duración en milisegundos durante la cual el LED debe desvanecerse.
 */
void fadeOut(uint16_t time)
{
  fadeIn(time, RgbColor(0));
}

/**
 * @brief Ejecuta un efecto de desvanecimiento y desvanecimiento en un LED con parámetros especificados.
 *
 * Esta función realiza una animación de desvanecimiento y desvanecimiento en un LED con el color objetivo dado.
 * Repite la animación un número especificado de veces, o indefinidamente si repeat se establece en 0.
 * La función también verifica si hay nuevos efectos en la cola y sale del bucle si se detecta un nuevo efecto.
 *
 * @param time La duración de las animaciones de desvanecimiento y desvanecimiento en milisegundos.
 * @param target El color objetivo para la animación de desvanecimiento.
 * @param wait El tiempo de espera en milisegundos entre cada repetición de la animación.
 * @param repeat El número de veces que se debe repetir la animación. Si se establece en 0, la animación se repite
 * indefinidamente.
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
 * @brief Maneja diferentes efectos LED basados en la enumeración LedEffect proporcionada.
 *
 * @param effect El valor de la enumeración LedEffect que determina qué efecto LED ejecutar.
 *
 * La función admite los siguientes efectos:
 * - Clear: Desvanece el LED durante 500 milisegundos.
 * - Flashing: Desvanece el LED con color cian (0, 255, 255) durante 500 milisegundos.
 * - Processing: Desvanece y desvanece el LED con color cian (0, 255, 255) durante 1000 milisegundos, con 200
 * milisegundos encendido y 0 milisegundos apagado.
 * - Success: Desvanece el LED con color verde (0, 255, 0) durante 500 milisegundos, espera 3000 milisegundos, luego
 * desvanece durante 500 milisegundos.
 * - Error: Desvanece el LED con color rojo (255, 0, 0) durante 500 milisegundos, espera 3000 milisegundos, luego
 * desvanece durante 500 milisegundos.
 * - Warning: Desvanece y desvanece el LED con color naranja (255, 128, 0) durante 500 milisegundos, con 200
 * milisegundos encendido y 0 milisegundos apagado.
 * - Closed: Desvanece y desvanece el LED con color rojo (255, 0, 0) durante 3000 milisegundos, con 1000 milisegundos
 * encendido y 0 milisegundos apagado.
 * - Opened: Desvanece y desvanece el LED con color verde (0, 255, 0) durante 3000 milisegundos, con 100 milisegundos
 * encendido y 0 milisegundos apagado.
 * - Waiting: Desvanece y desvanece el LED con color gris (128, 128, 128) durante 1000 milisegundos, con 200
 * milisegundos encendido y 0 milisegundos apagado.
 * - RevisionNeeded: Desvanece el LED con color amarillo (255, 255, 0) durante 500 milisegundos.
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
 * @brief Función de tarea para gestionar efectos LED.
 *
 * Esta función se ejecuta indefinidamente, esperando nuevos efectos LED que se reciban
 * de una cola. Cuando se recibe un nuevo efecto, lo procesa
 * limpiando la cola para evitar la acumulación de efectos antiguos y luego ejecutando
 * el efecto recibido.
 *
 * @param pvParameters Puntero a los parámetros pasados a la tarea (no utilizado).
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
/**
 * @brief Establece el efecto LED enviándolo a la cola de efectos.
 *
 * Esta función envía el efecto LED especificado a la cola de efectos si la cola no es nula.
 * El efecto será procesado por el consumidor de la cola.
 *
 * @param effect El efecto LED a establecer.
 */
void setLedEffect(LedEffect effect)
{
  if (effectQueue != nullptr) {
    xQueueSend(effectQueue, &effect, 0);
  }
}

// Inicialización de LED y Task
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