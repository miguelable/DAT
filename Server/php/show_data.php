<?php
// Configuracion de la base de datos
define('DB_HOST', 'dbserver');  // Cambia 'dbserver' por 'localhost' si es necesario
define('DB_NAME', 'grupo03');   // Nombre de la base de datos
define('DB_USER', 'grupo03');   // Nombre de usuario de la base de datos
define('DB_PASS', 'tai1mui1Go'); // Contrasena de la base de datos

try {
    // Crear conexion a la base de datos
    $conn = new PDO("mysql:host=" . DB_HOST . ";dbname=" . DB_NAME, DB_USER, DB_PASS);
    // Establecer modo de error de PDO a excepcion
    $conn->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
    // Consulta para obtener todos los datos de la tabla POT_SENSOR
    $stmt = $conn->prepare("SELECT * FROM POT_SENSOR ORDER BY tiempo_muestra DESC");
    $stmt->execute();

    // Obtener los resultados
    $resultados = $stmt->fetchAll(PDO::FETCH_ASSOC);

    // Mostrar los resultados
    echo "<html><head><title>Datos en POT_SENSOR</title></head><body>";
    //  Boton para eliminar todos los datos de la tabla 
    echo '<button class="button-eliminar" onclick="eliminarDatos();">Eliminar Datos</button>';
    echo "<h3>Datos en la tabla POT_SENSOR</h3>";

    if ($resultados) {
        echo "<div class='scroll-container'><table>
                <tr>
                    <th>ID Sonda</th>
                    <th>Potencia</th>
                    <th>Tiempo Muestra</th>
                </tr>";

        foreach ($resultados as $fila) {
            echo "<tr>
                    <td>{$fila['id_sonda']}</td>
                    <td>{$fila['potencia']}</td>
                    <td>{$fila['tiempo_muestra']}</td>
                  </tr>";
        }
        echo "</table></div>";
    } else {
        echo "No se encontraron datos en la tabla POT_SENSOR.";
    }

    echo "</body></html>";
} catch (PDOException $e) {
    echo "Error de conexion: " . $e->getMessage();
}