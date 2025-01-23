/**
 * @file hcsr04.cpp
 * @author Miguel Ferrer
 * @brief Implementación de la clase HCSR04
 * @version 0.1
 * @date 2025-01-14
 *
 * Este archivo contiene la implementación de la clase HCSR04, que proporciona funciones para gestionar el sensor
 * ultrasónico HC-SR04. La clase incluye funciones para inicializar el sensor, iniciar y detener las mediciones, y
 * recuperar datos de distancia y duración. La clase también soporta la configuración de una función de callback para
 * activar una acción cuando la distancia supera un umbral especificado.
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
    if (xTaskCreate(sensorDataTask, "sensorDataTask", 81, this, 1, &_sensorDataTask) != pdPASS)
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
    if (xTaskCreate(sensorDataTask, "sensorDataTask", 8192, this, 1, &_sensorDataTask) != pdPASS)
      log_w("Failed to create sensorDataTask task");
    else
      log_i("sensorDataTask task created");
}

/**
 * @brief Tarea para medir continuamente la distancia utilizando el sensor HCSR04.
 *
 * Esta función está destinada a ejecutarse como una tarea de FreeRTOS. Continuamente activa
 * el sensor ultrasónico HCSR04 para medir la distancia y llama a una función de callback definida por el usuario
 * si la distancia medida está por debajo de un umbral especificado.
 *
 * @param pvParameters Puntero a la instancia del objeto HCSR04.
 *
 * La función realiza los siguientes pasos en un bucle infinito:
 * 1. Activa el sensor ultrasónico enviando un pulso HIGH al pin de disparo.
 * 2. Mide la duración del pulso de eco.
 * 3. Calcula la distancia basada en la duración del pulso de eco.
 * 4. Si la distancia está por debajo del umbral y es mayor de 1 cm, llama a la función de callback definida por el
 * usuario.
 * 5. Retrasa la tarea durante 20 milisegundos antes de repetir el proceso.
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
      if (sensor->_distance > 5) {
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