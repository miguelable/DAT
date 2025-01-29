<?php
// Configuración de la base de datos
define('DB_HOST', 'dbserver');  // Cambia 'dbserver' por 'localhost' si es necesario
define('DB_NAME', 'grupo03');   // Nombre de la base de datos
define('DB_USER', 'grupo03');   // Nombre de usuario de la base de datos
define('DB_PASS', 'tai1mui1Go'); // Contraseña de la base de datos

try {
    // Conexión a la base de datos
    $conn = new PDO("mysql:host=" . DB_HOST . ";dbname=" . DB_NAME, DB_USER, DB_PASS);
    $conn->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

    // Obtener el nombre de la tabla desde la solicitud
    $table = isset($_GET['table']) ? $_GET['table'] : '';

    // Validar el nombre de la tabla (evitar inyecciones SQL)
    $allowedTables = ['users', 'PortAventura', 'CaribeAquaticPark', 'FerrariLand', 'unidades_familiares', 'dispositivos'];
    if (!in_array($table, $allowedTables)) {
        echo "Tabla no permitida.";
        exit;
    }

    // Consultas SQL para la tabla 'users'
    if ($table === 'users') {
        $queryAdmins = "SELECT username, created_at, altura, edad, email, unidad_familiar, fastpass FROM users WHERE privilegio = 'admin'";
        $queryUsers = "SELECT username, created_at, altura, edad, email, unidad_familiar, fastpass FROM users WHERE privilegio = 'user'";

        // Consultar los datos de administradores
        $stmtAdmins = $conn->prepare($queryAdmins);
        $stmtAdmins->execute();
        $admins = $stmtAdmins->fetchAll(PDO::FETCH_ASSOC);

        // Consultar los datos de usuarios
        $stmtUsers = $conn->prepare($queryUsers);
        $stmtUsers->execute();
        $users = $stmtUsers->fetchAll(PDO::FETCH_ASSOC);

        // Mostrar la tabla de administradores
        if ($admins) {
            echo "<h2>Administradores</h2>";
            echo "<table>
                    <thead>
                      <tr>";
            // Crear encabezados dinámicamente
            foreach (array_keys($admins[0]) as $header) {
                echo "<th>" . htmlspecialchars($header) . "</th>";
            }
            echo "  </tr>
                    </thead>
                    <tbody>";
            // Crear filas dinámicamente
            foreach ($admins as $fila) {
                echo "<tr>";
                foreach ($fila as $valor) {
                    echo "<td>" . htmlspecialchars($valor) . "</td>";
                }
                echo "</tr>";
            }
            echo "  </tbody>
                  </table>";
        } else {
            echo "<p>No se encontraron administradores.</p>";
        }

        // Mostrar la tabla de usuarios
        if ($users) {
            echo "<h2>Usuarios</h2>";
            echo "<table>
                    <thead>
                      <tr>";
            // Crear encabezados dinámicamente
            foreach (array_keys($users[0]) as $header) {
                echo "<th>" . htmlspecialchars($header) . "</th>";
            }
            echo "  </tr>
                    </thead>
                    <tbody>";
            // Crear filas dinámicamente
            foreach ($users as $fila) {
                echo "<tr>";
                foreach ($fila as $valor) {
                    echo "<td>" . htmlspecialchars($valor) . "</td>";
                }
                echo "</tr>";
            }
            echo "  </tbody>
                  </table>";
        } else {
            echo "<p>No se encontraron usuarios.</p>";
        }
    } else {
        // Para otras tablas
        $query = "SELECT * FROM $table"; // Consultar todas las columnas en otras tablas
        $stmt = $conn->prepare($query);
        $stmt->execute();
        $resultados = $stmt->fetchAll(PDO::FETCH_ASSOC);

        // Verificar si la consulta retornó resultados
        if ($resultados) {
            // Consultar la tabla 'dispositivos'
            if ($table === 'dispositivos') {
                $query = "SELECT * FROM dispositivos"; // Consultar todos los dispositivos
                $stmt = $conn->prepare($query);
                $stmt->execute();
                $resultados = $stmt->fetchAll(PDO::FETCH_ASSOC);

                // Botones globales para encender/apagar todos los dispositivos
                echo "<div class='device-buttons' style='display: flex; justify-content: center; gap: 20px; margin: 20px 0; padding-left: 160px;'>
                        <button onclick=\"updateDevices(1)\" style='background-color: #28a745; color: white; border: none; border-radius: 15px; padding: 15px 35px; font-size: 18px; font-weight: bold; cursor: pointer; transition: all 0.3s ease; box-shadow: 0 5px 15px rgba(40, 167, 69, 0.3);'>
                            Encender todos
                        </button>
                        <button onclick=\"updateDevices(0)\" style='background-color: #dc3545; color: white; border: none; border-radius: 15px; padding: 15px 35px; font-size: 18px; font-weight: bold; cursor: pointer; transition: all 0.3s ease; box-shadow: 0 5px 15px rgba(220, 53, 69, 0.3);'>
                            Apagar todos
                        </button>
                    </div>";

                // Tabla de dispositivos con botones individuales
                if ($resultados) {
                    echo "<table>
                            <thead>
                            <tr>";
                    // Crear encabezados dinámicamente
                    foreach (array_keys($resultados[0]) as $header) {
                        echo "<th>" . htmlspecialchars($header) . "</th>";
                    }
                    echo "<th>Acciones</th>"; // Nueva columna para acciones
                    echo "  </tr>
                            </thead>
                            <tbody>";

                    // Crear filas dinámicamente
                    foreach ($resultados as $fila) {
                        // Eliminar posibles espacios y hacer la comparación estricta
                        $estadoActual = (trim($fila['estado']) === 'Encendido') ? 1 : 0;

                        // Resto del código
                        $botonTexto = $estadoActual === 1 ? "Apagar" : "Encender";
                        $botonColor = $estadoActual === 1 ? "#dc3545" : "#28a745";

                        echo "<tr>";
                        foreach ($fila as $valor) {
                            echo "<td>" . htmlspecialchars($valor) . "</td>";
                        }
                        echo "<td>
                                <button onclick=\"toggleDevice(" . htmlspecialchars($fila['id']) . ", $estadoActual)\" 
                                        style='background-color: $botonColor; color: white; border: none; border-radius: 5px; padding: 5px 10px; cursor: pointer;'>
                                        $botonTexto
                                </button>
                            </td>";
                        echo "</tr>";
                    }
                    echo "  </tbody>
                        </table>";
                } else {
                    echo "<p>No se encontraron dispositivos.</p>";
                }
            } else {

                echo "<table>
                        <thead>
                        <tr>";
                // Crear encabezados dinámicamente
                foreach (array_keys($resultados[0]) as $header) {
                    echo "<th>" . htmlspecialchars($header) . "</th>";
                }
                echo "  </tr>
                        </thead>
                        <tbody>";
                // Crear filas dinámicamente
                foreach ($resultados as $fila) {
                    echo "<tr>";
                    foreach ($fila as $valor) {
                        echo "<td>" . htmlspecialchars($valor) . "</td>";
                    }
                    echo "</tr>";
                }
                echo "  </tbody>
                    </table>";
            }
        } else {
            echo "<p>No se encontraron datos en la tabla seleccionada.</p>";
        }
    }
} catch (PDOException $e) {
    echo "Error de conexión: " . $e->getMessage();
}