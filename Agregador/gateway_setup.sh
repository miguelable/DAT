# Este script configura un servidor PHP y ejecuta periódicamente un script PHP para sincronizar cachés.
#
# Pasos:
# 1. Determinar el directorio donde se encuentra el script.
# 2. Iniciar un servidor PHP en el puerto 8081, sirviendo archivos desde el directorio determinado.
# 3. Verificar si el script sync_caches.php existe en el directorio.
# 4. Otorgar permisos de ejecución al script sync_caches.php.
# 5. Entrar en un bucle infinito para ejecutar el script sync_caches.php cada 2 minutos.
#
# Variables:
# - BASE_DIR: El directorio donde se encuentra el script.
# - AGREGADOR_PATH: La ruta al directorio desde donde el servidor PHP servirá archivos.
# - SYNC_PATH: La ruta al script sync_caches.php.
# - INTERVALO_MINUTOS: El intervalo en minutos entre cada ejecución del script sync_caches.php.
#
# Registros:
# - La salida del script sync_caches.php se añade a SyncLog.txt en el mismo directorio.

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
INTERVALO_MINUTOS=1

# Bucle infinito para ejecutar el script cada INTERVALO_MINUTOS minutos
while true; do
    echo "Ejecutando sync_caches.php..."
    php "$SYNC_PATH" >> "$BASE_DIR/SyncLog.txt" 2>&1
    echo "Script ejecutado. Esperando $INTERVALO_MINUTOS minutos..."
    sleep $((INTERVALO_MINUTOS * 60))
done