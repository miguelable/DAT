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

    // Consulta para obtener todos los datos de la tabla POT_SENSOR
    $stmt = $conn->prepare("SELECT * FROM LED_STATUS ORDER BY id_sonda");
    $stmt->execute();

    // Obtener los resultados
    $resultados = $stmt->fetchAll(PDO::FETCH_ASSOC);

    // Mostrar los resultados
    echo "<html><head><title>Datos de la tabla LED_STATUS</title><link rel='stylesheet' type='text/css' href='styles.css'></head><body>";
    echo "<h3>Datos en la tabla LED_STATUS</h3>";
    if ($resultados) {
        echo "<table>
                <tr>
                    <th>ID Sonda</th>
                    <th>Estado del LED</th>
                    <th>Estado deseado</th>
                </tr>";

        foreach ($resultados as $fila) {
            $actual_status = $fila['actual_status'] == 1 ? 'ENCENDIDO' : 'APAGADO';
            $desired_status = $fila['desired_status'] == 1 ? 'ENCENDIDO' : 'APAGADO';
            $actual_class = $fila['actual_status'] == 1 ? 'encendido' : 'apagado';
            echo "<tr>
                    <td>{$fila['id_sonda']}</td>
                    <td class='{$actual_class}'>{$actual_status}</td>
                    <td>
                        <select class='desired-status' data-id='{$fila['id_sonda']}'>
                            <option value='1' " . ($fila['desired_status'] == 1 ? 'selected' : '') . ">ENCENDIDO</option>
                            <option value='0' " . ($fila['desired_status'] == 0 ? 'selected' : '') . ">APAGADO</option>
                        </select>
                    </td>
                  </tr>";
        }
        echo "</table>";
    } else {
        echo "No se encontraron datos en la tabla LED_STATUS.";
    }

    echo "</body></html>";
} catch (PDOException $e) {
    echo "Error de conexion: " . $e->getMessage();
}
