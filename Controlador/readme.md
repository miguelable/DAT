# Documentación de Archivos PHP

Este proyecto consiste en varios scripts PHP que manejan la configuración y sincronización de datos de sensores y estados de LEDs. A continuación se describe brevemente la funcionalidad de cada archivo.

## Archivos

### 1. `update_led_status.php`

Este script se encarga de actualizar el estado deseado de un LED en la base de datos.

**Principales Funciones:**

- **Conexión a la Base de Datos**: Establece una conexión con la base de datos utilizando PDO.
- **Actualización del Estado Deseado**: Actualiza el campo `desired_status` en la tabla `LED_STATUS` para una sonda específica, siempre y cuando la fuente de la actualización sea el usuario.

### 2. `show_leds_data.php`

Este script muestra los datos de la tabla `LED_STATUS` en una página HTML.

**Principales Funciones:**

- **Conexión a la Base de Datos**: Establece una conexión con la base de datos utilizando PDO.
- **Consulta de Datos**: Obtiene todos los registros de la tabla `LED_STATUS`.
- **Visualización de Datos**: Muestra los datos en una tabla HTML, permitiendo la selección del estado deseado para cada sonda.

### 3. `show_data.php`

Este script muestra los datos de la tabla `POT_SENSOR` en una página HTML.

**Principales Funciones:**

- **Conexión a la Base de Datos**: Establece una conexión con la base de datos utilizando PDO.
- **Consulta de Datos**: Obtiene todos los registros de la tabla `POT_SENSOR`.
- **Visualización de Datos**: Muestra los datos en una tabla HTML.

### 4. `manage_data.php`

Este script maneja las solicitudes para actualizar y obtener datos de las tablas `LED_STATUS` y `POT_SENSOR`.

**Principales Funciones:**

- **Conexión a la Base de Datos**: Establece una conexión con la base de datos utilizando PDO.
- **Manejo de `ledStatus`**: Procesa los datos relacionados con el estado de los LEDs, actualizando o creando registros en la tabla `LED_STATUS`.
- **Manejo de `sensorData`**: Procesa los datos de los sensores, insertando registros en la tabla `POT_SENSOR`.
- **Procesamiento de Solicitudes**: Determina el tipo de solicitud (`ledStatus` o `sensorData`) y llama a la función correspondiente para manejar los datos.
- **Respuesta JSON**: Devuelve una respuesta en formato JSON basada en el resultado del procesamiento de la solicitud.
