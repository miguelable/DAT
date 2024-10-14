<?php
// Leer el contenido del archivo JSON
$jsonData = file_get_contents('datos_sonda.json');

// Decodificar el JSON a un array asociativo
$dataArray = json_decode($jsonData, true);

// URL del servidor al que se enviarán los datos
$url = 'http://192.168.0.1:8080';

// Inicializar cURL
$ch = curl_init($url);

// Configurar cURL para enviar una solicitud POST con los datos JSON
curl_setopt($ch, CURLOPT_POST, 1);
curl_setopt($ch, CURLOPT_POSTFIELDS, json_encode($dataArray));
curl_setopt($ch, CURLOPT_HTTPHEADER, array('Content-Type: application/json'));

// Ejecutar la solicitud y obtener la respuesta
$response = curl_exec($ch);

// Cerrar cURL
curl_close($ch);

// Mostrar la respuesta del servidor
echo $response;

// Eliminar el fichero de datos
if ($response == 'OK')
    unlink('datos_sonda.json');