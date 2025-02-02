<?php

// URL base del servidor (sin el parámetro `type`)
$baseUrl = 'http://webalumnos.tlm.unavarra.es:10303/php/manage_data.php?type=';

// Función para enviar datos JSON a la URL con el tipo especificado
function sendJsonData($type, $filePath)
{
    global $baseUrl;

    // Verificar si el archivo existe
    if (!file_exists($filePath)) {
        die("El archivo $filePath no existe");
    }

    // Leer el contenido del archivo JSON y decodificarlo
    $dataArray = json_decode(file_get_contents($filePath), true);

    // Convertir timestamps Unix a formato de fecha y hora válido
    if ($type === 'sensorData') {
        foreach ($dataArray as &$data) {
            if (isset($data['timestamp'])) {
                $data['timestamp'] = date('Y-m-d H:i:s', $data['timestamp']);
            }
        }
    }

    $jsonDataToSend = json_encode($dataArray);

    // Verificar si la codificación JSON fue exitosa
    if ($jsonDataToSend === false) {
        die('Error al codificar datos a JSON: ' . json_last_error_msg());
    }

    // Concatenar el tipo a la URL base
    $url = $baseUrl . $type;

    // Inicializar cURL para enviar los datos JSON al servidor
    $ch = curl_init($url);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
    curl_setopt($ch, CURLOPT_HTTPHEADER, ['Content-Type: application/json']);
    curl_setopt($ch, CURLOPT_POST, true);
    curl_setopt($ch, CURLOPT_POSTFIELDS, $jsonDataToSend);

    // Ejecutar la solicitud y obtener la respuesta
    $response = curl_exec($ch);

    echo $response;

    $responseArray = json_decode($response, true);

    curl_close($ch);

    print_r($responseArray);

    // Procesar la respuesta para actualizar o eliminar datos locales según corresponda
    if ($type === 'ledStatus') {
        // Si estamos en ledStatus, actualizar el `desired_status`
        foreach ($responseArray as $key => $value) {
            $dataArray[$key]['desired_status'] = $value['desired_status'];
        }
        file_put_contents($filePath, json_encode($dataArray));
    } elseif ($type === 'sensorData') {
        // Si estamos en sensorData, eliminar los datos enviados con estado 200
        if ($responseArray === null || $responseArray['error']) {
            die('Error al enviar datos');
        } else {
            foreach ($responseArray as $key => $value) {
                if ($value['status'] === 200) {
                    unset($dataArray[$key]);
                }
            }
        }
        file_put_contents($filePath, json_encode($dataArray));
    }
}

// Llamar a la función para `ledStatus`
sendJsonData('ledStatus', 'led_status.json');

// Llamar a la función para `sensorData`
sendJsonData('sensorData', 'sensor_data.json');
