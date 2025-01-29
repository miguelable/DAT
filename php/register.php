<?php
// Configuración de la base de datos
define('DB_HOST', 'dbserver');  // Cambia 'dbserver' por 'localhost' si es necesario
define('DB_NAME', 'grupo03');   // Nombre de la base de datos
define('DB_USER', 'grupo03');   // Nombre de usuario de la base de datos
define('DB_PASS', 'tai1mui1Go'); // Contraseña de la base de datos

try {
    // Crear conexión a la base de datos
    $conn = new PDO("mysql:host=" . DB_HOST . ";dbname=" . DB_NAME . ";charset=utf8mb4", DB_USER, DB_PASS);
    // Establecer modo de error de PDO a excepción
    $conn->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

    // Validar que se reciban datos del formulario
    if ($_SERVER['REQUEST_METHOD'] === 'POST') {
        $user = trim($_POST['username'] ?? '');
        $pass = trim($_POST['password'] ?? '');
        $email = trim($_POST['email'] ?? '');
        $altura = trim($_POST['altura'] ?? '');
        $edad = trim($_POST['edad'] ?? '');
        $fastpass = trim($_POST['fastpass'] ?? '');

        // Validar que los campos no estén vacíos
        if (empty($user) || empty($pass) || empty($email) || empty($altura) || empty($edad) || empty($fastpass)) {
            throw new Exception("Todos los campos son obligatorios.");
        }

        // Verificar si el nombre de usuario ya existe
        $stmt = $conn->prepare("SELECT * FROM users WHERE username = :username");
        $stmt->bindParam(':username', $user);
        $stmt->execute();
        $existingUser = $stmt->fetch(PDO::FETCH_ASSOC);

        if ($existingUser) {
            echo "
            <!DOCTYPE html>
            <html lang='en'>
            <head>
                <meta charset='UTF-8'>
                <script src='https://cdn.jsdelivr.net/npm/sweetalert2@11'></script>
            </head>
            <body>
                <script>
                    Swal.fire({
                        icon: 'error',
                        title: '¡Error!',
                        text: 'El nombre de usuario ya está en uso. Por favor, elige otro.',
                        showConfirmButton: true
                    }).then(function() {
                        window.location.href = '../register.html';
                    });
                </script>
            </body>
            </html>";
            exit;
        }

        // Encriptar contraseña
        $hashedPass = password_hash($pass, PASSWORD_BCRYPT);

        // Insertar en la base de datos
        $sql = "INSERT INTO users (username, password, created_at, altura, edad, email, unidad_familiar, privilegio, fastpass)
                VALUES (:username, :password, NOW(), :altura, :edad, :email, NULL, 'user', :fastpass)";
        
        $stmt = $conn->prepare($sql);
        $stmt->bindParam(':username', $user);
        $stmt->bindParam(':password', $hashedPass);
        $stmt->bindParam(':altura', $altura);
        $stmt->bindParam(':edad', $edad);
        $stmt->bindParam(':email', $email);
        $stmt->bindParam(':fastpass', $fastpass);

        $stmt->execute();

        // Mostrar alerta y redirigir a login.html
        echo "
        <!DOCTYPE html>
        <html lang='en'>
        <head>
            <meta charset='UTF-8'>
            <title>Registro Exitoso</title>
            <script src='https://cdn.jsdelivr.net/npm/sweetalert2@11'></script>
        </head>
        <body>
            <script>
                Swal.fire({
                    icon: 'success',
                    title: '¡Registro exitoso!',
                    text: 'Tu cuenta ha sido creada. Redirigiendo al inicio de sesión...',
                    showConfirmButton: true
                }).then(function() {
                    window.location.href = '../login.html';
                });
            </script>
        </body>
        </html>";
        exit;
    } else {
        throw new Exception("Método no permitido.");
    }
} catch (PDOException $e) {
    echo "Error en la base de datos: " . $e->getMessage();
} catch (Exception $e) {
    echo "Error: " . $e->getMessage();
}
?>
