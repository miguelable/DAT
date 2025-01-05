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