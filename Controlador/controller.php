<?php
// Configuración de la base de datos
$host = "localhost"; 
$dbname = "data"; // Nombre de la base de datos
$username = "grupo03"; // Usuario de SQL
$password = "Hola1234+"; // Contraseña de SQL

// Crear socket
$serverSocket = socket_create(AF_INET, SOCK_STREAM, 0);
if ($serverSocket === false) {
    die("No se pudo crear el socket: " . socket_strerror(socket_last_error()));
}

// Enlazar el socket a una dirección y puerto
$port = 8080; // Puedes usar otro puerto si lo deseas
if (socket_bind($serverSocket, '127.0.0.1', $port) === false) {
    die("No se pudo enlazar el socket: " . socket_strerror(socket_last_error($serverSocket)));
}

// Escuchar conexiones
if (socket_listen($serverSocket, 5) === false) {
    die("Error al escuchar el socket: " . socket_strerror(socket_last_error($serverSocket)));
}

echo "Servidor escuchando en el puerto $port...\n";

// Aceptar conexiones y manejar datos
while (true) {
    // Aceptar una conexión
    $clientSocket = socket_accept($serverSocket);
    if ($clientSocket === false) {
        echo "No se pudo aceptar la conexión: " . socket_strerror(socket_last_error($serverSocket));
        continue;
    }

    // Recibir datos del cliente
    $input = socket_read($clientSocket, 1024);
    
    // Decodificar el JSON recibido
    $data = json_decode($input, true);
    
    // Conectar a la base de datos
    try {
        $conn = new PDO("mysql:host=$host;dbname=$dbname", $username, $password);
        $conn->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

        // Verificar y preparar la consulta
        if (isset($data['temperatura']) && isset($data['humedad'])) {
            $temperatura = $data['temperatura'];
            $humedad = $data['humedad'];

            $stmt = $conn->prepare("INSERT INTO data (temperatura, humedad) VALUES (:temperatura, :humedad)");
            $stmt->bindParam(':temperatura', $temperatura);
            $stmt->bindParam(':humedad', $humedad);
            
            // Ejecutar la consulta
            if ($stmt->execute()) {
                $response = json_encode(["status" => "success", "message" => "Datos insertados correctamente."]);
            } else {
                $response = json_encode(["status" => "error", "message" => "Error al insertar los datos."]);
            }
        } else {
            $response = json_encode(["status" => "error", "message" => "Datos no válidos."]);
        }
    } catch (PDOException $e) {
        $response = json_encode(["status" => "error", "message" => $e->getMessage()]);
    }

    // Enviar respuesta al cliente
    socket_write($clientSocket, $response, strlen($response));

    // Cerrar el socket del cliente
    socket_close($clientSocket);
}

// Cerrar el socket del servidor
socket_close($serverSocket);

