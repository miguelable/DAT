# This script sets up a MySQL database, user, and table for storing sensor data.
#
# Configuration Variables:
# - DB_HOST: The hostname of the MySQL server.
# - DB_NAME: The name of the database to be created.
# - DB_USER: The username for the database user.
# - DB_PASS: The password for the database user.
# - TABLE_NAME: The name of the table to be created.
#
# SQL Commands:
# - Creates the database if it does not exist.
# - Uses the created database.
# - Creates a table with the specified columns if it does not exist.
# - Creates a user with the specified username and password if it does not exist.
# - Grants all privileges on the database to the created user.
# - Flushes the privileges to ensure that all changes take effect.
#
# SQL Commands for Verification:
# - Selects and displays the MySQL users.
# - Uses the created database.
# - Shows the tables in the database.
# - Describes the structure of the created table.
# - Selects and displays all data from the created table.
#
# Execution:
# - Prompts the user to enter the MySQL root password.
# - Executes the SQL commands to set up the database, user, and table.
# - If the setup is successful, executes the SQL commands for verification.
# - If the setup fails, drops the created database and user, and displays an error message.
#!/bin/bash

# Variables de configuración
DB_HOST="localhost"         # Cambiar localhost por el nombre del servidor
DB_NAME="grupo03"          # Cambiar nombre si es necesario
DB_USER="grupo03"
DB_PASS="Hola1234+"
TABLE_NAME="POT_SENSOR"

# Comandos SQL para crear la base de datos, el usuario y la tabla
SQL_COMMANDS=$(cat <<EOF
CREATE DATABASE IF NOT EXISTS $DB_NAME;
USE $DB_NAME;
CREATE TABLE IF NOT EXISTS $TABLE_NAME (
    id INT AUTO_INCREMENT PRIMARY KEY,
    id_sonda INT NOT NULL,
    potencia INT NOT NULL,
    tiempo_muestra TIMESTAMP NOT NULL,
    estado_led BOOLEAN NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);
CREATE USER IF NOT EXISTS '$DB_USER'@'$DB_HOST' IDENTIFIED BY '$DB_PASS';
GRANT ALL PRIVILEGES ON $DB_NAME.* TO '$DB_USER'@'$DB_HOST';
FLUSH PRIVILEGES;
EOF
)

# Comandos SQL para mostrar la estructura de la tabla y sus datos
SHOW_TABLE_STRUCTURE=$(cat <<EOF
SELECT User, Host FROM mysql.user;
USE $DB_NAME;
SHOW TABLES;
DESCRIBE $TABLE_NAME;
SELECT * FROM $TABLE_NAME;
EOF
)

# Solicitar la contraseña una vez
read -sp 'Enter password: ' MYSQL_PWD

# Ejecutar los comandos SQL
sudo mysql -u root -p"$MYSQL_PWD" -e "$SQL_COMMANDS"

# Verificar si los comandos se ejecutaron correctamente
if [ $? -eq 0 ]; then
    echo "Base de datos, usuario y tabla creados exitosamente."
    # Ejecutar los comandos SQL para mostrar la estructura de la tabla y sus datos
    sudo mysql -u root -p"$MYSQL_PWD" -e "$SHOW_TABLE_STRUCTURE"
else
    echo "Error al crear la base de datos, el usuario o la tabla."
    # Eliminar la base de datos y el usuario
    sudo mysql -u root -p"$MYSQL_PWD" -e "DROP DATABASE IF EXISTS $DB_NAME; DROP USER IF EXISTS '$DB_USER'@'$DB_HOST';"
    echo "Base de datos y usuario eliminados."
fi