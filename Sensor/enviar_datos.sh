#!/bin/bash

ID=48

# Enviar de forma periódica datos de temperatura aleatorios entre 0 y 50 cada 10 segundos con decimal
# Se envían al gateway en formato JSON

# Bucle infinito
while true
do
    # Datos de la sonda
    TEMPERATURA=$(awk -v min=0 -v max=50 'BEGIN{srand(); printf "%.1f", min+rand()*(max-min)}')
    FECHA=$(date +"%Y-%m-%dT%H:%M:%S")
    # Construir el JSON
    JSON="{\"ID\": $ID, \"temperatura\": $TEMPERATURA, \"timestamp\": \"$FECHA\"}"
    # Enviar datos al gateway
    curl -X POST http://localhost:8080/gateway.php -H "Content-Type: application/json" -d "$JSON"
    sleep 10
done