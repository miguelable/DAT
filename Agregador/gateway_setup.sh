#!/bin/bash

# Obtener la ruta del directorio donde se encuentra cron_setup.sh
BASE_DIR=$(dirname "$(realpath "$0")")

# Ejecutar php -S 0.0.0.0:8081 en la ruta "Agregador/"
# para que el gateway esté disponible en http://localhost:8081/gateway.php
AGREGADOR_PATH="$BASE_DIR"
php -S 0.0.0.0:8081 -t "$AGREGADOR_PATH" &> /dev/null &
if [ $? -ne 0 ]; then
    echo "Error al iniciar el servidor PHP"
    exit 1
fi

# Dar permisos de ejecución a los scripts
chmod 600 "$BASE_DIR/stunnel.conf"
chmod 600 "$BASE_DIR/fullchain.pem"

# Iniciar stunnel con el archivo de configuración
sudo stunnel "$BASE_DIR/stunnel.conf" & > /dev/null
if [ $? -ne 0 ]; then
    echo "Error al iniciar stunnel"
    exit 1
fi

# Ruta al script del sensor
SYNC_PATH="$BASE_DIR/sync_caches.php"

# Comprobar si existe el fichero de sincronización
if [[ ! -f "$SYNC_PATH" ]]; then
    echo "El script no existe en la ruta: $SYNC_PATH"
    exit 1
fi

# Dar permisos de ejecución a los scripts
chmod +x "$SYNC_PATH"

# Intervalo en minutos
INTERVALO_SEGUNDOS=10

# Bucle infinito para ejecutar el script cada INTERVALO_SGUNDOS segundos
while true; do
    echo "Ejecutando sync_caches.php..."
    php "$SYNC_PATH" >> "$BASE_DIR/SyncLog.txt" 2>&1
    echo "Script ejecutado. Esperando $INTERVALO_SEGUNDOS segundos..."
    sleep $((INTERVALO_SEGUNDOS))
done