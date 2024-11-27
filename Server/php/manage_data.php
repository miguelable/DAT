<?php

// Configuración de la base de datos
define('DB_HOST', 'dbserver');
define('DB_NAME', 'grupo03');
define('DB_USER', 'grupo03');
define('DB_PASS', 'tai1mui1Go');

// Función para establecer la conexión con la base de datos
function getDbConnection()
{
    try {
        $conn = new PDO("mysql:host=" . DB_HOST . ";dbname=" . DB_NAME, DB_USER, DB_PASS);
        $conn->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
        return $conn;
    } catch (PDOException $e) {
        echo json_encode(['error' => 'Error de conexión: ' . $e->getMessage()]);
        exit;
    }
}

// Función para manejar el tipo `ledStatus`
function handleLedStatus($conn, $data)
{
    $response = [];
    foreach ($data as $sonda) {
        if (isset($sonda['id_sonda']) && isset($sonda['actual_status'])) {
            $id_sonda = $sonda['id_sonda'];
            $actual_status = $sonda['actual_status'];
            $desired_status = getDesiredStatus($conn, $id_sonda);

            if ($desired_status !== null) {
                updateActualStatus($conn, $id_sonda, $actual_status);
                $response[] = ['id_sonda' => $id_sonda, 'desired_status' => $desired_status, 'actual_status' => $actual_status];
            } else {
                createLedStatus($conn, $id_sonda, $actual_status);
                $response[] = ['id_sonda' => $id_sonda, 'desired_status' => $actual_status, 'actual_status' => $actual_status];
            }
        } else {
            $response[] = ['error' => 'Datos incompletos para la sonda'];
        }
    }
    return $response;
}

// Función para obtener el desired_status de la base de datos
function getDesiredStatus($conn, $id_sonda)
{
    $stmt = $conn->prepare("SELECT desired_status FROM LED_STATUS WHERE id_sonda = :id_sonda");
    $stmt->bindParam(':id_sonda', $id_sonda);
    $stmt->execute();
    $result = $stmt->fetch(PDO::FETCH_ASSOC);
    return $result ? $result['desired_status'] : null;
}

// Función para actualizar el actual_status de la base de datos
function updateActualStatus($conn, $id_sonda, $actual_status)
{
    $stmt = $conn->prepare("UPDATE LED_STATUS SET actual_status = :actual_status WHERE id_sonda = :id_sonda");
    $stmt->bindParam(':actual_status', $actual_status);
    $stmt->bindParam(':id_sonda', $id_sonda);
    $stmt->execute();
}

// Función para crear un nuevo registro en la tabla LED_STATUS
function createLedStatus($conn, $id_sonda, $actual_status)
{
    $stmt = $conn->prepare("INSERT INTO LED_STATUS (id_sonda, actual_status, desired_status) VALUES (:id_sonda, :actual_status, :desired_status)");
    $stmt->bindParam(':id_sonda', $id_sonda);
    $stmt->bindParam(':actual_status', $actual_status);
    $stmt->bindParam(':desired_status', $actual_status);
    $stmt->execute();
}

// Función para manejar el tipo `sensorData`
function handleSensorData($conn, $data)
{
    $response = [];
    foreach ($data as $dato) {
        if (isset($dato['id_sonda']) && isset($dato['potencia']) && isset($dato['timestamp'])) {
            $id_sonda = filter_var($dato['id_sonda'], FILTER_SANITIZE_NUMBER_INT);
            $potencia = filter_var($dato['potencia'], FILTER_SANITIZE_NUMBER_INT, FILTER_FLAG_ALLOW_FRACTION);
            $tiempo_muestra = filter_var($dato['timestamp'], FILTER_SANITIZE_FULL_SPECIAL_CHARS);

            insertSensorData($conn, $id_sonda, $potencia, $tiempo_muestra);
            $response[] = ['status' => 200, 'id_sonda' => $id_sonda, 'potencia' => $potencia];
        } else {
            $response[] = ['error' => 'Datos incompletos para el sensor'];
        }
    }
    return $response;
}

// Función para insertar datos en la tabla POT_SENSOR
function insertSensorData($conn, $id_sonda, $potencia, $tiempo_muestra)
{
    $stmt = $conn->prepare("INSERT INTO POT_SENSOR (id_sonda, potencia, tiempo_muestra) VALUES (:id_sonda, :potencia, :tiempo_muestra)");
    $stmt->bindParam(':id_sonda', $id_sonda);
    $stmt->bindParam(':potencia', $potencia);
    $stmt->bindParam(':tiempo_muestra', $tiempo_muestra);
    $stmt->execute();
}

// Función para procesar la solicitud en función del tipo
function processRequest($type, $data)
{
    $conn = getDbConnection();
    switch ($type) {
        case 'ledStatus':
            return handleLedStatus($conn, $data);
        case 'sensorData':
            return handleSensorData($conn, $data);
        default:
            return ['error' => 'Tipo no válido'];
    }
}

// Función principal para manejar la solicitud y devolver la respuesta
function main()
{
    $type = isset($_GET['type']) ? $_GET['type'] : null;
    $inputData = file_get_contents("php://input");
    $data = json_decode($inputData, true);

    if ($data !== null && json_last_error() === JSON_ERROR_NONE) {
        $response = processRequest($type, $data);
    } else {
        $response = ['error' => 'JSON invalido'];
    }

    echo json_encode($response);
}

// Ejecutar el script
main();
