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

try {
    $conn = new PDO("mysql:host=" . DB_HOST . ";dbname=" . DB_NAME, DB_USER, DB_PASS);
    $conn->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

    // Leer el cuerpo de la solicitud
    $input = json_decode(file_get_contents('php://input'), true);
    $deviceId = $input['id'];
    $newState = $input['state'];

    // Validar entrada
    if (!is_numeric($deviceId) || !in_array($newState, [0, 1])) {
        echo json_encode(['success' => false, 'message' => 'Datos inválidos']);
        exit;
    }

    // Convertir el estado numérico a texto
    $estadoTexto = $newState === 1 ? 'Encendido' : 'Apagado';

    // Actualizar el estado del dispositivo en la base de datos
    $stmt = $conn->prepare("UPDATE dispositivos SET estado = :estado WHERE id = :id");
    $stmt->execute([':estado' => $estadoTexto, ':id' => $deviceId]);

    // Publicar mensaje MQTT
    $mqtt = new Bluerhinos\phpMQTT(MQTT_SERVER, MQTT_PORT, MQTT_CLIENT_ID);

    if ($mqtt->connect(true, NULL, '', '')) {
        $mqtt->publish(MQTT_TOPIC, '{"device":"' . $deviceId . '","current_status": "' . $estadoTexto . '"}', 0, false);
        $mqtt->close();
    } else {
        error_log('Error: No se pudo conectar al servidor MQTT.');
    }

    echo json_encode(['success' => true, 'message' => 'Estado actualizado correctamente']);
} catch (PDOException $e) {
    echo json_encode(['success' => false, 'message' => 'Error en la base de datos', 'error' => $e->getMessage()]);
}