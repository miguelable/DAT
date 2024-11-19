<?php

// Dirección y puerto en los que el servidor escuchará
$host = '0.0.0.0'; // Escucha en todas las interfaces de red
$port = 54471;     // Puerto de escucha

// Variable para el estado deseado del LED
$desiredLedStatus = true;

// Crear el socket
$server = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
if (!$server) {
    die("Error al crear el socket: " . socket_strerror(socket_last_error()) . "\n");
} {
    echo "Socket creado...\n";
}

// Enlazar el socket al host y puerto
if (!socket_bind($server, $host, $port)) {
    die("Error al enlazar el socket: " . socket_strerror(socket_last_error()) . "\n");
} else {
    echo "Enlazado al puerto $port...\n";
}

// Escuchar conexiones entrantes
if (!socket_listen($server)) {
    die("Error al escuchar en el puerto: " . socket_strerror(socket_last_error()) . "\n");
} else {
    echo "Servidor escuchando en el puerto $port...\n";
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

function handle_led_status($value, $client)
{
    // Verificar si el archivo existe y, si no, inicializarlo con un arreglo vacío
    if (!file_exists('led_status.json')) {
        file_put_contents('led_status.json', '[]');
    }

    // Obtener los datos del fichero JSON de led_status.json
    $ledStatusFile = json_decode(file_get_contents('led_status.json'), true);

    // extraer el actual_status y devolver el desired_status correspondiente
    $id_sonda = $value['id_sonda'];
    $actual_status = $value['ledStatus'];

    // Buscar el id_sonda en el array ledStatus
    $found = false;
    foreach ($ledStatusFile as &$led) {
        if ($led['id_sonda'] == $id_sonda) {
            // Si se encuentra, actualizar el actual_status
            $led['actual_status'] = $actual_status;
            $found = true;
            // Devolver el desired_status
            $desiredStatus = ['id_sonda' => $id_sonda, 'desired_status' => $led['desired_status']];
            $response = json_encode($desiredStatus);
            echo "Enviando respuesta: $response\n";
            socket_write($client, $response, strlen($response));
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
        $ledStatusFile[] = $newLedStatus;
    }

    // Guardar los cambios en el archivo led_status.json
    file_put_contents('led_status.json', json_encode($ledStatusFile, JSON_PRETTY_PRINT));
}

// Función para manejar la conexión con un cliente
function handle_client($client)
{
    while (true) {
        // Leer la solicitud del cliente
        $request = socket_read($client, 1024);
        // comprobar si es un json, si no lo es, se salta
        if (strpos($request, '{') === false)
            continue;
        // si existe contenido antes del json, se elimina
        $request = substr($request, strpos($request, '{'));
        // parsear el json
        $jsonArray = json_decode($request, true);
        // comprobar si es un json valido, si no lo es, se salta
        if ($jsonArray === null)
            continue;
        // si el json contiene el campo "ledStatus"
        if (isset($jsonArray['ledStatus'])) {
            // Manejar el ledStatus
            handle_led_status($jsonArray, $client);
        } else if (isset($jsonArray['potencia'])) {
            // Almacenar los datos en el archivo JSON
            almacenarDatos($jsonArray);
            echo "Datos almacenados: $request\n";
        }
    }
}

while (true) {
    // Aceptar conexiones entrantes
    if (($client = socket_accept($server)) !== false) {
        // Crear un nuevo proceso para manejar al cliente
        $pid = pcntl_fork();
        if ($pid == -1) {
            die("Error al bifurcar el proceso\n");
        } elseif ($pid) {
            // Código del proceso padre
            echo "Proceso padre con PID: " . posix_getpid() . "\n";
        } else {
            // Código del proceso hijo
            echo "Proceso hijo con PID: " . posix_getpid() . "\n";
            handle_client($client);
            exit(0); // Importante: cerrar el proceso hijo
        }
    }
}

// Cerrar el socket del servidor al finalizar (aunque nunca se alcanzará)
socket_close($server);
echo "Servidor cerrado.\n";
