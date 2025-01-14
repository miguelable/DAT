/**
 * @file hcsr04.cpp
 * @author Miguel Ferrer (mferrer@inbiot.es)
 * @brief Implementation of HCSR04 class
 * @version 0.1
 * @date 2025-01-14
 *
 * This file contains the implementation of the HCSR04 class, which provides functions to manage the HC-SR04 ultrasonic
 * sensor. The class includes functions to initialize the sensor, start and stop measurements, and retrieve distance and
 * duration data. The class also supports setting a callback function to trigger an action when the distance exceeds a
 * specified threshold.
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "hcsr04.h"

HCSR04::HCSR04(uint8_t trigPin, uint8_t echoPin)
{
  _trigPin = trigPin;
  _echoPin = echoPin;
}

HCSR04::HCSR04()
{
  _trigPin = DEFAULT_TRIG;
  _echoPin = DEFAULT_ECHO;
}

HCSR04::~HCSR04()
{
  // Delete running tasks
  if (_sensorDataTask != NULL) {
    if (eTaskGetState(_sensorDataTask) == eSuspended)
      vTaskDelete(_sensorDataTask);
    else {
      vTaskSuspend(_sensorDataTask);
      vTaskDelete(_sensorDataTask);
      _sensorDataTask = NULL;
    }
  }
  log_i("HCSR04 object delated");
}

void HCSR04::setCallback(uint16_t threshold, ActivationCallback callback)
{
  _threshold = threshold;
  _callback  = callback;
}

void HCSR04::stopMeasure()
{
  if (_sensorDataTask != NULL) {
    if (eTaskGetState(_sensorDataTask) == eSuspended)
      log_w("sensorDataTask already suspended");
    else {
      vTaskSuspend(_sensorDataTask);
      log_i("sensorDataTask Suspended");
    }
  }
}

void HCSR04::startMeasure()
{
  if (_sensorDataTask == NULL) {
    if (xTaskCreate(sensorDataTask, "sensorDataTask", 4096, this, 1, &_sensorDataTask) != pdPASS)
      log_w("Failed to create sensorDataTask task");
    else
      log_i("sensorDataTask task created");
  }
  else {
    if (eTaskGetState(_sensorDataTask) == eSuspended) {
      vTaskResume(_sensorDataTask);
      log_i("sensorDataTask resumed");
    }
    else
      log_i("sensorDataTask already started");
  }
}

void HCSR04::init()
{
  pinMode(_trigPin, OUTPUT);
  pinMode(_echoPin, INPUT);
  if (_sensorDataTask == NULL)
    if (xTaskCreate(sensorDataTask, "sensorDataTask", 4096, this, 1, &_sensorDataTask) != pdPASS)
      log_w("Failed to create sensorDataTask task");
    else
      log_i("sensorDataTask task created");
}

/**
 * @brief Task to continuously measure distance using the HCSR04 sensor.
 *
 * This function is intended to be run as a FreeRTOS task. It continuously triggers
 * the HCSR04 ultrasonic sensor to measure distance and calls a user-defined callback
 * function if the measured distance is below a specified threshold.
 *
 * @param pvParameters Pointer to the HCSR04 object instance.
 *
 * The function performs the following steps in an infinite loop:
 * 1. Triggers the ultrasonic sensor by sending a HIGH pulse to the trigger pin.
 * 2. Measures the duration of the echo pulse.
 * 3. Calculates the distance based on the duration of the echo pulse.
 * 4. If the distance is below the threshold and greater than 1 cm, it calls the user-defined callback function.
 * 5. Delays the task for 20 milliseconds before repeating the process.
 */
void HCSR04::sensorDataTask(void* pvParameters)
{
  HCSR04* sensor = static_cast<HCSR04*>(pvParameters);
  while (true) {
    digitalWrite(sensor->_trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(sensor->_trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(sensor->_trigPin, LOW);

    sensor->_duration = pulseIn(sensor->_echoPin, HIGH);
    sensor->_distance = (sensor->_duration * .0343) / 2;

    if (sensor->_distance < sensor->_threshold && sensor->_callback != NULL)
      if (sensor->_distance > 1) {
        // Serial.printf("Distance: %0.2f cm\n", sensor->_distance);
        sensor->_callback();
      }
    vTaskDelay(20 / portTICK_PERIOD_MS);
  }
}

float HCSR04::getDistanceData()
{
  return _distance;
}

float HCSR04::getDurationData()
{
  return _duration;
}