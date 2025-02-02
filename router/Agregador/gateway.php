<?php

// Dirección y puerto en los que el servidor escuchará
$host = '0.0.0.0'; // Escucha en todas las interfaces de red
$port = 54471;     // Puerto de escucha

// URL base del servidor remoto (sin el parámetro `type`)
$remoteBaseUrl = 'http://webalumnos.tlm.unavarra.es:10303/php/manage_data.php';

// Crear el socket
$server = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
if (!$server) {
    die("Error al crear el socket: " . socket_strerror(socket_last_error()) . "\n");
} {
    echo "Socket creado...\n";
}

// Enlazar el socket al host y puerto
if (!socket_bind($server, $host, $port)) {
    die("Error al enlazar el socket: " . socket_strerror(socket_last_error()) . "\n");
} else {
    echo "Enlazado al puerto $port...\n";
}

// Escuchar conexiones entrantes
if (!socket_listen($server)) {
    die("Error al escuchar en el puerto: " . socket_strerror(socket_last_error()) . "\n");
} else {
    echo "Servidor escuchando en el puerto $port...\n";
}

// Función para enviar datos JSON al servidor remoto
function enviarAlServidorRemoto($jsonData)
{
    global $remoteBaseUrl;

    // Inicializar cURL para enviar los datos JSON
    $ch = curl_init($remoteBaseUrl);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
    curl_setopt($ch, CURLOPT_HTTPHEADER, ['Content-Type: application/json']);
    curl_setopt($ch, CURLOPT_POST, true);
    curl_setopt($ch, CURLOPT_POSTFIELDS, $jsonData);

    // Ejecutar la solicitud y obtener la respuesta
    $response = curl_exec($ch);

    if (curl_errno($ch)) {
        echo "Error al enviar datos al servidor remoto: " . curl_error($ch) . "\n";
        curl_close($ch);
        return false;
    } else {
        echo "Datos enviados al servidor remoto. Respuesta: $response\n";
        curl_close($ch);
        return true;
    }
}

// Función para manejar la conexión con un cliente
// function handle_client($client)
// {
//     while (true) {
//         // Leer la solicitud del cliente
//         $request = socket_read($client, 1024);

//         if ($request) {
//             echo "Mensaje recibido: $request\n";
//             // Enviar el fragmento tal cual al servidor remoto
//             enviarAlServidorRemoto($request);
//         }
//     }
// }


// Recibir fragmentos de la imagen
function handle_client($client)
{
    $buffer = '';
    while (true) {
        $data = socket_read($client, 8192);
        if ($data) {
            $buffer .= $data;  // Acumular los fragmentos

            // Verificar si un fragmento está completo
            while (($pos = strpos($buffer, '}')) !== false) {
                $fragment = substr($buffer, 0, $pos + 1);  // Obtener el fragmento completo
                $buffer = substr($buffer, $pos + 1);  // Restar el fragmento recibido
                enviarAlServidorRemoto($fragment);  // Enviar el fragmento al servidor remoto
            }
        }
    }
}




while (true) {
    // Aceptar conexiones entrantes
    if (($client = socket_accept($server)) !== false) {
        // Crear un nuevo proceso para manejar al cliente
        $pid = pcntl_fork();
        if ($pid == -1) {
            die("Error al bifurcar el proceso\n");
        } elseif ($pid) {
            // Código del proceso padre
            echo "Proceso padre con PID: " . posix_getpid() . "\n";
        } else {
            // Código del proceso hijo
            echo "Proceso hijo con PID: " . posix_getpid() . "\n";
            handle_client($client);
            exit(0); // Importante: cerrar el proceso hijo
        }
    }
}

// Cerrar el socket del servidor al finalizar (aunque nunca se alcanzará)
socket_close($server);
echo "Servidor cerrado.\n";