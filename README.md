# Práctica 4: Programación de sondas que usan servicios REST
Este proyecto implementa un sistema de gateway para la gestión, recepción y sincronización de datos de sensores de potencia y estados de LEDs. NOSE COMO EXPLICAR LO DE LA SEGURIDAD 

## Componentes del Proyecto

El sistema se compone de tres módulos principales:

1. **Servidor Router** - Recepción y procesamiento de datos.
2. **Sonda en Arduino** - Generación y envío de datos de prueba desde un Arduino ESP32.
3. **Servidor Web** - Volcado de información de la sonda en la web.

Cada módulo se configura y ejecuta de la siguiente manera:

### En el Servidor Router
Este componente se encarga de gestionar y procesar la comunicación de datos entre la sonda y el servidor web. Para ello, se han introducido diferentes archivos en el:

1. `gateway.php`

Este archivo administra de forma continua todos los datos que recibe y envía. Su función principal es procesar las solicitudes provenientes de la sonda y de la página web, ejecutando las operaciones necesarias en cada caso.

2. `syncServer`

### En la Sonda Arduino

Utilizando un ESP32, configuramos sus pines para leer los valores 
de una entrada analógica (potenciómetro) y controlar una salida 
digital (LED).
El código está diseñado para ser utilizado con múltiples sondas 
simultáneamente. Por ello, incluye configuraciones tanto para un 
LED convencional como para un LED RGB.
Para cambiar entre estas configuraciones, solo es necesario 
comentar o descomentar la línea: `#define LED_RGB `. 

1. `Conexión al Router`

Para realizar el intercambio de información en ambas direcciones, es necesario establecer una conexión con el servidor. Para ello, nos conectamos a la red WiFi proporcionada por nuestro router, a la cual tendremos acceso siempre que el servidor esté en funcionamiento.

2. `Envio de datos`

Configuramos el pin 34 del ESP32 para leer los valores generados por el potenciómetro y calcular el promedio de estos cada 10 segundos. Estos datos se almacenarán en un archivo JSON y se enviarán al router mediante una solicitud POST.

3. `Recibir el estado deseado del LED`

De manera similar, realizamos solicitudes GET para obtener el estado deseado del LED desde el servidor web. Además, el ESP32 enviará su estado actual del LED al servidor.


Este código tendrá incluida la implemantación de NOSE COMO EXPLICAR LO DE LA SEGURIDAD x2

### En el Servidor Web

En la dirección http://webalumnos.tlm.unavarra.es:10303/ se mostrarán, en tiempo real, los datos de potencia capturados por el potenciómetro para cada sonda, así como el estado del LED.
Estos datos se extraen directamente de la base de datos y se presentan en diferentes tablas, que se visualizan según el botón seleccionado. Además, se incluye la opción de eliminar todos los datos de las tablas, lo que también los elimina de la base de datos.