#!/bin/bash

# Variables de configuración
DB_HOST="localhost"
DB_NAME="t_h_DataBase"
DB_USER="grupo03"
DB_PASS="Hola1234+"
TABLE_NAME="data"

# Comandos SQL para crear la base de datos, el usuario y la tabla
SQL_COMMANDS=$(cat <<EOF
CREATE DATABASE IF NOT EXISTS $DB_NAME;
USE $DB_NAME;
CREATE TABLE IF NOT EXISTS $TABLE_NAME (
    id INT AUTO_INCREMENT PRIMARY KEY,
    id_sonda INT NOT NULL,
    temperatura FLOAT NOT NULL,
    humedad FLOAT NOT NULL,
    tiempo_muestra TIMESTAMP NOT NULL,
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