# Documentación de Archivos de Gateway

Este proyecto consiste en tres scripts (`gateway.php`, `sync_caches.php`, `gateway_setup.sh`) que configuran y mantienen un gateway local para el manejo y sincronización de datos de sensores. Estos archivos facilitan la gestión y actualización de los datos de los sensores y estados de LEDs almacenados en el servidor.

## Archivos

### 1. `gateway.php`

Este script recibe datos de sensores en formato JSON y responde con el estado deseado de un LED. Los datos se almacenan localmente y el estado deseado del LED puede modificarse a través de las solicitudes de actualización.

**Principales Funciones:**

- **Autenticación**: Verifica el token de autenticación proporcionado en la solicitud HTTP.
- **Recepción de Datos (`recibirDatos`)**: Maneja las peticiones POST, almacenando el estado actual del sensor y devolviendo el `desired_status`.
- **Almacenamiento de Datos**: Guarda los datos recibidos en `sensor_data.json` y mantiene el estado de los LEDs en `led_status.json`.

**Uso**: Se ejecuta como un servidor PHP y escucha las solicitudes en `https://localhost:8081/gateway.php`.

### 2. `sync_caches.php`

Este script sincroniza los datos locales de estado de los LEDs y los datos de sensores con un servidor remoto, actualizando `desired_status` según sea necesario o eliminando datos enviados exitosamente.

**Principales Funciones:**

- **Envío de Datos**: Envía datos JSON almacenados en `led_status.json` y `sensor_data.json` al servidor remoto (`manage_data.php`).
- **Actualización y Limpieza de Datos**:
  - **Para `ledStatus`**: Actualiza el `desired_status` en `led_status.json` con el valor devuelto por el servidor.
  - **Para `sensorData`**: Elimina datos de `sensor_data.json` que fueron enviados correctamente (estado 200).

**Uso**: Se ejecuta de manera periódica a través de `gateway_setup.sh`.

### 3. `gateway_setup.sh`

Este script configura y ejecuta el gateway, iniciando el servidor local y manejando la sincronización automática de los datos.

**Principales Funciones:**

- **Inicio del Servidor PHP**: Configura un servidor PHP en `http://localhost:8081`.
- **Configuración de Stunnel**: Inicia `stunnel` para habilitar conexiones seguras.
- **Ejecución Periódica de Sincronización**: Ejecuta `sync_caches.php` cada 10 segundos para sincronizar los datos con el servidor remoto y guarda los logs en `SyncLog.txt`.

**Uso**: Se ejecuta una vez para iniciar el gateway y dejar el sistema en funcionamiento.

## Configuración y Ejecución

### Paso 1: Iniciar el Gateway

Para iniciar el gateway y la sincronización automática, ejecuta `gateway_setup.sh`:

```bash
cd Agregador
./gateway_setup.sh
```
