<?php

// si se declara LOG_OK, si no no incluir logs
define('LOG_ON', true);
require_once realpath(__DIR__ . "/logs.php");


// Datos de configuración del cliente y el servidor
$ip = "127.0.0.10";
$port = 8888;
$server_ip = "127.0.0.1";
$server_port = 8888;

// get download directory
$download_directory = realpath(__DIR__ . "/download/");
$shared_directory = realpath(__DIR__ . "/shared/");

// Inicializar el socket
$sock_server = create_socket($ip, $port, $server_ip, $server_port, false);

// Constantes para mensajes de error
define('ERROR_FORK_1', "Error creando el proceso 1\n");
define('ERROR_FORK_2', "Error creando el proceso 2\n");
define('ERROR_FORK_3', "Error creando el proceso 3\n");

// Crear procesos hijos
$pid1 = create_child_process('task_file_sending_loop', ERROR_FORK_1);
if ($pid1 != -1) {
    $pid2 = create_child_process('task_terminal_loop', ERROR_FORK_2);
    if ($pid2 != -1) {
        $pid3 = create_child_process('task_client_requests', ERROR_FORK_3);
        if ($pid3 != -1) {
            // Proceso padre: esperar a que los 3 hijos terminen
            pcntl_waitpid($pid1, $status);
            pcntl_waitpid($pid2, $status);
            pcntl_waitpid($pid3, $status);
            log_info("Todos los procesos han terminado");
        }
    }
}

// Función para crear un proceso hijo
function create_child_process($task, $error_message)
{
    $pid = pcntl_fork();
    if ($pid == -1) {
        log_error($error_message);
        return -1;
    } else if ($pid == 0) {
        $task();
        exit(0); // Asegurarse de que el proceso hijo termine
    }
    return $pid;
}

// Tareas de los procesos hijos
function task_file_sending_loop()
{
    file_sending_loop();
}

// Tarea de la terminal del cliente
function task_terminal_loop()
{
    terminal_loop();
}

// Tarea aceptar peticiones entrantes
function task_client_requests()
{
    client_loop();
}

// Función para crear y conectar el socket o escuchar
function create_socket($ip, $port, $server_ip, $server_port, $isReadable)
{
    $socket = @socket_create(AF_INET, SOCK_STREAM, getprotobyname('tcp'));
    if ($socket === false) {
        log_error("Error creando el socket: " . socket_strerror(socket_last_error()));
        return false;
    }
    if ($isReadable === true) {
        if (@socket_bind($socket, $ip, $port) === false) {
            log_error("Error asociando el socket: " . socket_strerror(socket_last_error($socket)));
            socket_close($socket);
            return false;
        }
        if (@socket_listen($socket, 10) === false) {
            die("Error al escuchar en el socket: " . socket_strerror(socket_last_error($socket)));
        }
    } else {
        if (@socket_connect($socket,  $server_ip, $server_port) === false) {
            log_error("Error conectando al servidor: " . socket_strerror(socket_last_error($socket)));
            socket_close($socket);
            return false;
        }
    }
    return $socket;
}

// function para crear socket sin necesidad de meter server ip y puerto
function create_socket_r($ip, $port, $isReadable)
{
    return create_socket($ip, $port, "", "", $isReadable);
}

// Función para manejar errores de socket
function handle_socket_error($socket, $message)
{
    log_error("$message: " . socket_strerror(socket_last_error($socket)));
    return false;
}

// Función para enviar solicitudes HTTP y leer respuestas
function send_http_request($socket, $request)
{
    if (!@socket_write($socket, $request, strlen($request))) {
        return handle_socket_error($socket, "Error escribiendo en el socket");
    }
    return read_server_response($socket);
}

