<?php

// import error functions
require_once "logs.php";

$server_ip = "127.0.0.1";
$server_port = 8888;

// clase con la info de los clientes
class Client
{
    public $ip;
    public $files = [];
}

// array global de clientes
$clients_list = []; // array de clientes 

// Crear un socket
$sock = socket_create(AF_INET, SOCK_STREAM, getprotobyname('tcp'));
if ($sock === false) {
    die("Error al crear el socket: " . socket_strerror(socket_last_error()));
}

// Asociar el socket a una dirección/puerto
if (socket_bind($sock, $server_ip, $server_port) === false) {
    die("Error al asociar el socket: " . socket_strerror(socket_last_error($sock)));
}

// Escuchar conexiones
if (socket_listen($sock, 10) === false) {
    die("Error al escuchar en el socket: " . socket_strerror(socket_last_error($sock)));
}

log_info("Servidor escuchando en $server_ip:$server_port");

while (true) {
    // Aceptar conexiones entrantes
    if (($client = socket_accept($sock)) !== false) {
        // create a new client
        $clients_list = create_new_client($client, $clients_list);
        // handle the client
        handle_client($client, $clients_list);
    }
}

function handle_client($client, $clients)
{
    // Leer la petición del cliente
    $request = socket_read($client, 1024);
    log_verbose("Petición: $request");

    // Devolver el array de clientes conecatdos
    $response = "Clientes conectados: \n";
    foreach ($clients as $c) {
        $response .= $c->ip . "\n";
    }
    log_verbose($response);
    socket_write($client, $response, strlen($response));

    // Cerrar la conexión
    socket_close($client);
}

function create_new_client($client, $clients)
{
    // get the ip of the client
    $client_ip = "";
    socket_getpeername($client, $client_ip);
    // check if the ip is in the clients array
    foreach ($clients as $c) {
        if ($c->ip == $client_ip) {
            log_warning("Client $client_ip already exists");
            return $clients;
        }
    }
    // create a new client with the ip
    $new_client = new Client();
    $new_client->ip = $client_ip;
    // add the client to the clients array
    $clients[] = $new_client;
    log_info("New client connected with ip: $client_ip");
    return $clients; // return the updated clients array
}