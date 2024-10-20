<?php
// Configuración de la base de datos
define('DB_HOST', 'localhost');
define('DB_NAME', 't_h_DataBase');
define('DB_USER', 'grupo03');
define('DB_PASS', 'Hola1234+');

// Crear socket
$serverSocket = socket_create(AF_INET, SOCK_STREAM, 0);
if ($serverSocket === false) {
    error_log("No se pudo crear el socket: " . socket_strerror(socket_last_error()));
    exit(1);
}

// Enlazar el socket a una dirección y puerto
$port = 8080; // Puedes usar otro puerto si lo deseas
if (socket_bind($serverSocket, '127.0.0.1', $port) === false) {
    error_log("No se pudo enlazar el socket: " . socket_strerror(socket_last_error($serverSocket)));
    socket_close($serverSocket);
    exit(1);
}

// Escuchar conexiones
if (socket_listen($serverSocket, 5) === false) {
    error_log("Error al escuchar el socket: " . socket_strerror(socket_last_error($serverSocket)));
    socket_close($serverSocket);
    exit(1);
}

echo "Servidor escuchando en el puerto $port...\n";

// Aceptar conexiones y manejar datos
while (true) {
    // Aceptar una conexión
    $clientSocket = socket_accept($serverSocket);
    if ($clientSocket === false) {
        error_log("No se pudo aceptar la conexión: " . socket_strerror(socket_last_error($serverSocket)));
        continue;
    }

    // Recibir datos del cliente
    $input = socket_read($clientSocket, 4096);
    if ($input === false) {
        error_log("Error al leer datos del cliente: " . socket_strerror(socket_last_error($clientSocket)));
        socket_close($clientSocket);
        continue;
    }

    // Separar la cabecera de los datos
    $inputParts = explode("\r\n\r\n", $input);
    $datos = $inputParts[1];

    // Decodificar el JSON recibido
    $data = json_decode($datos, true);

    if ($data !== null && json_last_error() === JSON_ERROR_NONE) {
        try {
            $conn = new PDO("mysql:host=" . DB_HOST . ";dbname=" . DB_NAME, DB_USER, DB_PASS);
            $conn->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

            // Iterar sobre el array principal
            foreach ($data as $dato) {
                // Verificar y preparar la consulta
                if (isset($dato['ID']) && isset($dato['temperatura']) && isset($dato['humedad']) && isset($dato['timestamp'])) {
                    $id_sonda = filter_var($dato['ID'], FILTER_SANITIZE_NUMBER_INT);
                    $temperatura = filter_var($dato['temperatura'], FILTER_SANITIZE_NUMBER_FLOAT, FILTER_FLAG_ALLOW_FRACTION);
                    $humedad = filter_var($dato['humedad'], FILTER_SANITIZE_NUMBER_FLOAT, FILTER_FLAG_ALLOW_FRACTION);
                    $tiempo_muestra = filter_var($dato['timestamp'], FILTER_SANITIZE_FULL_SPECIAL_CHARS);

                    $stmt = $conn->prepare("INSERT INTO data (id_sonda, temperatura, humedad, tiempo_muestra) VALUES (:id_sonda, :temperatura, :humedad, :tiempo_muestra)");
                    $stmt->bindParam(':id_sonda', $id_sonda);
                    $stmt->bindParam(':temperatura', $temperatura);
                    $stmt->bindParam(':humedad', $humedad);
                    $stmt->bindParam(':tiempo_muestra', $tiempo_muestra);

                    // Ejecutar la consulta
                    if ($stmt->execute()) {
                        // Respuesta con estado 200
                        $response = array('status' => 200, 'message' => 'Datos insertados');
                    } else {
                        $response = array('status' => 500, 'message' => 'Error al insertar los datos.');
                        $errorInfo = $stmt->errorInfo();
                        error_log("Error al insertar los datos: " . $errorInfo[2]);
                    }
                } else {
                    $response = array('status' => 400, 'message' => 'Datos no válidos.');
                }
            }
        } catch (PDOException $e) {
            $response = array('status' => 500, 'message' => $e->getMessage());
        }
    } else {
        $response = array('status' => 400, 'message' => 'Datos JSON inválidos o no recibidos correctamente.');
    }

    // Enviar respuesta al cliente con encabezados HTTP
    $responseJson = json_encode($response);
    $httpResponse = "HTTP/1.1 " . $response['status'] . " " . getHttpStatusMessage($response['status']) . "\r\n";
    $httpResponse .= "Content-Type: application/json\r\n";
    $httpResponse .= "Content-Length: " . strlen($responseJson) . "\r\n";
    $httpResponse .= "\r\n";
    $httpResponse .= $responseJson;

    socket_write($clientSocket, $httpResponse, strlen($httpResponse));

    // Cerrar el socket del cliente
    socket_close($clientSocket);
}

// Cerrar el socket del servidor
socket_close($serverSocket);

// Función para obtener el mensaje de estado HTTP
function getHttpStatusMessage($statusCode)
{
    $statusMessages = [
        200 => 'OK',
        400 => 'Bad Request',
        500 => 'Internal Server Error',
    ];
    return isset($statusMessages[$statusCode]) ? $statusMessages[$statusCode] : 'Unknown Status';
}