<?php

// Función para recibir las peticiones POST
function recibirDatos()
{
    if ($_SERVER['REQUEST_METHOD'] === 'POST') {
        $datos = file_get_contents('php://input');
        $datosArray = json_decode($datos, true);

        if (json_last_error() === JSON_ERROR_NONE) {
            // Almacenar los datos en un archivo JSON
            almacenarDatos($datosArray);

            // Enviar datos a controller.php
            if (enviarDatosPorSocket($datosArray)) {
                echo json_encode(['status' => 'success', 'message' => 'Datos recibidos y enviados correctamente']);
            } else {
                echo json_encode(['status' => 'warning', 'message' => 'Datos almacenados, pero error al enviar']);
            }
        } else {
            http_response_code(400);
            echo json_encode(['status' => 'error', 'message' => 'Datos JSON inválidos']);
        }
    } else {
        http_response_code(405);
        echo json_encode(['status' => 'error', 'message' => 'Método no permitido']);
    }
}

// Función para almacenar los datos en un archivo JSON
function almacenarDatos($datos)
{
    $archivo = 'datos_sonda.json';
    $datosExistentes = [];

    if (file_exists($archivo)) {
        $contenido = file_get_contents($archivo);
        $datosExistentes = json_decode($contenido, true);
    }

    $datosExistentes[] = $datos;
    file_put_contents($archivo, json_encode($datosExistentes, JSON_PRETTY_PRINT));
}

// Función para enviar datos a controller.php mediante un socket
function enviarDatosPorSocket($datos)
{
    $host = '127.0.0.1'; // Cambia según tu configuración
    $port = 8080; // Puerto donde escucha controller.php

    // Crear socket
    $socket = socket_create(AF_INET, SOCK_STREAM, 0);
    if ($socket === false) {
        return false;
    }

    // Conectar al socket
    if (socket_connect($socket, $host, $port) === false) {
        socket_close($socket);
        return false;
    }

    // Enviar datos JSON
    $jsonDatos = json_encode($datos);
    socket_write($socket, $jsonDatos, strlen($jsonDatos));

    // Cerrar socket
    socket_close($socket);
    return true;
}

// Llamar a la función para recibir datos
recibirDatos();