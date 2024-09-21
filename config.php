<?php

// $numUsers = $argv[1];

// si la carpeta users no esta creada crearla
if (!file_exists("users")) {
    mkdir("users", 0777, true);
}

// print_r($argv[1]);

for ($i = 100; $i < 110; $i++) {
    $ip = "127.0.0." . $i;
    crearFichero($ip);
}

// funcion para crear el fichero
function crearFichero($ip)
{
    // extraer el código de user.php
    $codigo = file_get_contents("user.php");
    // reemplazar la ip
    $codigo = str_replace("127.0.0.10", $ip, $codigo);
    // reemplazar la dirección de descarga y compartir
    $client_number = substr($ip, -1);
    // si la carpeta de descarga no está creada la creamos
    if (!file_exists("users/client" . $client_number . "/download")) {
        mkdir("users/client" . $client_number . "/download", 0777, true);
    }
    // si la carpeta de compartir no está creada la creamos
    if (!file_exists("users/client" . $client_number . "/shared")) {
        mkdir("users/client" . $client_number . "/shared", 0777, true);
    }
    // si no esta creada la carpeta la creamos
    if (!file_exists("users/client" . $client_number)) {
        mkdir("users/client" . $client_number, 0777, true);
    }
    // crear el fichero con el código
    $file = "users/client" . $client_number . "/user.php"; // nombre del fichero
    $file = fopen($file, "w");
    fwrite($file, $codigo);
    fclose($file);

    // crear el fichero de texto
    $text = "users/client" . $client_number . "/shared/archivo" . $client_number . ".txt"; // nombre del fichero
    $text = fopen($text, "w");
    fwrite($text, "Hello form the client number " . $client_number . " file.");
    fclose($text);

    // ejecutar el fichero
    $command = "php users/client" . $client_number . "/user.php";
    exec($command, $output);
    echo $output[0] . "\n";
}