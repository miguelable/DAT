<?php

// import error functions
define('LOG_ON', true);
require_once realpath(__DIR__ . "/logs.php");

$server_ip = "127.0.0.1";
$server_port = 8888;

// clase con la info de los clientes
class Client
{
    // constructor to set ip and file
    public function __construct($ip, $files)
    {
        $this->ip = $ip;
        $this->files = $files;
    }
    public $ip;
    public $files = [];
}

// Crear un segmento de memoria compartida
$shm_key = ftok(__FILE__, 't');
$shm_id = shmop_open($shm_key, "c", 0644, 1024);
if (!$shm_id) {
    die("No se pudo crear el segmento de memoria compartida\n");
}

// Inicializar el array de clientes en la memoria compartida
$clients_list = [];
shmop_write($shm_id, serialize($clients_list), 0);

// Crear un socket
$sock = @socket_create(AF_INET, SOCK_STREAM, getprotobyname('tcp'));
if ($sock === false) {
    log_error("Error al crear el socket: " . socket_strerror(socket_last_error()));
}

// Asociar el socket a una dirección/puerto
if (@socket_bind($sock, $server_ip, $server_port) === false) {
    log_error("Error al asociar el socket: " . socket_strerror(socket_last_error($sock)));
}

// Escuchar conexiones
if (@socket_listen($sock, 10) === false) {
    log_error("Error al escuchar en el socket: " . socket_strerror(socket_last_error($sock)));
}

log_info("Servidor escuchando en $server_ip:$server_port");

while (true) {
    // Aceptar conexiones entrantes
    if (($client = socket_accept($sock)) !== false) {
        // Crear un nuevo proceso para manejar al cliente
        $pid = pcntl_fork();
        if ($pid == -1) {
            log_error("Error al crear el proceso hijo.");
            socket_close($client);
        } elseif ($pid == 0) {
            // Proceso hijo: manejar el cliente
            handle_client($client, $shm_id);
            exit; // Terminar el proceso hijo
        } else {
            // Proceso padre: cerrar el socket del cliente en el padre
            socket_close($client);
        }
    }
}

// Función para el manejo del cliente
function handle_client($client, $shm_id)
{
    // Leer la petición del cliente
    while (true) {
        $request = socket_read($client, 1024);
        log_verbose("Petición: $request\n");
        if ($request === false) {
            log_error("Error al leer la petición del cliente: " . socket_strerror(socket_last_error($client)));
            break; // Salir del bucle si hay un error
        }
        $data = explode(' ', $request);
        // comprobar si el array está vacío
        if (count(array_filter($data)) === 0) {
            break;
        }
        $method = $data[0];
        // separar los datos por / en comando y info
        $parts = explode('/', $data[1]);
        print_r($parts);
        $command = $parts[1] ?? null;
        $info = $parts[2] ?? null;

        switch ($method) {
            case 'PUT':
                $data_files = explode("\r\n\r\n", $request)[1];
                manage_client_files($info, $data_files, $shm_id);
                break;
            case 'GET':
                print_r($command);
                switch ($command) {
                    case 'hosts':
                        get_hosts_method($client, $shm_id);
                    case 'search':
                        get_search_method($info, $client, $shm_id);
                        break;
                    case 'peers':
                        get_peers_method($info, $client, $shm_id);
                        break;
                }
                break;
            default:
                log_error("Método no soportado: $method");
                break;
        }
    }

    // Cerrar la conexión al cliente
    log_info("Cerrando conexión con el cliente.");
    socket_close($client);
}

// Función para el método GET/hosts
function get_hosts_method($client, $shm_id)
{
    $clients_list = unserialize(shmop_read($shm_id, 0, shmop_size($shm_id)));
    // Devolver el array de clientes conecatdos
    $response = "Clientes conectados: \n";
    foreach ($clients_list as $c) {
        $response .= $c->ip . "\n";
    }
    send_response_to_client($client, $response);
}

