/**
 * @file hcsr04.h
 * @author Miguel Ferrer (mferrer@inbiot.es)
 * @brief Library for managing the HC-SR04 ultrasonic sensor.
 * @version 0.1
 * @date 2025-01-14
 *
 * This file contains the declaration of the HCSR04 class, which provides functions to manage the HC-SR04 ultrasonic
 * sensor. The class includes functions to initialize the sensor, start and stop measurements, and retrieve distance and
 * duration data. The class also supports setting a callback function to trigger an action when the distance exceeds a
 * specified threshold.
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef HCSR04_H
#define HCSR04_H

#define DEFAULT_TRIG 14 /*!< Default GPIO pin for the trigger pin. */
#define DEFAULT_ECHO 15 /*!< Default GPIO pin for the echo pin. */

#include <Arduino.h>
#include <freertos/task.h>

typedef void (*ActivationCallback)(); /*!< Function pointer type for the activation callback. */

// Class for the hc_sr04
/**
 * @brief Class for managing the HC-SR04 ultrasonic sensor.
 *
 * This class provides functions to initialize the sensor, start and stop measurements, and retrieve distance and
 * duration data. The class also supports setting a callback function to trigger an action when the distance exceeds a
 * specified threshold. The sensor data is read in a FreeRTOS task to ensure non-blocking operation.
 *
 */
class HCSR04
{
public:
  /**
   * @brief Constructor for the HCSR04 class.
   *
   * This initializes the HCSR04 ultrasonic sensor with the specified trigger and echo pins.
   *
   * @param trigPin The pin number connected to the trigger pin of the HCSR04 sensor.
   * @param echoPin The pin number connected to the echo pin of the HCSR04 sensor.
   */
  HCSR04(uint8_t trigPin, uint8_t echoPin);

  /**
   * @brief Default constructor for the HCSR04 class.
   *
   * This constructor initializes the HCSR04 object with default trigger and echo pins.
   * The default values for the trigger and echo pins are defined by the constants
   * DEFAULT_TRIG and DEFAULT_ECHO, respectively.
   */

  HCSR04();
  /**
   * @brief Destructor for the HCSR04 class.
   *
   * This destructor ensures that any running tasks associated with the HCSR04 object
   * are properly deleted. If the task is currently suspended, it will be deleted directly.
   * Otherwise, the task will first be suspended and then deleted. After deletion, the
   * task handle is set to NULL.
   *
   * Additionally, a log message is generated to indicate that the HCSR04 object has been deleted.
   */
  ~HCSR04();

  /**
   * @brief Initializes the HCSR04 sensor by setting up the trigger and echo pins.
   *
   * This function configures the trigger pin as an output and the echo pin as an input.
   * It also creates a FreeRTOS task to handle sensor data if it has not been created yet.
   *
   * @note If the task creation fails, a warning message is logged. If successful, an info message is logged.
   */
  void init();

  /**
   * @brief Sets the callback function to be called when the distance measurement exceeds the specified threshold.
   *
   * @param threshold The distance threshold in units of measurement (e.g., centimeters).
   * @param callback The function to be called when the threshold is exceeded. This function should match the
   * ActivationCallback type.
   */
  void setCallback(uint16_t threshold, ActivationCallback callback);

  /**
   * @brief Stops the measurement task for the HCSR04 sensor.
   *
   * This function suspends the task responsible for handling sensor data if it is currently running.
   * If the task is already suspended, a warning message is logged.
   *
   * @note This function checks if the task is not NULL before attempting to suspend it.
   */
  void stopMeasure();

  /**
   * @brief Starts the measurement process for the HCSR04 sensor.
   *
   * This function initiates the sensor data task if it is not already created.
   * If the task is successfully created, a log message is generated indicating
   * the task creation. If the task creation fails, a warning log message is generated.
   * If the task is already created and is in a suspended state, it resumes the task
   * and logs a message indicating the task resumption. If the task is already running,
   * it logs a message indicating that the task is already started.
   */
  void startMeasure();

  /**
   * @brief Retrieves the distance data measured by the HCSR04 sensor.
   *
   * @return float The distance measured by the sensor in centimeters.
   */
  float getDistanceData();

  /**
   * @brief Retrieves the duration data measured by the HCSR04 sensor.
   *
   * @return float The duration of the echo pulse in microseconds.
   */
  float getDurationData();

private:
  // Task to read measures
  static void  sensorDataTask(void* pvParameters);
  TaskHandle_t _sensorDataTask = NULL; /*!< Task handle for the sensor data task. */

  float              _threshold = 0; /*!< Distance threshold for the activation callback. */
  float              _distance  = 0; /*!< Distance data from the sensor. */
  float              _duration  = 0; /*!< Duration data from the sensor. */
  uint8_t            _trigPin;       /*!< GPIO pin for the trigger pin. */
  uint8_t            _echoPin;       /*!< GPIO pin for the echo pin. */
  ActivationCallback _callback;      /*!< Activation callback function. */
};

#endif // HCSR04_H