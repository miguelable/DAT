#ifndef LEDEFFECTS_H
#define LEDEFFECTS_H

#include <NeoPixelAnimator.h>
#include <NeoPixelBus.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

// Configuración
extern const uint16_t PixelCount;
extern const uint8_t  PixelPin;
extern const uint8_t  AnimationChannels;

// Declaraciones de objetos
extern NeoPixelBus<NeoGrbFeature, NeoWs2812xMethod> strip;
extern NeoPixelAnimator                             animations;

// Task y cola para gestionar las animaciones
extern TaskHandle_t  ledTask;
extern QueueHandle_t effectQueue;

// Enumerar casos de efectos para el led
enum LedEffect
{
  None,
  Clear,
  Flashing,
  Processing,
  Success,
  Error,
  Warning,
  Closed,
  Opened,
  Waiting,
  RevisionNeeded
};

// Declaraciones de funciones
void setupLed();
void setLedEffect(LedEffect effect);

#endif // LEDEFFECTS_H
