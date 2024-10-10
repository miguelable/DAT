<?php

// Función para recibir las peticiones POST
function recibirDatos()
{
    if ($_SERVER['REQUEST_METHOD'] === 'POST') {
        $datos = file_get_contents('php://input');
        $datosArray = json_decode($datos, true);

        if (json_last_error() === JSON_ERROR_NONE) {
            almacenarDatos($datosArray);
            echo json_encode(['status' => 'success', 'message' => 'Datos recibidos correctamente']);
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

// Llamar a la función para recibir datos
recibirDatos();