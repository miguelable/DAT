#include "LedEffects.h"
#include "hcsr04.h"
#include <Arduino.h>

#define FLASH_PIN 4
#define PIXEL_COUNT 1
#define LED_PIN 13
#define TRIG_PIN 14
#define ECHO_PIN 15
#define BUTTON_PIN 12

HCSR04 sensor(TRIG_PIN, ECHO_PIN);

void activationCallback()
{
  // Aquí se encenderá el led y la cámara
  Serial.println("Encendiendo Led");
  digitalWrite(FLASH_PIN, HIGH);
  Serial.println("Encendiendo Camara");
  delay(1000);
  digitalWrite(FLASH_PIN, LOW);
}

void setup()
{
  Serial.begin(115200); // UART1 para logs
  sensor.setCallback(15, activationCallback);
  sensor.init();
  pinMode(FLASH_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);
  setupLed();
  Serial.println("Setup done");
}

void loop()
{
  static uint8_t effectIndex = 8;

  // Ciclo a través de los diferentes efectos
  switch (effectIndex) {
    case 0:
      Serial.println("Effect: None");
      setLedEffect(None);
      break;
    case 1:
      Serial.println("Effect: Flashing");
      setLedEffect(Flashing);
      break;
    case 2:
      Serial.println("Effect: Processing");
      setLedEffect(Processing);
      break;
    case 3:
      Serial.println("Effect: Success");
      setLedEffect(Success);
      break;
    case 4:
      Serial.println("Effect: Error");
      setLedEffect(Error);
      break;
    case 5:
      Serial.println("Effect: Warning");
      setLedEffect(Warning);
      break;
    case 6:
      Serial.println("Effect: Closed");
      setLedEffect(Closed);
      break;
    case 7:
      Serial.println("Effect: Opened");
      setLedEffect(Opened);
      break;
    case 8:
      Serial.println("Effect: Waiting");
      setLedEffect(Waiting);
      break;
    case 9:
      Serial.println("Effect: RevisionNeeded");
      setLedEffect(RevisionNeeded);
      break;
    default:
      Serial.println("Effect: Cleared");
      setLedEffect(Clear);
      effectIndex = 0;
      return;
  }

  if (digitalRead(BUTTON_PIN) == HIGH) {
    effectIndex++;
    Serial.println("Button pressed");
    delay(500);
  }
  delay(1);
}
