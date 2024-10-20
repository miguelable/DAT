<?php

/**
 *
 * Este script maneja solicitudes POST para recibir y almacenar datos JSON.
 *
 * Funciones:
 * - recibirDatos(): Recibe solicitudes POST, decodifica datos JSON y los almacena en un archivo.
 * - almacenarDatos(array $datos): Almacena los datos proporcionados en un archivo JSON.
 *
 * recibirDatos():
 * - Verifica si el método de la solicitud es POST.
 * - Lee los datos brutos de POST y los decodifica desde JSON.
 * - Si los datos JSON son válidos, llama a almacenarDatos() para almacenar los datos y responde con HTTP 200.
 * - Si los datos JSON son inválidos, responde con HTTP 400 y un mensaje de error.
 * - Si el método de la solicitud no es POST, responde con HTTP 405 y un mensaje de error.
 *
 * almacenarDatos(array $datos):
 * - Lee los datos existentes de 'datos_sonda.json' si existe.
 * - Añade los nuevos datos a los datos existentes.
 * - Escribe los datos actualizados de nuevo en 'datos_sonda.json' en un formato JSON con formato bonito.
 */

// Función para recibir las peticiones POST
function recibirDatos()
{
    if ($_SERVER['REQUEST_METHOD'] === 'POST') {
        $datos = file_get_contents('php://input');
        $datosArray = json_decode($datos, true);

        if (json_last_error() === JSON_ERROR_NONE) {
            // Almacenar los datos en un archivo JSON
            almacenarDatos($datosArray);
            http_response_code(200);
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