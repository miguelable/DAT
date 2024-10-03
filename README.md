# Práctica 2: Programación de servicio P2P

Este proyecto implementa un sistema de comunicación entre clientes (peers) con la idea de Napster. Cada cliente tiene su propio directorio con archivos compartidos, y se generan múltiples procesos hijos para simular distintos clientes conectándose a un servidor central.

## Características
- Creación automática de carpetas para cada cliente (directorios `shared` y `download`).
- Ejecución de clientes en procesos hijos usando `pcntl_fork()`.
- Funcionalidad para compartir archivos entre los clientes.
- Gestión de direcciones IP para simular múltiples clientes conectados.

## Comandos para el correcto funcionamiento del proyecto
### 1. Levantar el servidor:
```bash
php server.php
```
El servidor se mantendrá escuchando con la IP 127.0.0.1 en el puerto 8888

### 2. Crear los clientes:
```bash
php config.php
```
El archivo de configuración se encargará de crear una carpeta llamada users que contendrá 10 subcarpetas, una para cada usuario. Dentro de cada subcarpeta de usuario, habrá una carpeta `shared`, donde se almacenarán los archivos compartidos, y una carpeta `download`, destinada a los archivos descargados. Además, cada subcarpeta incluirá un archivo PHP con la configuración específica del usuario.

### 2. Conexión de un cliente:
```bash
php users/client0/user.php
```
Se pondrá en marcha uno de los clientes donde tendrá opción de ejecutar distintos comandos:

```bash
> ? 
```
Muestra la lista de comandos disponibles
```bash
> search           <arg>   
```
Busca todos los archivos que contienen <arg>
```bash
> download         <file>
```
Descarga un archivo
```bash
> host_files       <ip>  
```
Obtiene la lista de archivos de un host
```bash
> hosts
```
Obtiene la lista de hosts
```bash
> exit  
```
Sale del programa

## Autores

- **Miguel Ferrer**
- **Paula Fernández**

**Fecha**: 16 de septiembre de 2024
