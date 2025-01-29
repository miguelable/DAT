<?php
// Configuración de la base de datos
define('DB_HOST', 'dbserver');
define('DB_NAME', 'grupo03');
define('DB_USER', 'grupo03');
define('DB_PASS', 'tai1mui1Go');

// Configuración MQTT
require 'vendor/autoload.php'; // Asegúrate de que el autoload de Composer esté configurado
define('MQTT_SERVER', 'webalumnos.tlm.unavarra.es');
define('MQTT_PORT', 10320);
define('MQTT_CLIENT_ID', 'phpMQTT-publisher-' . uniqid());
define('MQTT_TOPIC', 'status/device/3');

header('Content-Type: application/json');

// Verificar si se recibió el parámetro 'state'
$input = json_decode(file_get_contents('php://input'), true);
if (!isset($input['state'])) {
    // Si no se recibe el parámetro 'state', no hacer nada
    echo json_encode(['message' => 'Estado no recibido.']);
    exit;
}

try {
    // Conexión a la base de datos
    $conn = new PDO("mysql:host=" . DB_HOST . ";dbname=" . DB_NAME, DB_USER, DB_PASS);
    $conn->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

    // Obtener el valor del estado
    $newState = $input['state'];

    // Validar que el estado sea válido
    if ($newState !== 1 && $newState !== 0) {
        echo json_encode(['message' => 'Estado inválido. Debe ser 1 o 0.']);
        exit;
    }

    // Convertir el estado 1/0 a "Encendido"/"Apagado"
    $newState = ($newState === 1) ? 'Encendido' : 'Apagado';

    // Actualizar el estado de todos los dispositivos
    $stmt = $conn->prepare("UPDATE dispositivos SET estado = :state");
    $stmt->bindValue(':state', $newState, PDO::PARAM_STR);
    $stmt->execute();

    // Publicar mensaje MQTT
    $mqtt = new Bluerhinos\phpMQTT(MQTT_SERVER, MQTT_PORT, MQTT_CLIENT_ID);

    if ($mqtt->connect(true, NULL, '', '')) { // Si es necesario, añade usuario y contraseña
        $mqtt->publish(MQTT_TOPIC, '{"device":"3","current_status": "' . $newState . '"}', 0, false);
        $mqtt->close();
    } else {
        error_log('Error: No se pudo conectar al servidor MQTT.');
    }

    // Devolver respuesta exitosa
    echo json_encode(['message' => 'Estado actualizado correctamente.']);
} catch (PDOException $e) {
    // Manejo de errores de conexión y consultas
    echo json_encode(['message' => 'Error: ' . $e->getMessage()]);
    exit;
}