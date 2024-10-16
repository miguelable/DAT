#!/bin/bash

# Obtener la ruta del directorio donde se encuentra cron_setup.sh
BASE_DIR=$(dirname "$(realpath "$0")")

# Ejecutar php -S 0.0.0.0:8081 en la ruta "Agregador/"
# para que el gateway esté disponible en http://localhost:8081/gateway.php
AGREGADOR_PATH="$BASE_DIR/Agregador"
php -S 0.0.0.0:8081 -t "$AGREGADOR_PATH" &

echo "Gateway iniciado en http://localhost:8081/gateway.php"

# Ejecutar el controller
CONTROLLER_PATH="$BASE_DIR/Controlador/controller.php"
php "$CONTROLLER_PATH" & 

# Ruta al script del sensor
SENSOR_PATH="$BASE_DIR/Sensor/enviar_datos.sh"
SYNC_PATH="$BASE_DIR/Agregador/sync_caches.php"

# Comprobar si el script del sensor existe
if [[ ! -f "$SENSOR_PATH" ]]; then
    echo "El script no existe en la ruta: $SENSOR_PATH"
    exit 1
fi

# Comprobar si existe el fichero de sincronización
if [[ ! -f "$SYNC_PATH" ]]; then
    echo "El script no existe en la ruta: $SYNC_PATH"
    exit 1
fi

# Dar permisos de ejecución a los scripts
chmod +x "$SENSOR_PATH"
chmod +x "$SYNC_PATH"

# Primer cron para enviar datos al agregador cada minuto
CRON_SENSOR="* * * * * $SENSOR_PATH >> $BASE_DIR/Sensor/Sensorlog.txt 2>&1"
# Segundo cron para sincronizar caches cada 2 minutos (ejecutar con php)
CRON_SYNC="*/2 * * * * php $SYNC_PATH >> $BASE_DIR/Agregador/SyncLog.txt 2>&1"

# Añadir el cron job solo si no existe
(crontab -l | grep -q "$CRON_SENSOR") || (crontab -l; echo "$CRON_SENSOR") | crontab -
(crontab -l | grep -q "$CRON_SYNC") || (crontab -l; echo "$CRON_SYNC") | crontab -

echo "Cron Sensor añadido: $CRON_SENSOR"
echo "Cron Sync añadido: $CRON_SYNC"

# Comprobar si se han creado correctamente
if crontab -l | grep -q "$CRON_SENSOR"; then
    echo "Cron Sensor creado correctamente"
else
    echo "Error al crear el cron Sensor"
    # Eliminar los crons y cerrar puertos
    crontab -l | grep -v "$CRON_SENSOR" | crontab -
    crontab -l | grep -v "$CRON_SYNC" | crontab -
    echo "Cron Sensor eliminado"
    echo "Cron Sync eliminado"
    fuser -k 8080/tcp
    fuser -k 8081/tcp
    exit 1
fi

if crontab -l | grep -q "$CRON_SYNC"; then
    echo "Cron Sync creado correctamente"
else
    echo "Error al crear el cron Sync"
    # Eliminar los crons y cerrar puertos
    crontab -l | grep -v "$CRON_SENSOR" | crontab -
    crontab -l | grep -v "$CRON_SYNC" | crontab -
    echo "Cron Sensor eliminado"
    echo "Cron Sync eliminado"
    fuser -k 8080/tcp
    fuser -k 8081/tcp
    exit 1
fi

# Esperar a que el usuario presione enter para eliminar los crons
read -p "Presione enter para eliminar los crons y salir..."
# Eliminar los crons
crontab -l | grep -v "$CRON_SENSOR" | crontab -
crontab -l | grep -v "$CRON_SYNC" | crontab -
echo "Cron Sensor eliminado"
echo "Cron Sync eliminado"
exit 0