// Función para obtener los archivos que se comparten
function get_shared_files()
{
    $shared_directory = $GLOBALS['shared_directory'];
    if (!file_exists($shared_directory)) {
        log_error("Directorio compartido no encontrado: $shared_directory");
        return [];
    }
    $files = scandir($shared_directory);
    $shared_files = array_filter($files, function ($file) {
        return $file !== "." && $file !== "..";
    });

    if (empty($shared_files))
        log_warning("No hay archivos para compartir");
    return array_values($shared_files);
}

// Función para enviar los archivos que se comparten   
function send_shared_files($shared_files)
{
    if (empty($shared_files))
        return;

    $ip = $GLOBALS['ip'];
    $server_ip = $GLOBALS['server_ip'];
    $server_port = $GLOBALS['server_port'];
    $sock = $GLOBALS['sock_server'];

    // Crear el JSON con los nombres de los archivos
    $shared_files_data = json_encode(["files" => $shared_files]);
    // Preparar la solicitud HTTP
    $request = "PUT /hosts/$ip HTTP/1.1\r\n" .
        "Host: $server_ip:$server_port\r\n" .
        "Content-Type: application/json\r\n" .
        "Content-Length: " . strlen($shared_files_data) . "\r\n\r\n" .
        $shared_files_data;
    // Intentar enviar la solicitud
    $response = send_http_request($sock, $request);
    if ($response === false) {
        return false;
    }
    return true;
}

// Función para ejecutar el bucle de envío de archivos compartidos
function file_sending_loop()
{
    $sock = $GLOBALS['sock_server'];
    while (true) {
        $shared_files = get_shared_files();
        if (!send_shared_files($shared_files)) {
            log_warning("Intentando reconectar...");
            socket_close($sock);
            $sock = create_socket(
                $GLOBALS['ip'],
                $GLOBALS['port'],
                $GLOBALS['server_ip'],
                $GLOBALS['server_port'],
                false
            );
        }
        sleep(10);
    }
}

// Función para leer las órdenes del usuario por terminal
function terminal_loop()
{
    while (true) {
        echo "> ";
        $order = trim(fgets(STDIN));
        if (empty($order))
            continue;
        $order = explode(" ", $order);
        switch ($order[0]) {
            case "?":
                show_available_commands();
                break;
            case "exit":
                log_info("Saliendo del programa...");
                $GLOBALS['pid1'] = $GLOBALS['pid1'] ?? null;
                $GLOBALS['pid2'] = $GLOBALS['pid2'] ?? null;
                $GLOBALS['pid3'] = $GLOBALS['pid3'] ?? null;
                posix_kill($GLOBALS['pid1'], SIGTERM);
                posix_kill($GLOBALS['pid2'], SIGTERM);
                posix_kill($GLOBALS['pid3'], SIGTERM);
                exit;
            case "search":
                if (count($order) < 2 || empty($order[1]) || $order[1] === "?") {
                    log_debug("Use: search <file>");
                    break;
                }
                echo "Searching file: $order[1]\n";
                searchFile($order[1]);
                break;
            case "download":
                if (count($order) < 2 || empty($order[1]) || $order[1] === "?") {
                    log_debug("Use: download <file>");
                    break;
                }
                // Lógica para descargar archivos (implementación pendiente)
                downloadFile($order[1]);
                break;
            case "hosts":
                // Lógica para listar hosts (implementación pendiente)
                getHosts();
                break;
            case "host_files":
                if (count($order) < 2 || empty($order[1]) || $order[1] === "?") {
                    log_debug("Use: host_files <ip> <port>");
                    break;
                }
                if (count($order) > 2 && !empty($order[2])) {
                    getHostFiles($order[1], $order[2]);
                    break;
                }
                getHostFiles($order[1], $GLOBALS['server_port']);
                break;
            default:
                log_warning("Comando no reconocido: " . $order[0]);
                break;
        }
    }
}

