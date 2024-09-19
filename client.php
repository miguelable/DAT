<?php
// Función para obtener el contenido de la URL proporcionada y guardarlo en un archivo
function run_client($ip, $puerto, $path, $output_file) {
    // Crear un socket TCP
    $sock = socket_create(AF_INET, SOCK_STREAM, getprotobyname('tcp'));
    if ($sock === false) {
        die("Error al crear el socket: " . socket_strerror(socket_last_error()));
    }

    // Conectar al servidor
    if (socket_connect($sock, $ip, $puerto) === false) {
        die("Error al conectar al servidor: " . socket_strerror(socket_last_error($sock)));
    }

    // Enviar solicitud GET al servidor para el archivo en $path
    $mensaje = "GET $path HTTP/1.1\r\nHost: $ip:$puerto\r\nConnection: close\r\n\r\n";
    if (socket_write($sock, $mensaje, strlen($mensaje)) === false) {
        die("Error al enviar la solicitud: " . socket_strerror(socket_last_error($sock)));
    }
    
    // Recibir la respuesta del servidor
    $respuesta = '';
    while ($chunk = socket_read($sock, 1024)) {
        $respuesta .= $chunk;
    }

    // Verificar si la respuesta contiene '404 Not Found' (o cualquier código 404)
    if (strpos($respuesta, '404 Not Found') !== false) {
        echo "El archivo no fue encontrado.\n";
        // Cerrar el socket y detener la ejecución
        socket_close($sock);
        exit();  // Terminar la ejecución para evitar la descarga
    }

    // Separar encabezado y cuerpo de la respuesta
    $partes = explode("\r\n\r\n", $respuesta, 2);
    $encabezado = $partes[0];
    $contenido = isset($partes[1]) ? $partes[1] : '';



    // Guardar el contenido en un archivo
    file_put_contents($output_file, $contenido);
    echo "Archivo descargado como: $output_file\n";

    // Cerrar el socket
    socket_close($sock);
}
