#include "LedEffects.h"
#include <Arduino.h>
#include <freertos/queue.h>
#include <freertos/task.h>

// Configuración
const uint16_t PixelCount        = 1;
const uint8_t  PixelPin          = 13;
const uint8_t  AnimationChannels = 1;

// Objetos globales
NeoPixelBus<NeoGrbFeature, NeoWs2812xMethod> strip(PixelCount, PixelPin);
NeoPixelAnimator                             animations(AnimationChannels);

// Task y cola para animaciones
TaskHandle_t  ledTask;
QueueHandle_t effectQueue;

// Variables internas
struct MyAnimationState
{
  RgbColor StartingColor;
  RgbColor EndingColor;
};
MyAnimationState animationState[AnimationChannels];

// Función para actualizar animaciones
void blendAnimUpdate(const AnimationParam& param)
{
  RgbColor updatedColor = RgbColor::LinearBlend(animationState[param.index].StartingColor,
                                                animationState[param.index].EndingColor, param.progress);
  for (uint16_t pixel = 0; pixel < PixelCount; pixel++) {
    strip.SetPixelColor(pixel, updatedColor);
  }
}

// Funciones de animación
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

void fadeOut(uint16_t time)
{
  fadeIn(time, RgbColor(0));
}

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
      break;
    case Error:
      fadeIn(500, RgbColor(255, 0, 0));
      vTaskDelay(3000 / portTICK_PERIOD_MS);
      fadeOut(500);
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
    Serial.println("Error: No se pudo crear la cola.");
    return;
  }
  else {
    Serial.println("Cola de efectos creada.");
  }

  if (xTaskCreate(ledTaskFunction, "LED Task", 2048, nullptr, 1, &ledTask) != pdPASS) {
    Serial.println("Error: No se pudo crear la tarea LED.");
  }
  else {
    Serial.println("Tarea LED creada.");
  }
}
