<?php

$ip = '127.0.0.1';
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
    $download = false;
    // Leer la solicitud del cliente (navegador)
    $request = '';
    while ($chunk = socket_read($client, 1024)) {
        $request .= $chunk;
        if (strpos($request, "\r\n\r\n") !== false) {
            break; // Fin de los encabezados HTTP
        }
    }

    echo "Solicitud recibida:\n$request\n";

    // Separar la cabecera por los espacios
    $args = explode(" ", $request);

    // Extraer la URL
    $url = $args[1];
    if ($url === '/') {
        $file_content = get_main_page(); // Si no se especifica una URL, servir un archivo por defecto
    } else {
        $url = substr($url, 1); // Remover la barra inicial "/"
        if (strpos($url, '/&download=true') !== false) {
            $url = str_replace('/&download=true', '', $url);
            // si es true entonces flag de descarga a true
            $download = true;
        }
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

    // Construir la respuesta HTTP 
    $response = "HTTP/1.1 200 OK\r\n";
    $response .= get_content_type($url);
    if ($download === true)
        $response .= "Content-Disposition: attachment; filename=\"$url\"\r\n";
    $content_length = strlen($file_content);
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
    $files = scandir(__DIR__);
    $html = <<<HTML
    <!DOCTYPE html>
    <html>
        <head>
            <title>Servidor Web</title>
            <link rel="stylesheet" href="style.css">
        </head>
        <body>
            <h1>Archivos en el directorio</h1>
            <h3>Pulsa para ver o descargar un archivo</h3>
            <ul>
HTML;
    foreach ($files as $file) {
        if (!is_file($file) || in_array($file, ['.', '..']) || strpos($file, '.php') || strpos($file, '.git') !== false) {
            continue;
        }
        $html .= "<li>$file <a href=\"$file\">Ver</a> <a href=\"$file&download=true\">Descargar</a></li>";
    }
    $html .= <<<HTML
            </ul>
        </body>
    </html>
HTML;
    return $html;
}
