<?php

require 'vendor/autoload.php'; // Autoload de Composer para phpMQTT

// Configuración del servidor MQTT
$server = 'webalumnos.tlm.unavarra.es';
$port = 10320;
$username = ''; // Usuario MQTT, si aplica
$password = ''; // Contraseña MQTT, si aplica
$client_id = 'phpMQTT-subscriber-' . uniqid();

// Configuración de la base de datos
define('DB_HOST', 'dbserver');
define('DB_NAME', 'grupo03');
define('DB_USER', 'grupo03');
define('DB_PASS', 'tai1mui1Go');

// Conectar a la base de datos
try {
    $conn = new PDO("mysql:host=" . DB_HOST . ";dbname=" . DB_NAME, DB_USER, DB_PASS);
    $conn->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
} catch (PDOException $e) {
    die("Error al conectar con la base de datos: " . $e->getMessage() . "\n");
}

// Función para manejar los mensajes del topic `activation`
function procMsg($topic, $msg)
{
    global $conn;

    echo "Mensaje recibido en el topic [$topic]: $msg\n";

    // Decodificar el mensaje JSON
    $data = json_decode($msg, true);
    if (!$data || !isset($data['device'], $data['status'])) {
        echo "Formato de mensaje inválido\n";
        return;
    }

    $deviceId = $data['device'];

    // Consultar el estado actual del dispositivo desde la base de datos
    try {
        $stmt = $conn->prepare("SELECT estado FROM dispositivos WHERE id = :id");
        $stmt->execute([':id' => $deviceId]);
        $result = $stmt->fetch(PDO::FETCH_ASSOC);

        if ($result) {
            $estadoActual = $result['estado'];
        } else {
            echo "Dispositivo con ID $deviceId no encontrado en la base de datos\n";
            return;
        }
    } catch (PDOException $e) {
        echo "Error al consultar el estado del dispositivo: " . $e->getMessage() . "\n";
        return;
    }

    // Crear y publicar el mensaje de respuesta
    $responseTopic = "status/device/$deviceId";
    $responseMessage = json_encode(['device' => $deviceId, 'current_status' => $estadoActual]);

    global $mqtt;
    if ($mqtt->connect(true, NULL, '', '')) {
        $mqtt->publish($responseTopic, $responseMessage, 0, false);
        $mqtt->close();
        echo "Respuesta publicada en el topic [$responseTopic]: $responseMessage\n";
    } else {
        echo "Error: No se pudo conectar al servidor MQTT para publicar el mensaje.\n";
    }
}

// Crear instancia del cliente MQTT
$mqtt = new Bluerhinos\phpMQTT($server, $port, $client_id);

// Conectarse al servidor MQTT
if (!$mqtt->connect(true, NULL, $username, $password)) {
    die("Error: No se pudo conectar al servidor MQTT.\n");
}

// Suscribirse al topic `activation`
$topics['activation'] = [
    "qos" => 0, // QoS 0: entrega mejor esfuerzo
    "function" => "procMsg" // Función a ejecutar cuando se recibe un mensaje
];
$mqtt->subscribe($topics);

// Bucle para escuchar mensajes continuamente
while ($mqtt->proc()) {
    // El bucle mantiene el script en ejecución escuchando mensajes
}

// Cerrar la conexión cuando termine
$mqtt->close();