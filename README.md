# DAT

## Descripción del proyecto

Este código para ESP32 permite publicar y suscribirse a topics MQTT a través de una red Wi-Fi segura con conexión TLS/SSL. Específicamente, permite:

1. **Publicar** el valor de un potenciómetro (`potValue`) y el estado de un LED (`ledStatus`) en el _broker_ MQTT.
2. **Suscribirse** a un topic (`desiredStatus`) para recibir comandos que enciendan o apaguen un LED.
3. **Controlar** el color de un LED RGB según el valor del potenciómetro (si se define `LED_RGB`).

La comunicación se realiza a través del puerto **8883** (MQTT seguro).

## Requisitos

### Hardware

- ESP32 con conexión a un potenciómetro y un LED o LED RGB.

### Software

- mosquitto como broker MQTT.
- Cliente MQTT (`mosquitto_pub`, `mosquitto_sub`).
- Librerías Arduino:
  - `WiFi.h`
  - `WiFiClientSecure.h`
  - `PubSubClient.h`
  - `ArduinoJson.h`
  - `NeoPixelBus.h` (si se usa LED RGB)
  - `NTPClient.h`

## Configuración de Red y MQTT

- **Credenciales Wi-Fi**:
  ```cpp
  const char* ssid = "GL-MT300N-V2-0bb";
  const char* password = "goodlife";
  ```
- **Servidor MQTT**:
  ```cpp
  #define MQTTSERVER "192.168.8.1"
  #define MQTTPORT 8883
  ```
- **Certificado TLS**: Se utiliza un certificado para asegurar la conexión. El certificado está definido en ca_Cert.

## Topics MQTT

### Publicar Datos

1. **Valor del Potenciómetro** (`potValue`):
   - **Topic**: `grupo01/4636/potValue`
   - **Ejemplo de Mensaje**:
     ```json
     {
       "id_sonda": 4636,
       "potencia": 1024,
       "timestamp": 1713456789
     }
     ```
2. **Estado del LED** (`ledStatus`):
   - **Topic**: `grupo01/4636/ledStatus`
   - **Ejemplo de Mensaje**:
     ```json
     {
       "id_sonda": 4636,
       "led_status": "1"
     }
     ```

### Suscribirse a Comandos

- **Comando para Encender o Apagar el LED** (`desiredStatus`):
  - **Topic**: `grupo01/4636/desiredStatus`
  - **Ejemplo de Mensaje**:
    ```json
    {
      "desired_status": 1
    }
    ```

## Estructura del Código

1. **Conexión Wi-Fi y MQTT**:
   - Se conecta a la red Wi-Fi y al _broker_ MQTT con TLS.
2. **Tareas FreeRTOS**:
   - `readPotTask`: Lee el valor del potenciómetro y actualiza el LED.
   - `updateTimeTask`: Sincroniza la hora usando NTP.
   - `sendDataTask`: Publica los datos del potenciómetro y del LED en el _broker_.
3. **Callback MQTT**:
   - Escucha los mensajes del _topic_ `desiredStatus` y cambia el estado del LED en consecuencia.

## Compilación y Ejecución

> [!IMPORTANT]
> Se ha probado en ESP32 con Arduino IDE y PlatformIO.

1. **Cargar el código al ESP32** utilizando Arduino IDE o PlatformIO.
2. **Abrir el Monitor Serie** para visualizar la conexión y los mensajes MQTT.
3. **Interactuar con el ESP32**:
   - **Suscribirse a Comandos**: Utilizar `mosquitto_sub` para recibir mensajes del _topic_ `desiredStatus`.
   - **Monitor serie** enviando comandos para encender o apagar el LED.
     - `1` para encender el LED.
     - `0` para apagar el LED.

# Guía para Suscribirse y Publicar en los Topics

## Desde el Broker MQTT (Router)

### Suscribirse a Topics

Ejecuta los siguientes comandos en el terminal del broker para suscribirte a los topics:

1. **Suscribirse a `potValue`**:
   ```bash
   mosquitto_sub -h localhost -p 8883 --cafile /etc/mosquitto/certs/mqtt-server.crt --insecure -t grupo01/4636/potValue
   ```
2. **Suscribirse a `ledStatus`**:
   ```bash
   mosquitto_sub -h localhost -p 8883 --cafile /etc/mosquitto/certs/mqtt-server.crt --insecure -t grupo01/4636/ledStatus
   ```

### Publicar en el Topic `desiredStatus`

1. **Encender el LED**:
   ```bash
   mosquitto_pub -h localhost -p 8883 --cafile /etc/mosquitto/certs/mqtt-server.crt -t grupo01/4636/desiredStatus -m '{"desired_status":1}'
   ```
2. **Apagar el LED**:
   ```bash
   mosquitto_pub -h localhost -p 8883 --cafile /etc/mosquitto/certs/mqtt-server.crt -t grupo01/4636/desiredStatus -m '{"desired_status":0}'
   ```

## Desde otro Dispositivo

### Suscribirse a Topics

1. **Suscribirse a `potValue`**:
   ```bash
   mosquitto_sub -h webalumnos.tlm.unavarra.es -p 10320 -t grupo01/4636/potValue
   ```
2. **Suscribirse a `ledStatus`**:
   ```bash
   mosquitto_sub -h webalumnos.tlm.unavarra.es -p 10320 -t grupo01/4636/ledStatus
   ```

### Publicar en el Topic `desiredStatus`

1. **Encender el LED**:
   ```bash
   mosquitto_pub -h webalumnos.tlm.unavarra.es -p 10320 -t grupo01/4636/desiredStatus -m '{"desired_status":1}'
   ```
2. **Apagar el LED**:
   ```bash
   mosquitto_pub -h webalumnos.tlm.unavarra.es -p 10320 -t grupo01/4636/desiredStatus -m '{"desired_status":0}'
   ```

## Notas Adicionales

- Para que el código se ejecute correctamente, asegúrate de que el broker MQTT esté en ejecución y que el ESP32 esté conectado a la red Wi-Fi.
- Si se usa un LED RGB, asegúrate de que la librería `NeoPixelBus.h` esté instalada.
- Si se desea cambiar el _broker_ MQTT, modifica la dirección IP y el puerto en el código.
- Si se desea cambiar la red Wi-Fi, modifica el SSID y la contraseña en el código.
- Si se desea cambiar el certificado TLS, modifica el certificado en el código.
- **Parámetro** `--insecure`: Se usa en los comandos `mosquitto_sub` y `mosquitto_pub` para evitar la verificación del nombre común del certificado. Se puede quitar si el certificado es válido.
- **Puerto 10320**: Se ha utilizado para la conexión con el broker MQTT en la nube de la Universidad Pública de Navarra (webalumnos.tlm.unavarra.es).

## Autores

- Paula Fernández
- Miguel Ferrer
