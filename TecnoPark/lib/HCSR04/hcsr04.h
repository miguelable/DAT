#ifndef HCSR04_H
#define HCSR04_H

#define DEFAULT_TRIG 14
#define DEFAULT_ECHO 15

#include <Arduino.h>
#include <freertos/task.h>

typedef void (*ActivationCallback)();

// Class for the hc_sr04
class HCSR04
{
public:
  // Constructor & Destructor
  HCSR04(uint8_t trigPin, uint8_t echoPin);
  HCSR04();
  ~HCSR04();

  void init();
  void setCallback(uint16_t threshold, ActivationCallback callback);
  void stopMeasure();
  void startMeasure();

  float getDistanceData();
  float getDurationData();

private:
  // Task to read measures
  static void  sensorDataTask(void* pvParameters);
  TaskHandle_t _sensorDataTask = NULL;

  float              _threshold;
  float              _distance;
  float              _duration;
  uint8_t            _trigPin;
  uint8_t            _echoPin;
  ActivationCallback _callback;
};

#endif // HCSR04_H