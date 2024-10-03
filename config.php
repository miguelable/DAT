<?php

require_once "logs.php";

/*
 !! Important run server before start configuring the clients ¡¡
*/

// si la carpeta users no esta creada crearla
if (!file_exists("users")) {
    mkdir("users", 0777, true);
}

for ($i = 100; $i < 110; $i++) {
    // Crear un proceso hijo usando pcntl_fork
    $pid = pcntl_fork();

    if ($pid == -1) {
        // Si no se puede crear un hijo, muestra un error
        die("Error: No se pudo crear el proceso hijo\n");
    } elseif ($pid == 0) {
        // Código para el proceso hijo
        $ip = "127.0.0." . $i;
        crearFichero($ip);
        // Terminamos el proceso hijo
        exit(0);
    } else {
        // Código para el proceso padre
        log_debug("Hijo creado con IP 127.0.0.$i");
    }
}

// El padre debe esperar a que todos los hijos terminen
while (pcntl_waitpid(0, $status) != -1) {
    $status = pcntl_wexitstatus($status);
    log_info("Proceso hijo terminó con estado $status");
}

// Función para crear el fichero
function crearFichero($ip)
{
    // Extraer el código de user.php
    $codigo = file_get_contents("user.php");
    // Reemplazar la IP
    $codigo = preg_replace('/127\.0\.0\.10/', $ip, $codigo, 1);
    // Reemplazar la dirección de descarga y compartir
    $client_number = substr($ip, -1);

    // Crear carpetas si no existen
    if (!file_exists("users/client" . $client_number . "/download")) {
        mkdir("users/client" . $client_number . "/download", 0777, true);
    }
    if (!file_exists("users/client" . $client_number . "/shared")) {
        mkdir("users/client" . $client_number . "/shared", 0777, true);
    }
    if (!file_exists("users/client" . $client_number)) {
        mkdir("users/client" . $client_number, 0777, true);
    }

    // Crear el fichero PHP
    $file = "users/client" . $client_number . "/user.php"; // Nombre del fichero
    $file = fopen($file, "w");
    fwrite($file, $codigo);
    fclose($file);

    // Crear el fichero de texto
    $text = "users/client" . $client_number . "/shared/archivo" . $client_number . ".txt"; // Nombre del fichero
    $text = fopen($text, "w");
    fwrite($text, "Hello from the client number " . $client_number . " file.");
    fclose($text);

    // Crear el fichero de logs
    $log = "users/client" . $client_number . "/logs.php"; // Nombre del fichero
    $log = fopen($log, "w");
    fwrite($log,  file_get_contents("logs.php"));
    fclose($log);

}
