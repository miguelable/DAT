# Practica 3

Este proyecto implementa un sistema de gateway para la gestión, recepción y sincronización de datos de sensores de potencia y estados de LEDs. Está diseñado para ejecutar el procesamiento en un servidor local y sincronizar estos datos periódicamente con un servidor remoto. El sistema incluye scripts en Bash y PHP para el envío, almacenamiento y sincronización de datos, con la autenticación y control necesarios para garantizar la seguridad de la información.

## Componentes del Proyecto

El sistema se compone de tres módulos principales:

1. **Gateway Local** - Recepción y procesamiento de datos.
2. **Sincronización de Datos** - Actualización de datos en un servidor remoto.
3. **Simulador de Datos de Sonda** - Generación y envío de datos de prueba desde un sensor simulado.

Cada módulo se configura y ejecuta mediante los siguientes archivos:

### 1. `gateway.php`

Este script PHP recibe datos de sensores en formato JSON y responde con el estado deseado de un LED. Los datos de sensores y los estados de LEDs se almacenan localmente en `sensor_data.json` y `led_status.json`.

**Funciones principales**:

- **Autenticación**: Verifica el token de autenticación de cada solicitud para asegurar el acceso autorizado.
- **Recepción y Respuesta de Datos**: Guarda el estado actual del sensor y devuelve el `desired_status` de los LEDs.
- **Almacenamiento de Datos**: Almacena los datos de sensores y estados de LEDs en archivos JSON.

**Ejecución**:
Este archivo es iniciado por el servidor PHP configurado en `gateway_setup.sh` y escucha las solicitudes en `https://localhost:8081/gateway.php`.

### 2. `sync_caches.php`

Este script maneja la sincronización de datos con el servidor remoto, actualizando los `desired_status` de los LEDs y eliminando los datos de sensores enviados exitosamente.

**Funciones principales**:

- **Envío de Datos**: Envía datos JSON almacenados en `led_status.json` y `sensor_data.json` al servidor remoto `manage_data.php`.
- **Actualización de Datos**: Actualiza el `desired_status` de los LEDs o elimina datos de sensores enviados correctamente (estado 200).

**Ejecución**:
Este script es ejecutado periódicamente por `gateway_setup.sh` para sincronizar los datos cada 10 segundos y registra los logs en `SyncLog.txt`.

### 3. `gateway_setup.sh`

Este script configura y ejecuta el gateway local, gestionando el servidor PHP y la sincronización de datos.

**Funciones principales**:

- **Inicio del Servidor PHP**: Configura el servidor en `http://localhost:8081`.
- **Configuración de Stunnel**: Inicia `stunnel` para manejar conexiones seguras.
- **Ejecución de Sincronización**: Llama a `sync_caches.php` cada 10 segundos para sincronizar los datos con el servidor remoto.

**Ejecución**:
Ejecuta este script una vez para iniciar y mantener el sistema funcionando:

```bash
cd Agregador
./gateway_setup.sh
```

### 4. `enviar_datos.sh` (Simulador de Sonda)

Este script en Bash simula el envío de datos de potencia aleatorios desde una sonda hacia `gateway.php`. Genera datos de sensores en formato JSON y verifica continuamente si hay cambios en el estado deseado de los LEDs.

**Funciones principales**:

- **Generación de Datos Aleatorios**: Genera valores de potencia aleatorios y una marca temporal para cada envío.
- **Envío de Datos**: Usa `curl` para enviar los datos al servidor local.
- **Cambio de Estado del LED**: Verifica el `desired_status` del LED en cada respuesta del servidor y actualiza el estado del LED si detecta algún cambio.

**Requisitos**

Este script necesita las herramientas `bc`, `curl` y `jq`. Si no están presentes en el sistema, el script intentará instalarlas automáticamente.

**Ejecución**

Para iniciar el simulador, asegúrate de tener permisos de administrador y ejecuta:

```bash
cd Sensor
./enviar_datos.sh
```

## Resumen de Ejecución

Para poner en marcha el sistema completo:

1. Ejecuta `gateway_setup.sh` para iniciar el servidor PHP y la sincronización de datos.
2. En una terminal independiente, ejecuta `enviar_datos.sh` para simular el envío de datos desde una sonda.
