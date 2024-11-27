<?php
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

    // Consulta para eliminar todos los datos de la tabla LED_STATUS
    $stmt = $conn->prepare("DELETE FROM LED_STATUS");
    $stmt->execute();

    echo "Datos eliminados correctamente";
} catch (PDOException $e) {
    echo "Error de conexión: " . $e->getMessage();
}