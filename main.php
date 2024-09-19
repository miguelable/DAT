<?php

require_once "client.php";
require_once "server.php";

$server_host = '127.0.0.1';
$server_port = 8888;



if ($argv[1] == "--server") {
    run_server($server_host,$server_port);
} else {
    
    // Parsear la URL proporcionada
    $url_parts = parse_url($argv[1]);
    
    // Asignar valores por defecto
    $ip = isset($url_parts['host']) ? $url_parts['host'] : $server_host;
    $puerto = isset($url_parts['port']) ? $url_parts['port'] : $server_port;
    $path = isset($url_parts['path']) ? $url_parts['path'] : $argv[1];
    if (!isset($argv[2])){
        $outputFileName = substr ($url_parts['path'],1);
    } else 
        $outputFileName  = $argv[2];

    // Llamar al cliente con los valores obtenidos o por defecto
    run_client($ip, $puerto, $path, $outputFileName);
}

