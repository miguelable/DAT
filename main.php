<?php

$ip = '1.2.3.4';
$puerto = 80;

// Crear un socket
$sock = socket_create(AF_INET, SOCK_STREAM, getprotobyname('tcp'));
if ($sock === false) {
    die("Error al crear el socket: " . socket_strerror(socket_last_error()));
}

// Asociar el socket a una dirección/puerto
if (socket_bind($sock, $ip, $puerto) === false) {
    die("Error al asociar el socket: " . socket_strerror(socket_last_error($sock)));
}

// Escuchar conexiones
if (socket_listen($sock, 10) === false) {
    die("Error al escuchar en el socket: " . socket_strerror(socket_last_error($sock)));
}

echo "Servidor escuchando en $ip:$puerto";

while (true) {
    // Aceptar conexiones entrantes
    if (($client = socket_accept($sock)) !== false) {
        echo "Cliente conectado\n";
        handle_client($client);
    }
}

function handle_client($client)
{
    // Leer la solicitud del cliente (navegador)
    $request = '';
    while ($chunk = socket_read($client, 1024)) {
        $request .= $chunk;
        if (strpos($request, "\r\n\r\n") !== false) {
            break; // Fin de los encabezados HTTP
        }
    }

    echo "Solicitud recibida:\n$request\n";

    // Extraer los argumentos del GET
    $args = explode(" ", $request);
    if (count($args) < 2) {
        socket_close($client);
        return;
    }

    // Extraer la URL
    $url = $args[1];
    $url = substr($url, 1); // Remover la barra inicial "/"
    if ($url === '') {
        $file_content = get_main_page(); // Si no se especifica una URL, servir un archivo por defecto
    } else {
        // si el fichero no existe devolver error
        if (!file_exists($url)) {
            $file_content = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\nConnection: close\r\n\r\n";
            socket_write($client, $file_content);
            socket_close($client);
            return;
        }
        // Leer el contenido del archivo
        $file_content = file_get_contents($url);
    }

    // Leer el tamaño del archivo para meterlo a los headers de http
    $content_length = strlen($file_content);

    // Construir la respuesta HTTP 
    $response = "HTTP/1.1 200 OK\r\n";
    if (strpos($url, '.ico') !== false) {
        $response .= "Content-Type: image/x-icon\r\n";
    } else if (strpos($url, '.html') !== false) {
        $response .= "Content-Type: text/html\r\n";
    } else if (strpos($url, '.md') !== false) {
        $response .= "Content-Type: text/markdown\r\n";
    } else {
        $response .= "Content-Type: text/html\r\n";
    }
    $response .= "Content-Length: $content_length\r\n";
    $response .= "Connection: close\r\n\r\n";
    $response .= $file_content;

    // Escribir la respuesta al socket
    socket_write($client, $response);
    // Cerrar el socket del cliente
    socket_close($client);
}


function get_main_page()
{
    // Devuelve un html con un listado de los ficheros que tiene la carpeta
    $files = scandir(__DIR__);
    $html =
        "<!DOCTYPE html>
    <html>
        <head>
            <title>
                Servidor Web
            </title>
        </head>
        <body>
            <h1>Archivos en el directorio</h1>
            <ul>";
    foreach ($files as $file) {
        if ($file === '.' || $file === '..' || strpos($file, '.php') !== false) {
            continue;
        }
        $html .= "<li><a href=\"$file\">$file</a></li>";
    }
    $html .= "</ul>
        </body> 
    </html>";
    return $html;
}