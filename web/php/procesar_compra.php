<?php
// Configuración de la base de datos
define('DB_HOST', 'dbserver');
define('DB_NAME', 'grupo03');
define('DB_USER', 'grupo03');
define('DB_PASS', 'tai1mui1Go');

header('Content-Type: application/json; charset=utf-8');

// Conexión a la base de datos
$conn = connectToDatabase();

if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    try {
        $ticketQuantity = $_POST['ticketQuantity'];
        $ticketType = $_POST['ticketType'];
        $accessDate = $_POST['accessDate'];
        $exitDate = $_POST['exitDate'];
        $fastPass = $_POST['fastpass'];

        $responsables = [];
        $menores = [];
        $usernames = '';
        $familyId = null;

        // Verificación de si existen menores de 12 años
        for ($i = 0; $i < $ticketQuantity; $i++) {
            $edad = $_POST["visitor"][$i]["edad"];
            if ($edad <= 12) {
                $menores[] = $_POST["visitor"][$i]["name"];
            }
        }

        // Si hay menores, se crea la unidad familiar
        if (count($menores) > 0) {
            $familyId = generateUniqueFamilyId($conn);
        }

        // Procesar cada visitante
        for ($i = 0; $i < $ticketQuantity; $i++) {
            $cuenta = $_POST["visitor"][$i]["create-account"];
            $name = $_POST["visitor"][$i]["name"];
            $edad = $_POST["visitor"][$i]["edad"];
            
            // Verificar si el usuario existe
            if (isUsernameTaken($name, $conn)) {
                handleExistingUser($i, $name, $edad, $fastPass, $accessDate, $exitDate, $ticketType, $conn, $responsables, $menores, $usernames, $cuenta, $familyId);
            } else {
                handleNewUser($i, $name, $edad, $fastPass, $accessDate, $exitDate, $ticketType, $conn, $responsables, $menores, $usernames, $cuenta, $familyId);
            }
        }
        echo json_encode(['status' => 'success', 'message' => $usernames]);
    } catch (PDOException $e) {
        echo json_encode(['status' => 'error', 'message' => "Error al almacenar los datos: " . $e->getMessage()]);
    }
} else {
    echo json_encode(['status' => 'error', 'message' => 'Solicitud inválida.']);
}


function connectToDatabase() {
    try {
        $conn = new PDO("mysql:host=" . DB_HOST . ";dbname=" . DB_NAME, DB_USER, DB_PASS);
        $conn->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
        return $conn;
    } catch (PDOException $e) {
        echo json_encode(['status' => 'error', 'message' => "Error al conectar con la base de datos: " . $e->getMessage()]);
        exit;
    }
}

function generateUniqueFamilyId($conn) {
    $query = $conn->prepare("SELECT MAX(unidad_familiar) AS max_id FROM unidades_familiares");
    $query->execute();
    $result = $query->fetch(PDO::FETCH_ASSOC);
    return ($result['max_id'] ?? 0) + 1;
}

function generateUniqueUsername($baseName, $conn) {
    $baseName = strtolower(str_replace(' ', '_', $baseName));
    $username = $baseName;
    $counter = 1;
    while (isUsernameTaken($username, $conn)) {
        $username = $baseName . $counter++;
    }
    return $username;
}

function isUsernameTaken($username, $conn) {
    $query = $conn->prepare("SELECT COUNT(*) FROM users WHERE username = :username");
    $query->execute([':username' => $username]);
    return $query->fetchColumn() > 0;
}

function handleTicket($ticketType, $username, $accessDate, $exitDate, $conn) {
    $table = getTicketTable($ticketType);
    if (!$table) {
        echo json_encode(['status' => 'error', 'message' => 'Tipo de acceso no válido.']);
        exit;
    }

    if (isTicketExist($username, $table, $conn)) {
        updateTicket($username, $accessDate, $exitDate, $table, $conn);
    } else {
        insertTicket($username, $accessDate, $exitDate, $table, $conn);
    }
}

function getTicketTable($ticketType) {
    $validTypes = ['CaribeAquaticPark', 'FerrariLand', 'PortAventura'];
    return in_array($ticketType, $validTypes) ? $ticketType : null;
}

function isTicketExist($username, $table, $conn) {
    $stmtCheck = $conn->prepare("SELECT COUNT(*) FROM $table WHERE username = :username");
    $stmtCheck->execute([':username' => $username]);
    return $stmtCheck->fetchColumn() > 0;
}

function updateTicket($username, $accessDate, $exitDate, $table, $conn) {
    $stmtUpdate = $conn->prepare("UPDATE $table SET fecha_acceso = :fecha_acceso, fecha_salida = :fecha_salida WHERE username = :username");
    $stmtUpdate->execute([':username' => $username, ':fecha_acceso' => $accessDate, ':fecha_salida' => $exitDate]);
}

