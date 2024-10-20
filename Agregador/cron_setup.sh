#!/bin/bash

# Obtener la ruta del directorio donde se encuentra cron_setup.sh
BASE_DIR=$(dirname "$(realpath "$0")")

# Ejecutar php -S 0.0.0.0:8081 en la ruta "Agregador/"
# para que el gateway esté disponible en http://localhost:8081/gateway.php
AGREGADOR_PATH="$BASE_DIR"
php -S 0.0.0.0:8081 -t "$AGREGADOR_PATH" &

# Ruta al script del sensor
SYNC_PATH="$BASE_DIR/sync_caches.php"

# Comprobar si existe el fichero de sincronización
if [[ ! -f "$SYNC_PATH" ]]; then
    echo "El script no existe en la ruta: $SYNC_PATH"
    exit 1
fi

# Dar permisos de ejecución a los scripts
chmod +x "$SYNC_PATH"

# Segundo cron para sincronizar caches cada 2 minutos (ejecutar con php)
CRON_SYNC="*/2 * * * * php $SYNC_PATH >> $BASE_DIR/SyncLog.txt 2>&1"

# Añadir el cron job solo si no existe
(crontab -l | grep -q "$CRON_SYNC") || (crontab -l; echo "$CRON_SYNC") | crontab -

echo "Cron Sync añadido: $CRON_SYNC"

if crontab -l | grep -q "$CRON_SYNC"; then
    echo "Cron Sync creado correctamente"
else
    echo "Error al crear el cron Sync"
    # Eliminar el cron y cerrar puertos
    crontab -l | grep -v "$CRON_SYNC" | crontab -
    echo "Cron Sync eliminado"
    fuser -k 8080/tcp
    fuser -k 8081/tcp
    exit 1
fi

# Esperar a que el usuario presione enter para eliminar los crons
read -p "Presione enter para eliminar el cron y salir..."
# Eliminar los crons
crontab -l | grep -v "$CRON_SYNC" | crontab -
echo "Cron Sync eliminado"
exit 0
