<?php

$ip = "127.0.0.10";
$port = 8888;

$server_ip = "127.0.0.1";
$server_port = 8888;

// get download directory
$download_directory = "users/" . basename(__DIR__) . "/download/";
$shared_directory = "users/" . basename(__DIR__) . "/shared/";

// crear el socket del cliente en la $ip y $puerto
$sock = socket_create(AF_INET, SOCK_STREAM, getprotobyname('tcp'));
if ($sock === false) {
    die("Error creating the socket: " . socket_strerror(socket_last_error()));
} else {
    echo "Socket created\n";
}

if (socket_bind($sock, $ip, $port) === false) {
    die("Error al asociar el socket: " . socket_strerror(socket_last_error($sock)));
}

// conectar al servidor central
if (socket_connect($sock, $server_ip, $server_port) === false) {
    die("Error connecting to the server: " . socket_strerror(socket_last_error($sock)));
} else {
    echo "Connected to the server\n";
}

function send_shared_files($shared_directory, $ip, $sock, $server_ip, $server_port)
{
    // get files from the shared folder
    $files = scandir($shared_directory);
    foreach ($files as $file) {
        if ($file == "." || $file == "..") {
            continue;
        }
        $shared_files[] = $file;
    }

    // get json object of $shared files with name files
    $shared_files = ["files" => $shared_files];
    $json_shared_files = json_encode($shared_files);

    // send the file to the server
    $request = "PUT /hosts/$ip HTTP/1.1\r\n
                Host: $server_ip:$server_port\r\n
                Content-Type: application/json\r\n
                Content-Length: " . strlen($json_shared_files) . "\r\n\r\n
                $json_shared_files";
    socket_write($sock, $request, strlen($request));
    echo $request;
}


// hilo para enviar los ficheros en segundo plano cada 10 seg
$pid = pcntl_fork();
if ($pid == -1) {
    die("Error forking...\n");
} else if ($pid == 0) {
    while (true) {
        send_shared_files($shared_directory, $ip, $sock, $server_ip, $server_port);
        sleep(10);
    }
} else {
    // leer la respuesta del servidor
    $response = socket_read($sock, 1024);
    echo $response;
}