<?php

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
$sock = create_socket();

// 1. Función para crear y conectar el socket
function create_socket()
{
    $sock = socket_create(AF_INET, SOCK_STREAM, getprotobyname('tcp'));
    if ($sock === false) {
        log_error("Error creando el socket: " . socket_strerror(socket_last_error()));
        return false;
    }

    if (socket_bind($sock, $GLOBALS['ip'], $GLOBALS['port']) === false) {
        log_error("Error asociando el socket: " . socket_strerror(socket_last_error($sock)));
        socket_close($sock);
        return false;
    }

    if (socket_connect($sock,  $GLOBALS['server_ip'], $GLOBALS['server_port']) === false) {
        log_error("Error conectando al servidor: " . socket_strerror(socket_last_error($sock)));
        socket_close($sock);
        return false;
    }
    return $sock;
}

// 2. Función para obtener los archivos compartidos
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

    if (empty($shared_files)) {
        log_warning("No hay archivos para compartir");
    }
    return array_values($shared_files);
}

// 3. Función para enviar los archivos compartidos
function send_shared_files($shared_files)
{
    if (empty($shared_files)) {
        return;
    }

    $ip = $GLOBALS['ip'];
    $server_ip = $GLOBALS['server_ip'];
    $server_port = $GLOBALS['server_port'];
    $sock = $GLOBALS['sock'];

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
    return true;
}

// 4. Función para gestionar la reconexión en caso de fallo
function handle_socket_error()
{
    log_warning("Intentando reconectar...");
    socket_close($GLOBALS['sock']);
    $GLOBALS['sock'] = create_socket();
}

// 5. Función para ejecutar el bucle de envío de archivos compartidos
function file_sending_loop()
{
    while (true) {
        $shared_files = get_shared_files();
        if (!send_shared_files($shared_files)) {
            // Si ocurre un error, intenta reconectar
            handle_socket_error();
        }
        sleep(10); // Espera 10 segundos antes de enviar nuevamente
    }
}

// 6. Proceso principal
$pid = pcntl_fork();
if ($pid == -1) {
    log_error("Error creando el proceso hijo...\n");
} else if ($pid == 0) {
    // Proceso hijo: envía archivos compartidos periódicamente
    file_sending_loop($sock, $shared_directory, $ip,  $port, $server_ip, $server_port);
} else {
    $pid2 = pcntl_fork();
    if ($pid2 == -1) {
        log_error("Error creando el proceso hijo...\n");
    } else if ($pid2 == 0) {
        // Proceso hijo2: leer por terminal las ordenes del usuario
        terminal_loop();
    } else {
        $pid3 = pcntl_fork();
        if ($pid3 == -1) {
            log_error("Error creando el proceso hijo...\n");
        } else if ($pid3 == 0) {
            // Proceso hijo3: leer la respuesta del servidor
            $response = read_server_response($sock);
            // log_debug("Respuesta del servidor:\n$response");
        } else {
            // Proceso padre: esperar a que los 3 hijos terminen
            pcntl_waitpid($pid, $status);
            pcntl_waitpid($pid2, $status);
            pcntl_waitpid($pid3, $status);
            log_info("Todos los procesos hijos han terminado");
        }
    }
}

// 7. Función para leer las órdenes del usuario por terminal
function terminal_loop()
{
    while (true) {
        echo "> ";
        $order = trim(fgets(STDIN));
        // separar la orden por espacios
        if (empty($order)) {
            continue;
        }
        $order = explode(" ", $order);
        switch ($order[0]) {
            case "?":
                show_available_commands();
                break;
            case "exit":
                log_info("Saliendo del programa...");
                // Enviar señal a procesos hijos para que terminen
                posix_kill($GLOBALS['pid'], SIGTERM);  // pid del proceso hijo, si lo tienes
                posix_kill($GLOBALS['pid2'], SIGTERM);  // otro proceso hijo
                posix_kill($GLOBALS['pid3'], SIGTERM);  // otro proceso hijo
                exit;
            case "search":
                if (count($order) < 2 || empty($order[1]) || $order[1] === "?") {
                    log_debug("Use: search <file>");
                    break;
                }
                $args = $order[1];
                echo "Searching file: $args\n";
                searchFile($order[1]);
                break;
            case "download":
                // Lógica para descarga (implementación pendiente)
                break;
            case "hosts":
                // Lógica para listar hosts (implementación pendiente)
                break;
            case "host_files":
                // Lógica para listar archivos de un host (implementación pendiente)
                break;
            default:
                log_warning("Comando no reconocido: " . $order[0]);
                break;
        }
    }
}

// 8. Función para buscar archivos
function searchFile($args)
{
    $server_ip = $GLOBALS['server_ip'];
    $server_port = $GLOBALS['server_port'];
    $sock = $GLOBALS['sock'];
    // Crear la solicitud HTTP
    $request = "GET /search/$args HTTP/1.1\r\n" .
        // host variables globales
        "Host: $server_ip:$server_port\r\n" .
        "Content-Type: application/json\r\n" .
        "Content-Length: 0\r\n\r\n";

    // Enviar la solicitud
    if (!@socket_write($sock, $request, strlen($request))) {
        log_error("Error escribiendo en el socket: " . socket_strerror(socket_last_error($sock)));
        return false;
    }
    log_info("Solicitud enviada correctamente:\n$request");
    // Leer la respuesta
    $response = read_server_response($sock);
    if ($response === false) {
        return false;
    }
    log_debug("Respuesta del servidor:\n$response\n");
    return true;
}

// 9. Función para mostrar los comandos disponibles
function show_available_commands()
{
    echo "Comandos disponibles:\n";
    echo "search \t\t <arg> \t\t Buscar todos los archivos que contienen arg\n";
    echo "search_file \t <file> \t Buscar un archivo\n";
    echo "download \t <file> \t Descargar un archivo\n";
    echo "host_files \t <host> \t Obtener la lista de archivos de un host\n";
    echo "hosts\t\t\t\t Obtener la lista de hosts\n";
    echo "exit \t\t\t\t Salir del programa\n";
}

// 10. Función para leer la respuesta del servidor
function read_server_response($sock)
{
    $response = socket_read($sock, 1024);
    if ($response === false) {
        log_error("Error al leer la respuesta del servidor: " . socket_strerror(socket_last_error($sock)));
        return false;
    }
    return $response;
}