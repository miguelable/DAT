<?php

require_once "logs.php";

// Datos de configuración del cliente y el servidor
$ip = "127.0.0.10";
$port = 8888;
$server_ip = "127.0.0.1";
$server_port = 8888;

// get download directory
$download_directory = "users/" . basename(__DIR__) . "/download/";
$shared_directory = "users/" . basename(__DIR__) . "/shared/";

// 1. Función para crear y conectar el socket
function create_socket($ip, $port, $server_ip, $server_port)
{
    $sock = socket_create(AF_INET, SOCK_STREAM, getprotobyname('tcp'));
    if ($sock === false) {
        die("Error creando el socket: " . socket_strerror(socket_last_error()));
    }

    if (socket_bind($sock, $ip, $port) === false) {
        die("Error asociando el socket: " . socket_strerror(socket_last_error($sock)));
    }

    if (socket_connect($sock, $server_ip, $server_port) === false) {
        die("Error conectando al servidor: " . socket_strerror(socket_last_error($sock)));
    }

    log_info("Conectado al servidor");
    return $sock;
}

// 2. Función para obtener los archivos compartidos
function get_shared_files($shared_directory)
{
    if (!file_exists($shared_directory)) {
        log_error("Directorio compartido no encontrado: $shared_directory");
        return [];
    }

    $files = scandir($shared_directory);
    $shared_files = array_filter($files, function ($file) {
        return $file !== "." && $file !== "..";
    });

    if (empty($shared_files)) {
        log_warning("No hay archivos para compartir");
    }

    return $shared_files;
}

// 3. Función para enviar los archivos compartidos
function send_shared_files($sock, $shared_files, $ip, $server_ip, $server_port)
{
    if (empty($shared_files)) {
        return;
    }

    // Crear el JSON con los nombres de los archivos
    $shared_files_data = json_encode(["files" => $shared_files]);

    // Preparar la solicitud HTTP
    $request = "PUT /hosts/$ip HTTP/1.1\r\n" .
        "Host: $server_ip:$server_port\r\n" .
        "Content-Type: application/json\r\n" .
        "Content-Length: " . strlen($shared_files_data) . "\r\n\r\n" .
        $shared_files_data;

    // Intentar enviar la solicitud
    if (!@socket_write($sock, $request, strlen($request))) {
        log_error("Error escribiendo en el socket: " . socket_strerror(socket_last_error($sock)));
        return false;
    }

    log_info("Solicitud enviada correctamente:\n$request");
    return true;
}

// 4. Función para gestionar la reconexión en caso de fallo
function handle_socket_error(&$sock, $ip, $port, $server_ip, $server_port)
{
    log_warning("Intentando reconectar...");
    socket_close($sock);
    $sock = create_socket($ip, $port, $server_ip, $server_port);
}

// Inicializar el socket
$sock = create_socket($ip, $port, $server_ip, $server_port);

// 5. Función para ejecutar el bucle de envío de archivos compartidos
function file_sending_loop($sock, $shared_directory, $ip, $port, $server_ip, $server_port)
{
    while (true) {
        $shared_files = get_shared_files($shared_directory);
        if (!send_shared_files($sock, $shared_files, $ip, $server_ip, $server_port)) {
            // Si ocurre un error, intenta reconectar
            handle_socket_error($sock, $ip, $port, $server_ip, $server_port);
        }
        sleep(10); // Espera 10 segundos antes de enviar nuevamente
    }
}

// 6. Proceso principal
$pid = pcntl_fork();
if ($pid == -1) {
    die("Error creando el proceso hijo...\n");
} else if ($pid == 0) {
    // Proceso hijo: envía archivos compartidos periódicamente
    file_sending_loop($sock, $shared_directory, $ip,  $port, $server_ip, $server_port);
} else {
    // Proceso padre: leer la respuesta del servidor
    $response = socket_read($sock, 1024);
    log_debug("Respuesta del servidor:\n$response");
    socket_close($sock); // Cierra el socket al terminar
}