// Función para el método GET/peers
function get_peers_method($file_name, $client, $shm_id)
{
    $clients_list = unserialize(shmop_read($shm_id, 0, shmop_size($shm_id)));
    // Buscar los clientes que tienen el archivo solicitado
    $peersConArchivo = array_filter($clients_list, function ($client) use ($file_name) {
        return in_array($file_name, $client->files);
    });

    // Seleccionar hasta 5 peers de manera aleatoria
    $peersAleatorios = array_rand($peersConArchivo, min(5, count($peersConArchivo)));

    // Asegurarse de que $peersAleatorios sea un array (si solo hay uno, array_rand devuelve una sola clave)
    if (!is_array($peersAleatorios)) {
        $peersAleatorios = [$peersAleatorios];
    }

    // Devolver los peers seleccionados
    foreach ($peersAleatorios as $key) {
        $peer = $clients_list[$key];
        echo "Peer con IP: " . $peer->ip . "\n";
    }
}

// Función para el método GET/search
function get_search_method($file_name, $client, $shm_id)
{
    $clients_list = unserialize(shmop_read($shm_id, 0, shmop_size($shm_id)));
    // Array para almacenar los resultados de la búsqueda
    $resultados = [];
    // Buscar en cada cliente
    foreach ($clients_list as $peer) {
        foreach ($peer->files as $file) {
            // Verificar si el fragmento está contenido en el nombre del archivo
            if (strpos($file, $file_name) !== false) {
                // Si el archivo coincide, añadir el cliente y el archivo a los resultados
                $resultados[] = [
                    'ip' => $peer->ip,
                    'archivo' => $file
                ];
            }
        }
    }
    // Mostrar los resultados de la búsqueda
    if (!empty($resultados)) {
        foreach ($resultados as $resultado) {
            $response = "Cliente con IP " . $resultado['ip'] . " tiene el archivo: " . $resultado['archivo'] . "\n";
        }
    } else {
        $response = "No se encontraron archivos que coincidan con el fragmento: $file_name\n";
    }
    send_response_to_client($client, $response);
}

// Función para enviar la respuesta al cliente
function send_response_to_client($client, $response)
{
    // Intentar escribir la respuesta en el socket del cliente
    if (!@socket_write($client, $response, strlen($response))) {
        return handle_socket_error($client, "Error escribiendo en el socket del cliente");
    }
    
    // Si la escritura es exitosa, devolver true
    return true;
}

// Función para manejar errores de socket
function handle_socket_error($socket, $message)
{
    log_error("$message: " . socket_strerror(socket_last_error($socket)));
    return false;
}

// Función para el método PUT
function manage_client_files($client_ip, $file_json, $shm_id)
{
    $clients_list = unserialize(shmop_read($shm_id, 0, shmop_size($shm_id)));
    // Decodificar JSON
    $client_files = decode_json($file_json);
    if ($client_files === null) {
        echo "Error decoding json: " . json_last_error_msg();
        return;
    }

    // Manejar la lista de clientes
    if (empty($clients_list)) {
        add_new_client($client_ip, $client_files, $shm_id);
    } else {
        update_or_add_client($client_ip, $client_files, $shm_id);
    }
}

// Función para la decodificación del json
function decode_json($file_json)
{
    $json = json_decode($file_json, true);
    if (json_last_error() === JSON_ERROR_NONE) {
        return $json['files'];
    }
    return null;
}

// Función para crear un nuevo cliente
function add_new_client($client_ip, $client_files, $shm_id)
{
    $clients_list = unserialize(shmop_read($shm_id, 0, shmop_size($shm_id)));
    $new_client = new Client($client_ip, $client_files);
    array_push($clients_list, $new_client);
    shmop_write($shm_id, serialize($clients_list), 0);
    log_info("New client connected with IP: $client_ip");

    print_r($clients_list);
}

// Función para actualizar o añadir un cliente
function update_or_add_client($client_ip, $client_files, $shm_id)
{
    $clients_list = unserialize(shmop_read($shm_id, 0, shmop_size($shm_id)));
    $client_found = false;
    foreach ($clients_list as $client) {
        if ($client->ip === $client_ip) {
            $client->files = $client_files;
            log_info("Files updated for client with IP: $client_ip");
            print_r($clients_list);
            $client_found = true;
            break;
        }
    }

    if (!$client_found) {
        log_warning("No client found with IP: $client_ip");
        add_new_client($client_ip, $client_files, $shm_id);
    } else {
        shmop_write($shm_id, serialize($clients_list), 0);
    }
}