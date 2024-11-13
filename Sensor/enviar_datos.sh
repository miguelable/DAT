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

ID=48
ESTADO_LED=1
AUTH_TOKEN="mi_token_super_secreto"

# Enviar de forma periódica datos de temperatura aleatorios entre 0 y 50 cada 10 segundos con decimal
# Se envían al gateway en formato JSON

echo "Simulando envio de datos cada 10 segundos"

# Datos de la sonda
while true
do
    # Genera un valor de potencia aleatorio entre 0 y 4095
    POTENCIA=$(shuf -i 0-4095 -n 1)
    FECHA=$(date +"%Y-%m-%dT%H:%M:%S")
    # Construir el JSON
    JSON="{\"id_sonda\": $ID, \"potencia\": $POTENCIA, \"timestamp\": \"$FECHA\", \"actual_status\": $ESTADO_LED}"
    # Enviar datos al gateway y esperar respuesta de desired led state
    respuesta=$(curl -s -k -X POST -H "Content-Type: application/json" -H "Authorization: Bearer $AUTH_TOKEN" -d "$JSON" https://localhost:8443/gateway.php)  
    # Datos enviados
    echo "Datos enviados: $JSON - Respuesta: $respuesta"
    # Extraer desired_status de la respuesta
    desired_status=$(echo $respuesta | jq -r '.desired_status')

    # Comparar desired_status con ESTADO_LED y verificar si existe $desired_status
    if [ -n "$desired_status" ]; then
        if [ "$desired_status" != "$ESTADO_LED" ]; then
            echo "Cambiando estado del LED de $ESTADO_LED a $desired_status"
            ESTADO_LED=$desired_status
        fi
    fi
    # Esperar 2 segundos   
    sleep 2
done