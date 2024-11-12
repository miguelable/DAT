<?php

// Función para recibir las peticiones POST
function recibirDatos()
{
    // Verificar si el archivo existe y, si no, inicializarlo con un arreglo vacío
    if (!file_exists('led_status.json')) {
        file_put_contents('led_status.json', '[]');
    }

    // Obtener los datos del fichero JSON de led_status.json
    $ledStatus = json_decode(file_get_contents('led_status.json'), true);

    if ($_SERVER['REQUEST_METHOD'] === 'POST') {
        $datos = file_get_contents('php://input');
        $value = json_decode($datos, true);

        // extraer el actual_status y devolver el desired_status correspondiente
        $id_sonda = $value['id_sonda'];
        $actual_status = $value['actual_status'];

        // Buscar el id_sonda en el array ledStatus
        $found = false;
        foreach ($ledStatus as &$led) {
            if ($led['id_sonda'] == $id_sonda) {
                // Si se encuentra, actualizar el actual_status
                $led['actual_status'] = $actual_status;
                $desiredStatus = ['id_sonda' => $id_sonda, 'desired_status' => $led['desired_status']];
                $found = true;
                break;
            }
        }

        // Si no se encuentra, agregar un nuevo objeto
        if (!$found) {
            $newLedStatus = [
                'id_sonda' => $id_sonda,
                'desired_status' => $actual_status,
                'actual_status' => $actual_status
            ];
            $ledStatus[] = $newLedStatus;
            $desiredStatus = ['id_sonda' => $id_sonda, 'desired_status' => $actual_status];
        }

        // Guardar los cambios en el archivo led_status.json
        file_put_contents('led_status.json', json_encode($ledStatus, JSON_PRETTY_PRINT));

        // Devolver el desired_status correspondiente
        echo json_encode($desiredStatus);

        // Almacenar los datos en el archivo JSON
        almacenarDatos($value);
    } else {
        http_response_code(405);
        echo 'Método no permitido';
    }
}

// Función para almacenar los datos en un archivo JSON
function almacenarDatos($value)
{
    // Verificar si el archivo existe y, si no, inicializarlo con un arreglo vacío
    if (!file_exists('sensor_data.json')) {
        file_put_contents('sensor_data.json', '[]');
    }

    // Leer el contenido existente del archivo y decodificarlo
    $data = json_decode(file_get_contents('sensor_data.json'), true);

    // Añadir los nuevos datos al final del arreglo existente
    $data[] = $value;

    // Guardar el arreglo actualizado en el archivo JSON
    file_put_contents('sensor_data.json', json_encode($data, JSON_PRETTY_PRINT));
}

// Llamar a la función para recibir datos
recibirDatos();