<?php

// Verificar si el archivo 'datos_sonda.json' existe
if (!file_exists('datos_sonda.json')) {
    die('El archivo datos_sonda.json no existe');
}

// Leer el contenido del archivo JSON
$jsonData = file_get_contents('datos_sonda.json');

// Verificar si hubo un error al leer el archivo
if ($jsonData === false) {
    die('Error al leer el archivo datos_sonda.json');
}

// Decodificar el JSON a un array asociativo
$dataArray = json_decode($jsonData, true);

// Verificar si hubo un error en la decodificación
if (json_last_error() !== JSON_ERROR_NONE) {
    die('Error al decodificar JSON: ' . json_last_error_msg());
}

// URL del servidor al que se enviarán los datos
$url = 'http://127.0.0.1:8080';

// Codificar el array de nuevo a JSON para enviarlo
$jsonDataToSend = json_encode($dataArray, JSON_UNESCAPED_UNICODE | JSON_PRETTY_PRINT);

// Verificar si hubo un error al codificar los datos
if ($jsonDataToSend === false) {
    die('Error al codificar datos a JSON: ' . json_last_error_msg());
}

// Mostrar el JSON que se enviará (para depuración)
echo "Datos JSON que se enviarán:\n";
echo $jsonDataToSend . "\n\n";

// Configurar las opciones de la solicitud HTTP
$options = [
    'http' => [
        'header'  => "Content-type: application/json\r\n",
        'method'  => 'POST',
        'content' => $jsonDataToSend,
    ],
];

// Crear el contexto de la solicitud
$context  = stream_context_create($options);

// Enviar la solicitud y obtener la respuesta
$response = @file_get_contents($url, false, $context);

// Verificar si la solicitud fue exitosa
if ($response === false) {
    $error = error_get_last();
    die('Error al enviar la solicitud: ' . $error['message']);
}

// Mostrar la respuesta del servidor (para depuración)
echo "Respuesta del servidor:\n";
echo $response . "\n";

// Decodificar la respuesta del servidor
$responseData = json_decode($response, true);

// Verificar si la respuesta es válida y contiene un código de estado 200
if ($responseData && isset($responseData['status']) && $responseData['status'] == 200) {
    // Eliminar el fichero de datos si la respuesta es 'OK'
    if (!unlink('datos_sonda.json')) {
        echo "Error al eliminar el archivo datos_sonda.json\n";
    } else {
        echo "Archivo datos_sonda.json eliminado correctamente\n";
    }
} else {
    echo "La respuesta del servidor no fue exitosa o no contiene un estado 200\n";
}