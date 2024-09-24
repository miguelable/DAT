<?php

$ip = '127.0.0.1';
$puerto = 8888; // Cambiado a 8080 para evitar conflictos con el puerto 80, que suele estar en uso para otros servicios

function run_server($ip, $puerto) {

    // Crear un socket
    $sock = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
    if ($sock === false) {
        die("Error al crear el socket: " . socket_strerror(socket_last_error()));
    }

    // Asociar el socket a una dirección/puerto
    if (socket_bind($sock, $ip, $puerto) === false) {
        die("Error al asociar el socket: " . socket_strerror(socket_last_error()));
    }

    // Escuchar conexiones
    if (socket_listen($sock, 10) === false) {
        die("Error al escuchar en el socket: " . socket_strerror(socket_last_error()));
    }

    echo "Servidor escuchando en $ip:$puerto\n";

    while (true) {
        // Aceptar conexiones entrantes
        $client = socket_accept($sock);
        if ($client === false) {
            echo "Error al aceptar conexión: " . socket_strerror(socket_last_error()) . "\n";
            continue;
        }

        echo "Cliente conectado\n";
        handle_client($client);
    }

    // Cerrar el socket del servidor (esto nunca se alcanzará en este ejemplo)
    socket_close($sock);
}

function handle_client($client) {
    // Leer la solicitud del cliente
    $request = socket_read($client, 1024);
    if ($request === false) {
        echo "Error al leer solicitud: " . socket_strerror(socket_last_error()) . "\n";
        socket_close($client);
        return;
    }

    // Analizar la solicitud 
    $header = extract_header($request);

    // Enviar la respuesta
    $file_path = __DIR__ . $header['path']; // Ruta completa del archivo
    if (file_exists($file_path)) {
        $file_content = file_get_contents($file_path);
        $response = "HTTP/1.1 200 OK\r\n";
        $response .= "Content-Length: " . strlen($file_content) . "\r\n";
        $response .= "Content-Type: application/octet-stream\r\n";
        $response .= "\r\n";
        $response .= $file_content;
    } else {
        $response = "HTTP/1.1 404 Not Found\r\n";
        $response .= "\r\n";
    }

    socket_write($client, $response);
    socket_close($client);
}

function extract_header($request) {
    // Simplemente extraemos la ruta para este ejemplo. Puedes expandir esto según sea necesario.
    $lines = explode("\r\n", $request);
    $request_line = explode(' ', $lines[0]);
    $path = $request_line[1]; // La ruta solicitada

    return array(
        "protocol_version" => "HTTP/1.1",
        "path" => $path
    );
}
?>
