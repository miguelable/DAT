<?php
session_start();

// Configuración de la base de datos
define('DB_HOST', 'dbserver');  // Cambia 'dbserver' por 'localhost' si es necesario
define('DB_NAME', 'grupo03');   // Nombre de la base de datos
define('DB_USER', 'grupo03');   // Nombre de usuario de la base de datos
define('DB_PASS', 'tai1mui1Go'); // Contraseña de la base de datos

try {
    // Crear conexión a la base de datos
    $conn = new PDO("mysql:host=" . DB_HOST . ";dbname=" . DB_NAME, DB_USER, DB_PASS);
    // Establecer modo de error de PDO a excepción
    $conn->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

    if (isset($_SESSION['username'])) {
        $username = $_SESSION['username'];

        // Verificar si el usuario tiene sesión activa
        error_log("Nombre de usuario desde sesión: " . $username); // Verificar que el username existe

        // Obtener datos del usuario desde la tabla 'users'
        $query = "SELECT * FROM users WHERE username = :username";
        $stmt = $conn->prepare($query);
        $stmt->bindParam(':username', $username);
        $stmt->execute();

        $userData = $stmt->fetch(PDO::FETCH_ASSOC);

        // Verificar si se encontraron datos
        if ($userData) {
            // Si el usuario existe, enviar los datos como JSON
            echo json_encode([
                'user' => $userData,
                'parques' => obtenerParques($username), // Función para obtener los parques
                'unidad_familiar' => obtenerUnidadFamiliar($username), // Función para obtener la unidad familiar
                'pulsera' => obtenerPulsera($username) // Función para obtener la pulsera
            ]);
        } else {
            // Si no se encuentran datos del usuario
            echo json_encode([
                'error' => 'No identificado',
            ]);
        }
    } else {
        // Si no hay sesión activa
        echo json_encode(['error' => 'No estás autenticado']);
    }
} catch (PDOException $e) {
    // Si hay un error en la conexión o en la consulta
    echo json_encode(['error' => 'Error de conexión: ' . $e->getMessage()]);
}

function obtenerPulsera($username)
{
    // Función para obtener la pulsera del usuario

    // Conexión a la base de datos ya establecida en el archivo principal
    global $conn; // Usamos la conexión existente

    // Consulta SQL para obtener los datos de la pulsera del usuario
    $query = "SELECT numero_pulsera, ultimo_registro, ultima_imagen FROM users WHERE username = :username";
    $stmt = $conn->prepare($query);
    $stmt->bindParam(':username', $username);
    $stmt->execute();

    // Obtener los resultados de la consulta
    $result = $stmt->fetch(PDO::FETCH_ASSOC);

    $pulseraData = [];
    // Verificar si los resultados existen
    if ($result) {
        // si el número de la pulsera no existe, asignar una aleatoria
        if ($result['numero_pulsera'] == null) {
            $numero_pulsera = rand(0000, 9999);
            $query = "UPDATE users SET numero_pulsera = :numero_pulsera WHERE username = :username";
            $stmt = $conn->prepare($query);
            $stmt->bindParam(':numero_pulsera', $numero_pulsera);
            $stmt->bindParam(':username', $username);
            $stmt->execute();
            $result['numero_pulsera'] = $numero_pulsera;
        }
        // Asignar los resultados al array de la pulsera
        $pulseraData = [
            'numero_pulsera' => $result['numero_pulsera'],
            'ultimo_registro' => $result['ultimo_registro'],
            'ultima_imagen' => $result['ultima_imagen']
        ];
    } else {
        // Si no se encuentran resultados, asignar el mensaje de error
        $pulseraData = 'No se encontraron datos de la pulsera';
    }

    // Retornar el array de la pulsera o el mensaje de error
    return $pulseraData;
}

function obtenerParques($username)
{
    // Conexión a la base de datos ya establecida en el archivo principal
    global $conn; // Usamos la conexión existente

    // Definir los nombres de los parques
    $parques = ['PortAventura', 'CaribeAquaticPark', 'FerrariLand'];

    $parquesData = [];
    // Iterar sobre los parques y obtener los datos de cada uno
    foreach ($parques as $parque) {
        $query = "SELECT fecha_acceso, fecha_salida FROM " . $parque . " WHERE username = :username";
        $stmt = $conn->prepare($query);
        $stmt->bindParam(':username', $username);
        $stmt->execute();

        // Obtener los resultados y agregarlos al array de parques
        $result = $stmt->fetch(PDO::FETCH_ASSOC);
        if ($result) {
            $parquesData[$parque] = [
                'fecha_acceso' => $result['fecha_acceso'],
                'fecha_salida' => $result['fecha_salida']
            ];
        }
    }

    return $parquesData;
}
function obtenerUnidadFamiliar($username)
{
    // Conexión a la base de datos ya establecida en el archivo principal
    global $conn; // Usamos la conexión existente

    // Definir el array para los integrantes de la unidad familiar
    $integrantes_unidad = [];

    // Consulta SQL para obtener los datos de la unidad familiar donde cualquiera de los campos coincida con el username
    $query = "SELECT responsable_1, responsable_2, menor_1, menor_2, menor_3 
              FROM unidades_familiares 
              WHERE responsable_1 = :username OR responsable_2 = :username OR menor_1 = :username 
              OR menor_2 = :username OR menor_3 = :username";

    $stmt = $conn->prepare($query);
    $stmt->bindParam(':username', $username);
    $stmt->execute();

    // Obtener los resultados de la consulta
    $result = $stmt->fetch(PDO::FETCH_ASSOC);

    // Verificar si los resultados existen
    if ($result) {
        // Asignar los resultados al array si se encuentra el username
        $integrantes_unidad = [
            'responsable_1' => $result['responsable_1'],
            'responsable_2' => $result['responsable_2'],
            'menor_1' => $result['menor_1'],
            'menor_2' => $result['menor_2'],
            'menor_3' => $result['menor_3']
        ];
    } else {
        // Si no se encuentran resultados, asignar el mensaje de error
        $integrantes_unidad = 'No pertenece a ninguna unidad familiar';
    }

    // Retornar el array de integrantes de la unidad familiar o el mensaje de error
    return $integrantes_unidad;
}
