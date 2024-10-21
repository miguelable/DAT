# Practica 3

## Configuración del Sistema

### 1. Configuración de la Base de Datos

El script [`setup_database.sh`](Controlador/setup_database.sh) configura una base de datos MySQL para almacenar los datos del sensor.

#### Pasos:

1. Abre una terminal en la carpeta `Controlador`.
2. Ejecuta el script de configuración de la base de datos:
   ```sh
   ./setup_database.sh
   ```
3. Ingresa la contraseña de root de MySQL cuando se te solicite.

### 2. Configuración del Gateway

El script [`gateway_setup.sh`](Agregador/gateway_setup.sh) configura un servidor PHP y ejecuta periódicamente un script para sincronizar cachés.

#### Pasos:

1. Abre una terminal en la carpeta `Agregador`.
2. Asegúrate de que el script tenga permisos de ejecución:
   ```sh
   chmod +x gateway_setup.sh
   ```
3. Ejecuta el script de configuración del gateway:
   ```sh
   ./gateway_setup.sh
   ```

### 3. Ejecución del Sensor

El script [`enviar_datos.sh`](Sensor/enviar_datos.sh) simula la recolección de datos de temperatura y humedad y los envía al gateway.

#### Pasos:

1. Abre una terminal en la carpeta `Sensor`.
2. Asegúrate de que el script tenga permisos de ejecución:
   ```sh
   chmod +x enviar_datos.sh
   ```
3. Ejecuta el script del sensor:
   ```sh
   ./enviar_datos.sh
   ```

## Descripción de Archivos

### Agregador

- [`gateway.php`](Agregador/gateway.php): Maneja solicitudes POST para recibir y almacenar datos JSON.
- [`gateway_setup.sh`](Agregador/gateway_setup.sh): Configura un servidor PHP y ejecuta periódicamente el script `sync_caches.php`.
- [`sync_caches.php`](Agregador/sync_caches.php): Sincroniza datos JSON locales con un servidor remoto.
- `datos_sonda.json`: Archivo JSON donde se almacenan temporalmente los datos recibidos.
- `SyncLog.txt`: Archivo de log donde se registran las ejecuciones del script `sync_caches.php`.

### Controlador

- [`controller.php`](Controlador/controller.php): Configura un socket de servidor para recibir datos JSON y almacenarlos en una base de datos MySQL.
- [`setup_database.sh`](Controlador/setup_database.sh): Configura la base de datos MySQL, el usuario y la tabla para almacenar los datos del sensor.

### Sensor

- [`enviar_datos.sh`](Sensor/enviar_datos.sh): Simula la recolección de datos de temperatura y humedad y los envía al gateway en formato JSON cada 10 segundos.

## Información Útil

- Asegúrate de tener instaladas las dependencias necesarias:
  - `bc` y `curl` para el script del sensor.
  - Extensiones de PHP para Sockets y PDO MySQL.
- Puedes ajustar los detalles de conexión a la base de datos y el puerto del servidor en los scripts correspondientes.
- Los datos se envían al gateway en formato JSON y se almacenan temporalmente en `datos_sonda.json` antes de ser sincronizados con el servidor remoto.

## Ejecución del Sistema

1. Configura la base de datos ejecutando `setup_database.sh`.
2. Configura y ejecuta el gateway con `gateway_setup.sh`.
3. Ejecuta el sensor con `enviar_datos.sh`.

¡Listo! Ahora tu sistema debería estar funcionando correctamente, recolectando datos del sensor, enviándolos al gateway y almacenándolos en la base de datos.

> [!TIP]
>
> Para leer los datos de la base de datos, puedes utilizar el siguiente comando en la terminal:
>
> ```sh
> mysql -u grupo03 -p -e "USE t_h_DataBase; SELECT >* FROM data;"
> ```
>
> Ingresa la contraseña del usuario `grupo03` cuando se te solicite.
