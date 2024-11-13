<?php
// Configuracion de la base de datos
define('DB_HOST', 'dbserver'); // Cambia 'dbserver' por 'localhost' si es necesario
define('DB_NAME', 'grupo03');   // Nombre de la base de datos
define('DB_USER', 'grupo03');   // Nombre de usuario de la base de datos
define('DB_PASS', 'tai1mui1Go'); // Contrasena de la base de datos

try {
    // Crear conexion a la base de datos
    $conn = new PDO("mysql:host=" . DB_HOST . ";dbname=" . DB_NAME, DB_USER, DB_PASS);
    // Establecer modo de error de PDO a excepcion
    $conn->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

    // Obtener los datos enviados por POST
    $id_sonda = $_POST['id_sonda'];
    $desired_status = $_POST['desired_status'];
    $source = $_POST['source']; // Nuevo parámetro para identificar la fuente

    if ($source === 'user') {
        // Actualizar el estado deseado en la base de datos
        $stmt = $conn->prepare("UPDATE LED_STATUS SET desired_status = :desired_status WHERE id_sonda = :id_sonda");
        $stmt->bindParam(':desired_status', $desired_status);
        $stmt->bindParam(':id_sonda', $id_sonda);
        $stmt->execute();
        echo "Estado deseado actualizado correctamente por el usuario";
    } else {
        echo "Fuente de actualización no válida";
    }
} catch (PDOException $e) {
    echo "Error de conexion: " . $e->getMessage();
}
