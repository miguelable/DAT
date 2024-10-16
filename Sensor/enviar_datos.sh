#!/bin/bash

ID=48

# Enviar de forma periódica datos de temperatura aleatorios entre 0 y 50 cada 10 segundos con decimal
# Se envían al gateway en formato JSON

# Datos de la sonda
TEMPERATURA=$(awk -v min=0 -v max=50 'BEGIN{srand(); printf "%.1f", min+rand()*(max-min)}')
HUMEDAD=$(awk -v min=0 -v max=100 'BEGIN{srand(); printf "%.1f", min+rand()*(max-min)}')
FECHA=$(date +"%Y-%m-%dT%H:%M:%S")
# Construir el JSON
JSON="{\"ID\": $ID, \"temperatura\": $TEMPERATURA, \"humedad\": $HUMEDAD, \"timestamp\": \"$FECHA\"}"
# Enviar datos al gateway
curl -X POST http://localhost:8081/gateway.php -H "Content-Type: application/json" -d "$JSON"
sleep 10
