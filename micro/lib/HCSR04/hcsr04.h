/**
 * @file hcsr04.h
 * @author Miguel Ferrer
 * @brief Biblioteca para gestionar el sensor ultrasónico HC-SR04.
 * @version 0.1
 * @date 2025-01-14
 *
 * Este archivo contiene la declaración de la clase HCSR04, que proporciona funciones para gestionar el sensor
 * ultrasónico HC-SR04. La clase incluye funciones para inicializar el sensor, iniciar y detener las mediciones, y
 * recuperar datos de distancia y duración. La clase también admite la configuración de una función de callback para
 * activar una acción cuando la distancia supera un umbral especificado.
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef HCSR04_H
#define HCSR04_H

#define DEFAULT_TRIG 14 /*!< Pin GPIO predeterminado para el pin de disparo. */
#define DEFAULT_ECHO 15 /*!< Pin GPIO predeterminado para el pin de eco. */

#include <Arduino.h>
#include <freertos/task.h>

typedef void (*ActivationCallback)(); /*!< Tipo de puntero a función para el callback de activación. */

/**
 * @brief Clase para gestionar el sensor ultrasónico HC-SR04.
 *
 * Esta clase proporciona funciones para inicializar el sensor, iniciar y detener las mediciones, y recuperar datos de
 * distancia y duración. La clase también admite la configuración de una función de callback para activar una acción
 * cuando la distancia supera un umbral especificado. Los datos del sensor se leen en una tarea de FreeRTOS para
 * garantizar una operación no bloqueante.
 *
 */
class HCSR04
{
public:
  /**
   * @brief Constructor para la clase HCSR04.
   *
   * Este constructor inicializa el sensor ultrasónico HCSR04 con los pines de disparo y eco especificados.
   *
   * @param trigPin El número de pin conectado al pin de disparo del sensor HCSR04.
   * @param echoPin El número de pin conectado al pin de eco del sensor HCSR04.
   */
  HCSR04(uint8_t trigPin, uint8_t echoPin);

  /**
   * @brief Constructor predeterminado para la clase HCSR04.
   *
   * Este constructor inicializa el objeto HCSR04 con los pines de disparo y eco predeterminados.
   * Los valores predeterminados para los pines de disparo y eco están definidos por las constantes
   * DEFAULT_TRIG y DEFAULT_ECHO, respectivamente.
   */
  HCSR04();

  /**
   * @brief Destructor para la clase HCSR04.
   *
   * Este destructor asegura que cualquier tarea en ejecución asociada con el objeto HCSR04
   * se elimine correctamente. Si la tarea está actualmente suspendida, se eliminará directamente.
   * De lo contrario, la tarea se suspenderá primero y luego se eliminará. Después de la eliminación,
   * el manejador de la tarea se establece en NULL.
   *
   * Además, se genera un mensaje de registro para indicar que el objeto HCSR04 ha sido eliminado.
   */
  ~HCSR04();

  /**
   * @brief Inicializa el sensor HCSR04 configurando los pines de disparo y eco.
   *
   * Esta función configura el pin de disparo como salida y el pin de eco como entrada.
   * También crea una tarea de FreeRTOS para manejar los datos del sensor si aún no se ha creado.
   *
   * @note Si la creación de la tarea falla, se registra un mensaje de advertencia. Si tiene éxito, se registra un
   * mensaje de información.
   */
  void init();

  /**
   * @brief Establece la función de callback que se llamará cuando la medición de distancia supere el umbral
   * especificado.
   *
   * @param threshold El umbral de distancia en unidades de medida (por ejemplo, centímetros).
   * @param callback La función que se llamará cuando se supere el umbral. Esta función debe coincidir con el tipo
   * ActivationCallback.
   */
  void setCallback(uint16_t threshold, ActivationCallback callback);

  /**
   * @brief Detiene la tarea de medición para el sensor HCSR04.
   *
   * Esta función suspende la tarea responsable de manejar los datos del sensor si está actualmente en ejecución.
   * Si la tarea ya está suspendida, se registra un mensaje de advertencia.
   *
   * @note Esta función verifica que la tarea no sea NULL antes de intentar suspenderla.
   */
  void stopMeasure();

  /**
   * @brief Inicia el proceso de medición para el sensor HCSR04.
   *
   * Esta función inicia la tarea de datos del sensor si aún no se ha creado.
   * Si la tarea se crea con éxito, se genera un mensaje de registro que indica la creación de la tarea.
   * Si la creación de la tarea falla, se genera un mensaje de advertencia en el registro.
   * Si la tarea ya está creada y está en un estado suspendido, se reanuda la tarea
   * y se registra un mensaje que indica la reanudación de la tarea. Si la tarea ya está en ejecución,
   * se registra un mensaje que indica que la tarea ya está iniciada.
   */
  void startMeasure();

  /**
   * @brief Recupera los datos de distancia medidos por el sensor HCSR04.
   *
   * @return float La distancia medida por el sensor en centímetros.
   */
  float getDistanceData();

  /**
   * @brief Recupera los datos de duración medidos por el sensor HCSR04.
   *
   * @return float La duración del pulso de eco en microsegundos.
   */
  float getDurationData();

private:
  // Tarea para leer las medidas
  static void  sensorDataTask(void* pvParameters);
  TaskHandle_t _sensorDataTask = NULL; /*!< Manejador de tarea para la tarea de datos del sensor. */

  float              _threshold = 0; /*!< Umbral de distancia para el callback de activación. */
  float              _distance  = 0; /*!< Datos de distancia del sensor. */
  float              _duration  = 0; /*!< Datos de duración del sensor. */
  uint8_t            _trigPin;       /*!< Pin GPIO para el pin de disparo. */
  uint8_t            _echoPin;       /*!< Pin GPIO para el pin de eco. */
  ActivationCallback _callback;      /*!< Función de callback de activación. */
};

#endif // HCSR04_H