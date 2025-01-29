<?php

// Ruta para guardar la imagen reconstruida
$reconstructedImageFile = '../images/last_image.jpg';

// Ruta para guardar los fragmentos recibidos temporalmente
$tempDir = 'fragments/';

// Configuración de la base de datos
define('DB_HOST', 'dbserver');  // Cambia 'dbserver' por 'localhost' si es necesario
define('DB_NAME', 'grupo03');   // Nombre de la base de datos
define('DB_USER', 'grupo03');   // Nombre de usuario de la base de datos
define('DB_PASS', 'tai1mui1Go'); // Contraseña de la base de datos

// Configuración MQTT
require 'vendor/autoload.php'; // Asegúrate de que el autoload de Composer esté configurado
define('MQTT_SERVER', 'webalumnos.tlm.unavarra.es');
define('MQTT_PORT', 10320);
define('MQTT_CLIENT_ID', 'phpMQTT-publisher-' . uniqid());
define('MQTT_TOPIC', 'status/device/3');

// Conexión a la base de datos
try {
    $conn = new PDO("mysql:host=" . DB_HOST . ";dbname=" . DB_NAME, DB_USER, DB_PASS);
    $conn->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
} catch (PDOException $e) {
    echo json_encode(['status' => 'error', 'message' => 'Error de conexión: ' . $e->getMessage()]);
    exit;
}

// Leer el cuerpo de la solicitud
$requestBody = file_get_contents('php://input');
$data = json_decode($requestBody, true);

if ($data === null || json_last_error() !== JSON_ERROR_NONE) {
    echo json_encode(['status' => 'error', 'message' => 'JSON invalido']);
    exit;
}

// Validar que la solicitud contenga los campos necesarios
if (
    !isset($data['unique_id'], $data['fragment_number'], $data['total_fragments'], $data['data'])
) {
    echo json_encode(['status' => 'error', 'message' => 'Datos incompletos en la solicitud']);
    exit;
}

$uniqueId = $data['unique_id'];
$fragmentNumber = (int)$data['fragment_number'];
$totalFragments = (int)$data['total_fragments'];
$fragmentData = $data['data'];

// Guardar el fragmento en un archivo temporal
$fragmentFile = $tempDir . $uniqueId . '_fragment_' . $fragmentNumber . '.txt';
if (file_put_contents($fragmentFile, $fragmentData) === false) {
    echo json_encode(['status' => 'error', 'message' => 'No se pudo guardar el fragmento']);
    exit;
}

// Verificar si se han recibido todos los fragmentos
$receivedFragments = glob($tempDir . $uniqueId . '_fragment_*.txt');
if (count($receivedFragments) === $totalFragments) {
    // Reensamblar los fragmentos en el orden correcto
    $completeData = '';
    for ($i = 1; $i <= $totalFragments; $i++) {
        $fragmentFile = $tempDir . $uniqueId . '_fragment_' . $i . '.txt';
        $fragmentContent = file_get_contents($fragmentFile);
        if ($fragmentContent === false) {
            echo json_encode(['status' => 'error', 'message' => 'Error al leer el fragmento ' . $i]);
            exit;
        }
        $completeData .= $fragmentContent;
    }

    // Decodificar la imagen Base64 y guardarla como archivo
    $imageData = base64_decode($completeData);
    if ($imageData === false) {
        echo json_encode(['status' => 'error', 'message' => 'Error al decodificar la imagen']);
        exit;
    }

    if (file_put_contents($reconstructedImageFile, $imageData) === false) {
        echo json_encode(['status' => 'error', 'message' => 'No se pudo guardar la imagen reconstruida']);
        exit;
    }

    // Actualizar todos los usuarios en la tabla 'users'
    try {
        $currentTimestamp = date('Y-m-d H:i:s'); // Fecha y hora actual
        $imageDataBase64 = base64_encode($imageData); // Imagen en Base64

        $query = "UPDATE users SET ultimo_registro = :currentTimestamp, ultima_imagen = :imageDataBase64";
        $stmt = $conn->prepare($query);
        $stmt->bindParam(':currentTimestamp', $currentTimestamp);
        $stmt->bindParam(':imageDataBase64', $imageDataBase64);
        $stmt->execute();

        // Eliminar los fragmentos temporales
        array_map('unlink', $receivedFragments);

        // Publicar mensaje MQTT
        $mqtt = new Bluerhinos\phpMQTT(MQTT_SERVER, MQTT_PORT, MQTT_CLIENT_ID);

        if ($mqtt->connect(true, NULL, '', '')) { // Si es necesario, añade usuario y contraseña
            $mqtt->publish(MQTT_TOPIC, '{"device":"3","current_status":"Exito"}', 0, false); // QoS 0, sin retener
            $mqtt->close();
        } else {
            error_log('Error: No se pudo conectar al servidor MQTT.');
        }
        echo json_encode(['status' => 'success', 'message' => 'Imagen reconstruida y usuarios actualizados exitosamente']);
    } catch (PDOException $e) {
        // publicar mensaje MQTT
        $mqtt = new Bluerhinos\phpMQTT(MQTT_SERVER, MQTT_PORT, MQTT_CLIENT_ID);

        if ($mqtt->connect(true, NULL, '', '')) { // Si es necesario, añade usuario y contraseña
            $mqtt->publish(MQTT_TOPIC, '{"device":"3","current_status":"Error"}', 0, false); // QoS 0, sin retener
            $mqtt->close();
        } else {
            error_log('Error: No se pudo conectar al servidor MQTT.');
        }
        echo json_encode(['status' => 'error', 'message' => 'Error al actualizar usuarios: ' . $e->getMessage()]);
    }
} else {
    echo json_encode(['status' => 'success', 'message' => 'Fragmento recibido', 'fragment_received' => $fragmentNumber]);
}