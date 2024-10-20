#!/bin/bash

# Función para verificar si un comando está disponible
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Verificar e instalar dependencias si es necesario
if ! command_exists bc; then
    echo "Instalando bc..."
    sudo apt-get update
    sudo apt-get install -y bc
fi

if ! command_exists curl; then
    echo "Instalando curl..."
    sudo apt-get update
    sudo apt-get install -y curl
fi

if ! command_exists jq; then
    echo "Instalando jq..."
    sudo apt-get update
    sudo apt-get install -y jq
fi

ID=48

# Enviar de forma periódica datos de temperatura aleatorios entre 0 y 50 cada 10 segundos con decimal
# Se envían al gateway en formato JSON

echo "Simulando envio de datos cada 10 segundos"

# Datos de la sonda
while true
do
    # Generar temperatura aleatoria entre 0 y 50 con un decimal
    TEMPERATURA=$(printf "%.1f" "$(echo "scale=1; $RANDOM/3276.7" | bc)")
    # Generar humedad aleatoria entre 0 y 100 con un decimal
    HUMEDAD=$(printf "%.1f" "$(echo "scale=1; $RANDOM/327.67" | bc)")
    FECHA=$(date +"%Y-%m-%dT%H:%M:%S")
    # Construir el JSON
    JSON="{\"ID\": $ID, \"temperatura\": $TEMPERATURA, \"humedad\": $HUMEDAD, \"timestamp\": \"$FECHA\"}"
    # Enviar datos al gateway
    curl -X POST http://localhost:8081/gateway.php -H "Content-Type: application/json" -d "$JSON"
    # Esperar 10 segundos   
    sleep 10
done