function insertTicket($username, $accessDate, $exitDate, $table, $conn) {
    $stmtInsert = $conn->prepare("INSERT INTO $table (username, fecha_acceso, fecha_salida) VALUES (:username, :fecha_acceso, :fecha_salida)");
    $stmtInsert->execute([':username' => $username, ':fecha_acceso' => $accessDate, ':fecha_salida' => $exitDate]);
}

function handleExistingUser($i, $name, $edad, $fastPass, $accessDate, $exitDate, $ticketType, $conn, &$responsables, &$menores, &$usernames, $cuenta, $familyId) {
    if ($edad <= 12) {
        $menores[] = $name;
    } else {
        $responsables[] = $name;
    }

    if (count($responsables) === 0 && count($menores) > 0) {
        echo json_encode(['status' => 'error', 'message' => "Los menores de 12 años deben ir acompañados por un mayor de edad"]);
        exit;
    }

    updateUser($i, $name, $edad, $fastPass, $conn);

    if (isset($ticketType)) {
        handleTicket($ticketType, $name, $accessDate, $exitDate, $conn);
    }

    // Si hay unidad familiar, asignarla
    if ($familyId !== null) {
        assignFamilyToUser($name, $familyId, $conn);
    }
}

function updateUser($i, $name, $edad, $fastPass, $conn) {
    $stmtUpdate = $conn->prepare("UPDATE users SET altura = :altura, edad = :edad, email = :email, fastpass = :fastpass WHERE username = :username");
    $stmtUpdate->execute([ 
        ':altura' => $_POST["visitor"][$i]["altura"], 
        ':edad' => $edad,
        ':email' => $_POST["visitor"][$i]["email"], 
        ':fastpass' => $fastPass, 
        ':username' => $name 
    ]);
}

function handleNewUser($i, $name, $edad, $fastPass, $accessDate, $exitDate, $ticketType, $conn, &$responsables, &$menores, &$usernames, $cuenta, $familyId) {
    $username = generateUniqueUsername($name, $conn);
    if ($edad <= 12) {
        $menores[] = $username;
    } else {
        $responsables[] = $username;
    }

    if (count($responsables) === 0 && count($menores) > 0) {
        echo json_encode(['status' => 'error', 'message' => "Los menores de 12 años deben ir acompañados por un mayor de edad"]);
        exit;
    }

    if ($cuenta === 'on') {
        $usernames .= "Para el visitante " . ($i + 1) . ", el nombre de usuario es: $username y su contraseña: cambiar. ";

        if ($familyId !== null) {
            insertFamily($familyId, $responsables, $menores, $conn);
        }

        insertUser($i, $username, $edad, $fastPass, $familyId, $conn);
    }

    if (isset($ticketType)) {
        handleTicket($ticketType, $username, $accessDate, $exitDate, $conn);
    }
}

function insertFamily($familyId, $responsables, $menores, $conn) {
    // Verificar que los responsables y menores tengan datos
    $responsable1 = $responsables[0] ?? null;
    $responsable2 = $responsables[1] ?? null;
    
    $menor1 = $menores[0] ?? null;
    $menor2 = $menores[1] ?? null;
    $menor3 = $menores[2] ?? null;
    $menor4 = $menores[3] ?? null;
    $menor5 = $menores[4] ?? null;

    // Preparar la inserción en la base de datos, asegurándose de incluir solo una inserción por familia
    $stmt = $conn->prepare("INSERT INTO unidades_familiares (unidad_familiar, responsable_1, responsable_2, menor_1, menor_2, menor_3, menor_4, menor_5, password) 
                            VALUES (:familyId, :responsable1, :responsable2, :menor1, :menor2, :menor3, :menor4, :menor5, 'cambiar')");

    // Ejecutar la inserción solo una vez para la familia
    $stmt->execute([
        ':familyId' => $familyId, 
        ':responsable1' => $responsable1, 
        ':responsable2' => $responsable2, 
        ':menor1' => $menor1, 
        ':menor2' => $menor2, 
        ':menor3' => $menor3, 
        ':menor4' => $menor4, 
        ':menor5' => $menor5
    ]);
}

function insertUser($i, $username, $edad, $fastPass, $familyId, $conn) {
    $stmt = $conn->prepare("INSERT INTO users (username, altura, edad, email, fastpass, unidad_familiar) VALUES (:username, :altura, :edad, :email, :fastpass, :unidad_familiar)");
    $stmt->execute([ 
        ':username' => $username, 
        ':altura' => $_POST["visitor"][$i]["altura"], 
        ':edad' => $edad,
        ':email' => $_POST["visitor"][$i]["email"], 
        ':fastpass' => $fastPass, 
        ':unidad_familiar' => $familyId
    ]);
}

function assignFamilyToUser($username, $familyId, $conn) {
    $stmtUpdate = $conn->prepare("UPDATE users SET unidad_familiar = :unidad_familiar WHERE username = :username");
    $stmtUpdate->execute([':unidad_familiar' => $familyId, ':username' => $username]);
}