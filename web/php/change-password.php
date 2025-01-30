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

    // Procesar el cambio de contraseña
    if ($_SERVER['REQUEST_METHOD'] === 'POST') {
        $username = $_POST['username'];
        $currentPassword = $_POST['current_password'];
        $newPassword = $_POST['new_password'];

        // Buscar el usuario en la base de datos
        $stmt = $conn->prepare("SELECT password FROM users WHERE username = :username");
        $stmt->execute([':username' => $username]);
        $user = $stmt->fetch(PDO::FETCH_ASSOC);

        if ($user && password_verify($currentPassword, $user['password'])) {
            // Si la contraseña actual es correcta, actualizar la contraseña
            $newPasswordHashed = password_hash($newPassword, PASSWORD_BCRYPT);
            $stmtUpdate = $conn->prepare("UPDATE users SET password = :new_password WHERE username = :username");
            $stmtUpdate->execute([
                ':new_password' => $newPasswordHashed,
                ':username' => $username
            ]);
            echo "Contraseña cambiada con éxito.<br>";
            echo "<a href='login.html'>Volver al inicio de sesión</a>";
        } else {
            echo "La contraseña actual es incorrecta.";
        }
    }
} catch (PDOException $e) {
    echo "Error al conectar con la base de datos: " . $e->getMessage();
}
?>

<!DOCTYPE html>
<html lang="en">
<head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>TECNOPARK</title>
        <link rel="stylesheet" href="css/styles.css">
        <link rel="icon" href="images/favicon.png" type="image/x-icon">
    </head>
<body class="login-body">
    <header>
        <img src="images/pato.png" alt="Logo" class="header-logo">
        <h1>TECNOPARK</h1>
    </header>
    <div class="login-container">
        <h1>Cambiar Contraseña</h1>
        <form action="change-password.php" method="POST">
            <p>Username:</p>
            <input type="text" id="input-login" name="username" placeholder="Usuario" required>
            <p>Contraseña actual:</p>
            <input type="password" id="input-login" name="current_password" placeholder="Contraseña actual" required>
            <p>Contraseña nueva:</p>
            <input type="password" id="input-login" name="new_password" placeholder="Nueva contraseña" required>
            <button id="button-submit" type="submit">Cambiar Contraseña</button>
        </form>
    </div>
</body>
<footer>
    <p>&copy; 2024 TECNOPARK. Todos los derechos reservados.</p>
</footer>
</html>