// Función para buscar archivos
function searchFile($args)
{
    $server_ip = $GLOBALS['server_ip'];
    $server_port = $GLOBALS['server_port'];
    $sock = $GLOBALS['sock_server'];
    // Crear la solicitud HTTP
    $request = "GET /search/$args HTTP/1.1\r\n" .
        // host variables globales
        "Host: $server_ip:$server_port\r\n" .
        "Content-Type: application/json\r\n" .
        "Content-Length: 0\r\n\r\n";

    // Enviar la solicitud
    $response = send_http_request($sock, $request);
    if ($response === false) {
        return false;
    }
    log_debug("Respuesta del servidor:\n$response\n");
    return true;
}

// Función para mostrar los comandos disponibles
function show_available_commands()
{
    echo "Comandos disponibles:\n";
    echo "search \t\t <arg> \t\t Buscar todos los archivos que contienen arg\n";
    echo "download \t <file> \t Descargar un archivo\n";
    echo "host_files \t <ip> \t\t Obtener la lista de archivos de un host\n";
    echo "hosts\t\t\t\t Obtener la lista de hosts\n";
    echo "exit \t\t\t\t Salir del programa\n";
}

// Función para leer la respuesta del servidor
function read_server_response($sock)
{
    $response = socket_read($sock, 1024);
    if ($response === false) {
        log_error("Error al leer la respuesta del servidor: " . socket_strerror(socket_last_error($sock)));
        return false;
    }
    return $response;
}

// Función para manejar las peticiones de los clientes
function client_loop()
{
    // crear un socket de lectura
    $sock_client = create_socket_r($GLOBALS['ip'], $GLOBALS['port'], true);
    while (true) {
        // Aceptar conexiones entrantes
        if (($extern_client = socket_accept($sock_client)) !== false) {
            $pid = pcntl_fork();
            if ($pid == -1) {
                log_error("Error al crear el proceso cliente");
                socket_close($extern_client);
            } elseif ($pid == 0) {
                handle_client($extern_client);
                exit;
            } else {
                // Proceso padre: cerrar el socket del cliente en el padre
                socket_close($extern_client);
            }
        }
    }
}

// Función para manejar las peticiones de los clientes
function handle_client($client)
{
    // Leer la petición del cliente
    while (true) {
        $request = socket_read($client, 1024);
        if ($request === false) {
            log_error("Error al leer la petición del cliente: " . socket_strerror(socket_last_error($client)));
            break;
        }
        $data = explode(' ', $request);
        // comprobar si el array está vacío
        if (count(array_filter($data)) === 0) {
            break;
        }
        $file = explode('/', $data[1])[2];
        // comprobar si el fichero está en la carpeta para compartir
        $shared_files = get_shared_files();
        if (!in_array($file, $shared_files)) {
            log_warning("El archivo $file no está disponible para compartir");
            // enviar un mensaje de error http
            $response = "HTTP/1.1 404 Not Found\r\n" .
                "Content-Type: application/json\r\n" .
                "Content-Length: 0\r\n\r\n";
            // Enviar respuesta al cliente
            $response = send_http_request($client, $response);
            if ($response === false) {
                return false;
            }
        } else {
            // enviar el fichero
            $file_path = $GLOBALS['shared_directory'] . "/$file";
            $file_content = file_get_contents($file_path);
            $response = "HTTP/1.1 200 OK\r\n" .
                "Content-Type: application/json\r\n" .
                "Content-Length: " . strlen($file_content) . "\r\n\r\n" .
                $file_content;
            // Enviar respuesta al cliente
            $response = send_http_request($client, $response);
            log_info("Compartido el fichero $file");
            if ($response === false) {
                return false;
            }
        }
    }
}

// Función para obtener la lista de hosts
function getHosts()
{
    $server_ip = $GLOBALS['server_ip'];
    $server_port = $GLOBALS['server_port'];
    $sock = $GLOBALS['sock_server'];
    // Crear la solicitud HTTP
    $request = "GET /hosts HTTP/1.1\r\n" .
        "Host: $server_ip:$server_port\r\n" .
        "Content-Type: application/json\r\n" .
        "Content-Length: 0\r\n\r\n";
    // Enviar la solicitud
    $response = send_http_request($sock, $request);
    if ($response === false) {
        return false;
    }
    log_debug("Respuesta del servidor:\n$response");
    return true;
}

