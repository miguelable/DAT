<?php

// import error functions
define('LOG_ON', true);
require_once realpath(__DIR__ . "/logs.php");

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
        // Crear un nuevo proceso para manejar al cliente
        $pid = pcntl_fork();
        if ($pid == -1) {
            log_error("Error al crear el proceso hijo.");
            socket_close($client);
        } elseif ($pid == 0) {
            // Proceso hijo: manejar el cliente
            handle_client($client, $clients_list);
            exit; // Terminar el proceso hijo
        } else {
            // Proceso padre: cerrar el socket del cliente en el padre
            socket_close($client);
        }
    }
}

function handle_client($client, $clients_list)
{
    // Leer la petición del cliente
    while (true) {
        $request = socket_read($client, 1024);
        if ($request === false) {
            log_error("Error al leer la petición del cliente: " . socket_strerror(socket_last_error($client)));
            break; // Salir del bucle si hay un error
        }
        $data = explode(' ', $request);

        // Verificar el método HTTP (GET o PUT)
        if ($data[0] == 'GET') {
            // Caso 1: GET /peers/nombreArchivo
            if (preg_match('/GET \/peers\/([^\s]+)/', $request, $matches)) {
                $nombreArchivo = $matches[1];
                echo "Solicitud GET para peers con el archivo: $nombreArchivo\n";

                // Buscar los clientes que tienen el archivo solicitado
                $peersConArchivo = array_filter($clients_list, function ($client) use ($nombreArchivo) {
                    return in_array($nombreArchivo, $client->files);
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
                // Caso 2: GET /search/trozoNombreArchivo
            } elseif (preg_match('/GET \/search\/([^\s]+)/', $request, $matches)) {
                $trozoNombreArchivo = $matches[1];
                echo "Solicitud GET para search con el fragmento del nombre del archivo: $trozoNombreArchivo\n";

                // Array para almacenar los resultados de la búsqueda
                $resultados = [];

                // Buscar en cada cliente
                foreach ($clients_list as $peer) {
                    foreach ($peer->files as $file) {
                        // Verificar si el fragmento está contenido en el nombre del archivo
                        if (strpos($file, $trozoNombreArchivo) !== false) {
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
                    echo "Resultados de la búsqueda:\n";
                    foreach ($resultados as $resultado) {
                        echo "Cliente con IP: " . $resultado['ip'] . " tiene el archivo: " . $resultado['archivo'] . "\n";
                    }
                } else {
                    echo "No se encontraron archivos que coincidan con el fragmento: $trozoNombreArchivo\n";
                }
            } else {
                echo "Ruta GET desconocida.\n";
            }
        }
        if ($data[0] == 'PUT') {
            // Caso 3: PUT /host/XXXX:PPP
            // Utilizar expresión regular para extraer la IP
            if (preg_match('/\/hosts\/([\d\.]+)/', $data[1], $matches)) {
                $ip_client = $matches[1]; // La IP extraída
            }

            // Buscar la el json en $data[5]
            $json_content = substr($data[5], strpos($data[5], '{'));

            // Decodificar el JSON
            $data1 = json_decode($json_content, true);
            // Verificar si la decodificación fue exitosa
            if (json_last_error() === JSON_ERROR_NONE) {
                $files = $data1['files']; // Acceder a la lista de archivos
                // print_r($files); // Imprimir el contenido de files
            } else {
                echo "Error al decodificar el JSON: " . json_last_error_msg();
            }

            update_files($ip_client,  $clients_list, $files);
        } else {
            echo "Método HTTP no soportado.\n";
        }

        log_verbose("Petición: $request\n");

        // Devolver el array de clientes conecatdos
        $response = "Clientes conectados: \n";
        foreach ($clients_list as $c) {
            $response .= $c->ip . "\n";
        }
        log_verbose($response);
        if (socket_write($client, $response, strlen($response)) === false) {
            log_error("Error escribiendo en el socket: " . socket_strerror(socket_last_error($client)));
            break; // Salir si hay un error al escribir
        }
    }
    // Cerrar la conexión al cliente
    log_info("Cerrando conexión con el cliente.");
    socket_close($client);
}

function update_files($ip_client, $clients_list, $files)
{
    $client_found = false; // Indicador para verificar si encontramos al cliente
    foreach ($clients_list as $client) {
        if ($client->ip === $ip_client) {
            // Actualizar la lista de archivos del cliente
            $client->files = $files;
            // print_r($clients_list); // Imprimir los detalles del cliente actualizado
            log_info("Archivos actualizados para el cliente con IP: $ip_client");
            $client_found = true; // Cliente encontrado y actualizado
            break; // Salimos del loop porque ya encontramos al cliente
        }
    }

    // Si no se encontró el cliente, registramos una advertencia
    if (!$client_found) {
        log_warning("No se encontró un cliente con la IP: $ip_client");
    }

    return $clients_list; // Devolver siempre la lista de clientes
}

function create_new_client($client, $clients_list)
{
    // get the ip of the client
    $client_ip = "";
    $client_files = "";
    socket_getpeername($client, $client_ip);

    // check if the ip is in the clients array
    foreach ($clients_list as $c) {
        if ($c->ip == $client_ip) {
            //log_warning("Client $client_ip already exists");
            return $clients_list;
        }
    }
    // create a new client with the ip
    $new_client = new Client();
    $new_client->ip = $client_ip;
    $new_client->files = $client_files;

    $index = substr($client_ip, -1); // Obtener el último carácter

    // add the client to the clients array
    $clients_list[$index] = $new_client;
    log_info("New client connected with ip: $client_ip");
    return $clients_list; // return the updated clients array
}

// En $clients_list cada nuevo cliente se almacena con el index del último valor de su ip
// tengo que hacer que no salga esto log_warning("Client $client_ip already exists"); todo el rato