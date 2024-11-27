# Práctica 4: Programación de sondas que usan servicios REST
Este proyecto implementa un sistema de gateway para la gestión, recepción y sincronización de datos de sensores de potencia y estados de LEDs. El sistema está compuesto por tres módulos principales:

1. **Servidor Router** - Recepción y procesamiento de datos.
2. **Sonda en Arduino** - Generación y envío de datos de prueba desde el Arduino ESP32.
3. **Servidor Web** - Volcado de información de la sonda en la web.

Para realizar la conexión mediante SSL se ha hecho un tunel entre un puerto https y el puerto http en el que corre gateway.php. Se han creado los certificados propios y se ha ejecutado stunnel para redireccionar el puerto. 

## Componentes del Proyecto

### Servidor Router
Este componente se encarga de gestionar y procesar la comunicación de datos entre la sonda y el servidor web. Para ello, se han introducido diferentes archivos en el:

1. `Ficheros carpeta agregador`

- `gateway.php`


    Este archivo administra de forma continua todos los datos que recibe y envía. Su función principal es procesar las solicitudes provenientes de la sonda y de la página web, ejecutando las operaciones necesarias en cada caso.

    - Cuando recibe una petición POST desde la sonda, el servidor procesa la información y almacena los datos de potencia junto con la marca de tiempo en un archivo llamado `sensor_data.josn`. 

    - En el caso de una solicitud GET , busca el ID de la sonda en el archivo  `led_status` y devuelve el estado actual del LED correspondiente.

Estos archivos json se mandan como se explicará mas adelante a la base de datos.

- `sync_caches.php`

El código sincroniza datos almacenados localmente con un servidor procesando dos tipos de información. Verifica la existencia de los archivos JSON, lee su contenido, realiza transformaciones específicas si es necesario (como formatear fechas), y los envía al servidor codificados en formato JSON utilizando cURL. Tras recibir la respuesta, actualiza los datos locales según el tipo: modificando valores específicos o eliminando registros exitosamente procesados. Finalmente, guarda los cambios en los archivos locales, asegurando que la información permanezca consistente entre el servidor y los archivos JSON utilizados.

2. `Ficheros carpeta init.d`
- `gatewayServer` : Ejecuta el gateway.php nada más iniciar el router de modo que no es necesario ejecutarlo manualmente
- `syncServer` : Ejecuta la sincronización periódica de los datos y los estados de las sondas
     
    > ⚠️ Es necesario ejecutar el siguiente comando en el router para inicializar la sincronización:
    >
    > ```bash
    > /etc/init.d/syncServer start
    > ```
     
- `stunnel` : Necesario para redireccionar el tráfico del puerto 54472 (https) al puerto donde está corriendo gateway.php 54471 (http)

3. `Ficheros carpeta stunnel`
   
Incluye los archivos que contienen la clave, el certificado y la configuración necesaria para stunnel.

### Sonda Arduino (ESP32)

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


Este código tendrá incluida la implemantación de la conexión SSL con el router. Para ello se ha utilizado WiFiClientSecure y se ha seteado el certificado generado con stunnel. Se ha configurado el cilente con setInsecure ya que es un certificado autofirmado.

### Servidor Web

En la dirección http://webalumnos.tlm.unavarra.es:10303/ se mostrarán, en tiempo real, los datos de potencia capturados por el potenciómetro para cada sonda, así como el estado del LED.
Estos datos se extraen directamente de la base de datos y se presentan en diferentes tablas, que se visualizan según el botón seleccionado. Además, se incluye la opción de eliminar todos los datos de las tablas, lo que también los elimina de la base de datos. Para ello, se han implementado los códigos HTML, CSS y JS necesarios para el diseño de la interfaz web. Además, se han creado varios archivos PHP para gestionar las conexiones entre los dispositivos y la base de datos, permitiendo la actualización y visualización de los datos de manera eficiente. Estos son:

- `manage_data.php`: Gestiona solicitudes HTTP para sincronizar estados de LEDs y registrar datos de sensores en MySQL, validando y devolviendo respuestas en formato JSON según el tipo de solicitud.

- `show_data.php`: Muestra los datos de la tabla POT_SENSOR ordenados por tiempo_muestra en orden descendente .

- `show_leds_data.php`: Recupera los registros de la tabla LED_STATUS, muestra los datos en una tabla HTML y permite actualizar el desired_status a través de un menú desplegable, con un botón para eliminar los datos.

- `update_led_status.php`: Actualiza el desired_status de una sonda en la tabla LED_STATUS si la solicitud proviene de un usuario.

- `delete_pot_sensor.php`: Elimina todos los registros de la tabla POT_SENSOR en MySQL y muestra un mensaje de éxito o error.

- `detele_led_status.php`: Elimina todos los registros de la tabla POT_SENSOR en MySQL y muestra un mensaje de éxito o error.