// Función para obtener la lista de archivos de un host
function getHostFiles($host, $port)
{
    $server_ip = $GLOBALS['server_ip'];
    $server_port = $GLOBALS['server_port'];
    $sock = $GLOBALS['sock_server'];
    // Crear la solicitud HTTP
    $request = "GET /hosts/$host:$port HTTP/1.1\r\n" .
        "Host: $server_ip:$server_port\r\n" .
        "Content-Type: application/json\r\n" .
        "Content-Length: 0\r\n\r\n";

    // Enviar la solicitud
    $response = send_http_request($sock, $request);
    log_debug("Respuesta del servidor:\n$response\n");
}

// Función para descargar un fichero
function downloadFile($file)
{
    $server_ip = $GLOBALS['server_ip'];
    $server_port = $GLOBALS['server_port'];
    $sock = $GLOBALS['sock_server'];
    // Crear la solicitud HTTP
    $request = "GET /peers/$file HTTP/1.1\r\n" .
        "Host: $server_ip:$server_port\r\n" .
        "Content-Type: application/json\r\n" .
        "Content-Length: 0\r\n\r\n";

    // Enviar la solicitud
    $response = send_http_request($sock, $request);
    if ($response === false) {
        return false;
    }

    $client_ips = json_decode(explode("\r\n\r\n", $response)[1], true);

    if (empty($client_ips)) {
        log_warning("No se encontraron archivos");
        return false;
    }

    // probar a conectarme a las ips para descargar el fichero
    foreach ($client_ips as $ip) {
        $socket = create_socket($GLOBALS['ip'], $GLOBALS['port'], $ip, $GLOBALS['server_port'], false);
        // Crear la solicitud HTTP
        $request = "GET /download/$file HTTP/1.1\r\n" .
            "Host: $ip:$server_port\r\n" .
            "Content-Type: application/json\r\n" .
            "Content-Length: 0\r\n\r\n";
        // comprobar si el socket se ha creado
        if ($socket === false) {
            continue;
        }
        // Enviar la solicitud
        $response = send_http_request($socket, $request);
        if ($response === false) {
            continue;
        } else {
            log_debug("Seleccionando peer $ip para descargar");
            // Comprobar si hay contenido de la descarga
            if (strpos($response, "200 OK") === false) {
                log_warning("No se ha podido descargar el archivo");
                continue;
            } else {
                log_info("Archivo $file descargado satisfactoriamente");
                saveDownloadedFile($file, $response);
                return true;
            }
        }
    }
    return true;
}

// Función para guardar el archivo descargado
function saveDownloadedFile($file, $response)
{
    // Extraer de la respuesta HTTP el contenido
    $content = explode("\r\n\r\n", $response, 2)[1];

    // Crear el archivo en el directorio de descarga
    $download_directory = $GLOBALS['download_directory'];
    $file_path_download = $download_directory . "/$file";
    if (saveFile($file_path_download, $content)) {
        log_info("Archivo guardado en descargas");
    } else {
        log_error("Error al guardar el archivo en descargas");
    }

    // Guardar el archivo para poder compartirlo también
    $shared_directory = $GLOBALS['shared_directory'];
    $file_path_share = $shared_directory . "/$file";
    if (saveFile($file_path_share, $content)) {
        log_info("Archivo listo para compartir");
    } else {
        log_error("Error al guardar el archivo para compartir");
    }
}

// Función auxiliar para guardar contenido en un archivo
function saveFile($file_path, $content)
{
    if (file_put_contents($file_path, $content) !== false) {
        return true;
    } else {
        return false;
    }
}