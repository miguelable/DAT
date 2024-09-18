<?php

// el cliente pregunta a la ip 127.0.0.1 y puerto 80 para que le devuelva la pagina por defecto
$ip = '127.0.0.1';
$puerto = 80;
// pagina por defecto vacía
$pagina = '';
// enviar solicitud al servidor
$sock = socket_create(AF_INET, SOCK_STREAM, getprotobyname('tcp'));
if ($sock === false) {
    die("Error al crear el socket: " . socket_strerror(socket_last_error()));
}
// conectar al servidor
if (socket_connect($sock, $ip, $puerto) === false) {
    die("Error al conectar al servidor: " . socket_strerror(socket_last_error($sock)));
}
// enviar solicitud al servidor
$mensaje = "GET / HTTP/1.1\r\nHost: $ip:$puerto\r\n\r\n";
if (socket_write($sock, $mensaje, strlen($mensaje)) === false) {
    die("Error al enviar la solicitud: " . socket_strerror(socket_last_error($sock)));
}
// recibir respuesta del servidor
while ($chunk = socket_read($sock, 1024)) {
    $pagina .= $chunk;
}
echo "Respuesta del servidor:\n$pagina\n";
// extraer el contenido de la respuesta
$pagina = explode("\r\n\r\n", $pagina)[1];
// guardar la página en un archivo
file_put_contents('pagina.html', $pagina);
// cerrar el socket
socket_close($sock);
