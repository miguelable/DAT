/**
 * @file main.cpp
 * @author Miguel Ferrer (mferrer@inbiot.es)
 * @brief Main file for the TecnoPark project.
 * @version 0.1
 * @date 2025-01-14
 *
 * This file contains the main setup and loop functions for the TecnoPark project.
 * It initializes the HCSR04 ultrasonic sensor, sets up the LED effects, and handles
 * the button press to cycle through different LED effects.
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "LedEffects.h"
#include "hcsr04.h"
#include <Arduino.h>

#define FLASH_PIN 4   /*!< Pin number connected to the LED flash. */
#define PIXEL_COUNT 1 /*!< Number of pixels in the LED strip. */
#define LED_PIN 13    /*!< Pin number connected to the LED strip. */
#define TRIG_PIN 14   /*!< Pin number connected to the trigger pin of the HCSR04 sensor. */
#define ECHO_PIN 15   /*!< Pin number connected to the echo pin of the HCSR04 sensor. */
#define BUTTON_PIN 12 /*!< Pin number connected to the button. */

/**
 * @brief Creates an instance of the HCSR04 ultrasonic sensor.
 *
 * This instance is used to interact with the HCSR04 sensor, which measures
 * distance by sending an ultrasonic pulse and timing how long it takes to
 * receive the echo. The sensor is initialized with the specified trigger
 * and echo pins.
 *
 */
HCSR04 sensor(TRIG_PIN, ECHO_PIN);

/**
 * @brief Callback function to activate the LED and camera.
 *
 * This function is intended to be called when an activation event occurs.
 * It performs the following actions:
 * 1. Turns on the LED connected to FLASH_PIN.
 * 2. Prints a message to the Serial monitor indicating the LED is on.
 * 3. Turns on the camera.
 * 4. Waits for 1 second.
 * 5. Turns off the LED.
 */
void activationCallback()
{
  // Aquí se encenderá el led y la cámara
  Serial.println("Encendiendo Led");
  digitalWrite(FLASH_PIN, HIGH);
  Serial.println("Encendiendo Camara");
  delay(1000);
  digitalWrite(FLASH_PIN, LOW);
}

/**
 * @brief Initializes the serial communication, sensor, and pins.
 *
 * This function sets up the serial communication at a baud rate of 115200 for logging purposes.
 * It also sets a callback function for the sensor on pin 15 and initializes the sensor.
 * Additionally, it configures the FLASH_PIN as an output and the BUTTON_PIN as an input.
 * The setupLed() function is called to initialize the LED, and a message "Setup done" is printed
 * to the serial monitor to indicate that the setup process is complete.
 */
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

/**
 * @brief Main loop function that cycles through different LED effects based on the effectIndex.
 *
 * This function continuously checks the state of a button and cycles through various LED effects.
 * When the button is pressed, the effectIndex is incremented to switch to the next effect.
 * The effects are displayed in the Serial Monitor and applied to the LED using the setLedEffect function.
 *
 * Effects:
 * - 0: None
 * - 1: Flashing
 * - 2: Processing
 * - 3: Success
 * - 4: Error
 * - 5: Warning
 * - 6: Closed
 * - 7: Opened
 * - 8: Waiting
 * - 9: RevisionNeeded
 *
 * If the effectIndex exceeds 9, it resets to 0 and sets the effect to Clear.
 *
 * @note The button state is read from BUTTON_PIN. When the button is pressed, a 500ms delay is introduced to debounce
 * the button.
 */
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
