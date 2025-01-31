# TecnoPark

Repositorio para el desarrollo de la aplicación de TecnoPark.

### Descripción del Proyecto

El proyecto TecnoPark está diseñado para gestionar un sistema de parque temático utilizando un ESP32. Este sistema incluye la gestión de una cámara ESP32-CAM, efectos LED, un sensor ultrasónico HC-SR04 y la comunicación mediante MQTT y WebSockets. El objetivo es proporcionar una solución integrada para la supervisión y control de diferentes atracciones en el parque.

### Estructura del Proyecto

```
TecnoPark
├── .vscode/
│   ├── c_cpp_properties.json
│   ├── extensions.json
│   ├── launch.json
│   └── settings.json
├── docs/
│   ├── annotated_dup.js
│   ├── annotated.html
│   ├── camConfig_8h_source.html
│   ├── camConfig_8h.html
│   ├── camConfig_8h.js
│   ├── classes.html
│   ├── classESP32Cam.html
│   ├── classESP32Cam.js
│   ├── classHCSR04.html
│   ├── classHCSR04.js
│   ├── clipboard.js
│   ├── cookie.js
│   ├── dir_43b07ced696ca9c2ff3ac6292e4a39b2.html
│   ├── dir_43b07ced696ca9c2ff3ac6292e4a39b2.js
│   ├── ...
│   ├── Doxyfile
├── include/
│   ├── README
├── lib/
│   ├── ESP32Cam/
│   │   ├── esp32cam.cpp
│   │   └── esp32cam.h
│   ├── HCSR04/
│   │   ├── hcsr04.cpp
│   │   └── hcsr04.h
│   ├── LEDRGB/
│   │   ├── LedEffects.cpp
│   │   └── LedEffects.h
│   ├── Mqtt/
│   │   ├── mqttClient.cpp
│   │   └── mqttClient.h
│   ├── websocket/
│   │   ├── websockets.cpp
│   │   └── websockets.h
│   ├── Private/
│   │   └── secrets.h
│   ├── others/
│   │   ├── ESP32WebSockets.cpp.txt
│   │   └── main.cpp.txt
├── src/
│   ├── main.cpp
├── test/
├── .clang-format
├── .gitignore
├── platformio.ini
└── README.md
├── web/
│   ├── css/
│   ├── images/
│   ├── js/
│   ├── php/
│   ├── videos/
│   ├── admin.html
│   ├── apache2.conf
│   ├── caribeaquatic.html
│   ├── china.html
│   ├── codigo.html
│   ├── compraentradas.html
│   ├── farwest.html
│   ├── ferrariland.html
│   ├── hoteles.html
│   ├── index.html
│   ├── login.html
│   ├── mapa.html
│   ├── mediterrania.html
│   ├── mexico.html
│   ├── parques.html
│   ├── polynesia.html
│   ├── portaventura.html
│   ├── register.html
│   └── sesamoaventura.html
```

### Componentes del Firmware

1. main.cpp
   Este archivo contiene las funciones principales de configuración y bucle para el proyecto TecnoPark. Inicializa el sensor ultrasónico HC-SR04, configura los efectos LED y maneja la pulsación del botón para alternar entre diferentes efectos LED.

2. esp32cam.h y esp32cam.cpp
   Estos archivos contienen la declaración y la implementación de la clase ESP32Cam, que proporciona funciones para gestionar el módulo de cámara ESP32-CAM. Las funciones incluyen inicialización, captura de imágenes y recuperación de configuración.

3. hcsr04.h y hcsr04.cpp
   Estos archivos contienen la declaración y la implementación de la clase HCSR04, que proporciona funciones para gestionar el sensor ultrasónico HC-SR04. La clase incluye funciones para inicializar el sensor, iniciar y detener las mediciones, y recuperar datos de distancia y duración.

4. LedEffects.h y LedEffects.cpp
   Estos archivos contienen la declaración y la implementación de la biblioteca LedEffects, que proporciona funciones para gestionar efectos LED en una tira LED RGB. La biblioteca utiliza las bibliotecas NeoPixelBus y NeoPixelAnimator para controlar la tira LED.

5. mqttClient.h y mqttClient.cpp
   Estos archivos contienen la declaración y la implementación de la clase MqttClient, que proporciona funciones para conectarse a un broker MQTT. La clase utiliza las bibliotecas PubSubClient y WiFiClientSecure para establecer una conexión segura con el broker MQTT.

6. websockets.h y websockets.cpp
   Estos archivos contienen la declaración y la implementación de la biblioteca para WebSocket y cámara ESP32. La biblioteca proporciona funciones para conectarse a una red WiFi, establecer una conexión segura con el servidor y enviar imágenes capturadas desde la cámara al servidor.

7. secrets.h
   Este archivo contiene credenciales secretas y configuraciones de seguridad que no deben compartirse públicamente. Incluye credenciales de WiFi, configuraciones del servidor NTP y certificados del servidor para comunicación segura.

### Diagrama de Dependencias

```
main.cpp
├── LedEffects.h
├── hcsr04.h
├── mqttClient.h
└── websockets.h

esp32cam.cpp
└── esp32cam.h

hcsr04.cpp
└── hcsr04.h

LedEffects.cpp
└── LedEffects.h

mqttClient.cpp
├── mqttClient.h
├── LedEffects.h
└── websockets.h

websockets.cpp
├── websockets.h
├── LedEffects.h
└── secrets.h
```

### Funcionamiento General

1. **Inicialización**: En el archivo main.cpp, se inicializan los componentes de hardware y red, incluyendo la cámara, el sensor ultrasónico, los efectos LED y la conexión WiFi y MQTT.
2. **Captura de Imágenes**: La clase ESP32Cam gestiona la captura de imágenes utilizando el módulo de cámara ESP32-CAM.
3. **Medición de Distancia**: La clase HCSR04 gestiona el sensor ultrasónico para medir la distancia y activar una función de callback cuando se supera un umbral especificado.
4. **Efectos LED**: La biblioteca LedEffects gestiona los efectos LED en una tira LED RGB.
5. **Comunicación MQTT**: La clase MqttClient gestiona la conexión y comunicación con un broker MQTT para enviar y recibir mensajes.
6. **Comunicación WebSocket**: La biblioteca websockets gestiona la conexión y comunicación segura con un servidor mediante WebSockets.

### Licencia

Este proyecto está licenciado bajo los términos de la licencia MIT. Para más detalles, consulta el archivo LICENSE.

Este README proporciona una visión general del proyecto TecnoPark y explica cada uno de sus componentes de manera sencilla y visual. Si tienes alguna pregunta o necesitas más información, no dudes en contactarnos.

### Autores

- Miguel Ferrer Ferrández
- Paula Fernández Torres
