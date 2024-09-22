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