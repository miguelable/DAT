<?php
// Iniciar sesión
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

    // Comprobar si el formulario fue enviado
    if ($_SERVER["REQUEST_METHOD"] == "POST") {
        // Obtener los datos del formulario
        $username = $_POST['username'];
        $password = $_POST['password'];

        // Crear la consulta para verificar si el usuario existe
        $stmt = $conn->prepare("SELECT * FROM users WHERE username = :username");
        $stmt->bindParam(':username', $username);
        $stmt->execute();
        $user = $stmt->fetch(PDO::FETCH_ASSOC);

        // Verificar si el usuario existe y la contraseña es correcta
        if ($user && password_verify($password, $user['password'])) {
            // Almacenar el nombre de usuario en la sesión
            $_SESSION['username'] = $user['username'];  // Guardamos el nombre de usuario

            // Verificar el privilegio del usuario
            if ($user['privilegio'] == 'admin') {
                // Redirigir a admin.html si el usuario es admin
                header("Location: ../admin.html");
            } else {
                // Redirigir a index.html si el usuario es normal
                header("Location: ../index.html");
            }
            exit;
        } else {
            // Si las credenciales son incorrectas, redirigir a login.html con el parámetro error=true
            header("Location: ../login.html?error=true");
            exit;
        }
    }
} catch (PDOException $e) {
    echo "Error de conexión: " . $e->getMessage();
}