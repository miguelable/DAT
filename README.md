# Práctica 2: Programación de servicio P2P

Este proyecto implementa un sistema de comunicación entre clientes (peers) con la idea de Napster. Cada cliente tiene su propio directorio con archivos compartidos, y se generan múltiples procesos hijos para simular distintos clientes conectándose a un servidor central.

## Características
- Creación automática de carpetas para cada cliente (directorios `shared` y `download`).
- Ejecución de clientes en procesos hijos usando `pcntl_fork()`.
- Funcionalidad para compartir archivos entre los clientes.
- Gestión de direcciones IP para simular múltiples clientes conectados.

## Descripción de los Archivos y Funciones
### `server.php`
El servidor PHP implementa un sistema de gestión de archivos en red, permitiendo a múltiples clientes conectarse y compartir archivos. El servidor escucha conexiones entrantes en una dirección IP y un puerto específicos, utilizando sockets TCP. Al aceptar una conexión de un cliente, se crea un nuevo proceso hijo para manejar las interacciones con ese cliente. Además, otro proceso hijo se encarga de eliminar periódicamente la lista de clientes en memoria compartida.

El servidor utiliza memoria compartida para mantener un registro de los clientes conectados y sus archivos asociados. Los clientes pueden enviar solicitudes para subir archivos (método PUT), obtener información sobre otros clientes conectados (método GET), buscar archivos específicos y obtener la lista de pares que poseen un archivo determinado.

#### Configuración inicial: 
- Se definen las direcciones IP y puertos para el cliente y el servidor, así como los directorios de descarga y compartición de archivos.
- Se inicializa un socket para la conexión.

#### Función Principal: `handle_client($client, $shm_id)`
Maneja la comunicación con un cliente conectado. Lee las solicitudes del cliente y ejecuta acciones según el método y los parámetros proporcionados. Esta función procesa las solicitudes de tipo PUT y GET, dirigiendo a las funciones específicas para manejar cada caso.

#### Funciones espefíficas para métodos: 
- `get_hosts_method($client, $shm_id)`: Responde a la solicitud de listar todos los clientes conectados. Obtiene la lista de clientes desde la memoria compartida y envía una respuesta al cliente que hizo la solicitud.
- `get_hosts_files($client, $shm_id, $ip_client)`: Devuelve la lista de archivos asociados a un cliente específico, identificado por su dirección IP. Busca en la lista de clientes en memoria compartida y compone una respuesta que incluye los archivos del cliente.
- `get_peers_method($file_name, $client, $shm_id)`: Filtra y responde con las direcciones IP de los clientes que poseen un archivo específico. Si no se encuentran clientes que posean el archivo, envía un mensaje de error al cliente solicitante.
- `get_search_method($file_name, $client, $shm_id)`: Realiza una búsqueda en los archivos de todos los clientes conectados. Si se encuentran coincidencias, devuelve una lista de los archivos disponibles que contienen el fragmento de nombre de archivo especificado.
- `manage_client_files($client, $client_ip, $file_json, $shm_id)`: Gestiona la subida de archivos de un cliente (método PUT). Decodifica un JSON recibido que contiene los archivos del cliente y actualiza la lista de clientes en memoria compartida.

#### Funciones Auxiliares y de Gestión: 
- `erase_client_list($shm_id)`: Función que se ejecuta en un proceso hijo separado. Elimina la lista de clientes de la memoria compartida cada 60 segundos, asegurando que los datos antiguos no permanezcan en el sistema.
- `send_response_to_client($client, $response)`: Envía una respuesta al cliente a través del socket. Maneja errores de escritura y asegura que el mensaje se envíe correctamente.
- `add_new_client($client_ip, $client_files, $shm_id)`: Añade un nuevo cliente a la lista en memoria compartida. Crea una instancia de la clase Client y la almacena en la memoria.
- `update_or_add_client($client_ip, $client_files, $shm_id)`: Actualiza la lista de archivos de un cliente existente o añade un nuevo cliente si no se encuentra uno con la misma IP. Maneja el registro de la conexión de los clientes.

### `user.php`
Implementa un cliente que se conecta a un servidor mediante sockets, permitiendo el intercambio y la descarga de archivos. Se establecen procesos hijos para manejar tareas específicas, como enviar archivos, recibir solicitudes de clientes y gestionar comandos de terminal. El cliente está diseñado para enviar y recibir datos utilizando el protocolo HTTP, gestionando errores y manteniendo un registro de las operaciones.

#### Configuración inicial: 
- Se definen las direcciones IP y puertos para el cliente y el servidor, así como los directorios de descarga y compartición de archivos.
- Se inicializa un socket para la conexión.

#### Funciones de Gestión de procesos Hijos: 
- `create_child_process($task, $error_message)`: Crea un proceso hijo para ejecutar una tarea específica. Maneja errores de creación de procesos.
- `task_file_sending_loop()`: Llama a file_sending_loop() en un proceso hijo.
- `task_terminal_loop()`: Llama a terminal_loop() en un proceso hijo.
- `task_client_requests()`: Llama a client_loop() en un proceso hijo.

#### Funciones de Gestión de Sockets:
 - `create_socket($ip, $port, $server_ip, $server_port, $isReadable)`: Crea y conecta un socket. Puede ser para un cliente o para escuchar conexiones entrantes.
- `create_socket_r($ip, $port, $isReadable)`: Función auxiliar para crear un socket de lectura.
- `handle_socket_error($socket, $message)`: Maneja errores relacionados con sockets y registra mensajes de error.
- `send_http_request($socket, $request)`: Envía una solicitud HTTP a través del socket y lee la respuesta del servidor.
- `read_server_response($sock)`: Lee la respuesta del servidor para una solicitud dada.

#### Funciones de Gestión de Métodos:
- `get_shared_files()`: Obtiene y filtra la lista de archivos disponibles en el directorio compartido.
- `send_shared_files($shared_files)`: Envía la lista de archivos compartidos al servidor mediante una solicitud HTTP.
- `file_sending_loop()`: Bucle continuo que envía archivos compartidos al servidor cada 10 segundos para las peticiones PUT.
- `searchFile($args)`: Envía una solicitud para buscar archivos que coincidan con el argumento proporcionado.
- `downloadFile($file)`: Envía una solicitud para descargar un archivo y gestiona la conexión con los peers que lo comparten.
- `getHosts()`: Envía una solicitud para obtener la lista de hosts conectados.
- `getHostFiles($host, $port)`: Solicita la lista de archivos de un host específico.
- `downloadFile($file)`: Envía una solicitud para descargar un archivo y gestiona la conexión con los peers que lo comparten.
- `saveDownloadedFile($file, $response)`: Guarda el archivo descargado en los directorios designados tanto para descargas como para compartir.
- `saveFile($file_path, $content)`: Función auxiliar que guarda contenido en un archivo, manejando errores.

#### Funciones para el manejo de clientes:
- `client_loop()`: Acepta conexiones entrantes de clientes y maneja sus solicitudes en un nuevo proceso hijo.
- `handle_client($client)`: Maneja la petición de un cliente, verificando si el archivo solicitado está disponible y respondiendo adecuadamente.

### `config.php`
El archivo config.php configura el entorno para varios clientes en un sistema PHP. Primero, verifica y crea un directorio llamado users si no existe. Luego, utiliza un bucle para generar diez procesos hijos (de 127.0.0.100 a 127.0.0.109) mediante pcntl_fork(), cada uno de los cuales crea un archivo de configuración específico con su dirección IP. La función crearFichero($ip) es responsable de crear los directorios necesarios para descargas y archivos compartidos, así como de generar un archivo PHP (user.php) y un archivo de texto en la carpeta correspondiente del cliente. El proceso padre registra la creación de cada hijo y espera a que terminen, registrando su estado de salida.

### `logs.php`
El archivo contiene funciones de logging que permiten registrar mensajes de diferentes niveles (error, advertencia, información, depuración y verbose) en la consola. Cada función verifica si la constante LOG_ON está definida y activa antes de imprimir el mensaje, aplicando un color específico para cada tipo de log: rojo para errores, amarillo para advertencias, verde para información, azul para depuración y blanco para mensajes verbosos.

## Uso
### 1. Levantamiento del servidor:
```bash
php server.php
```
El servidor estará en escucha continua en la IP 127.0.0.1 y el puerto 8888, ejecutándose de forma permanente en un terminal independiente.

### 2. Creación de los clientes:
```bash
php config.php
```
El archivo de configuración se encargará de crear una carpeta llamada users que contendrá 10 subcarpetas, una para cada usuario. Dentro de cada subcarpeta de usuario, habrá una carpeta `shared`, donde se almacenarán los archivos compartidos, y una carpeta `download`, destinada a los archivos descargados. Además, cada subcarpeta incluirá un archivo PHP con la configuración específica del usuario.

### 3. Conexión de un cliente:
```bash
php users/client0/user.php
```
Se pondrá en marcha uno de los clientes donde tendrá opción de ejecutar distintos comandos:

- Mostrar la lista de comandos disponibles
```bash
> ? 
```
- Buscar todos los archivos que contienen arg
```bash
> search           <arg>   
```
- Descargar un archivo
```bash
> download         <file>
```
- Obtener la lista de archivos de un host
```bash
> host_files       <ip>  
```
- Obtener la lista de hosts
```bash
> hosts
```
- Salir del programa
```bash
> exit  
```

Se permite la conexión simultánea de hasta 10 clientes, cada uno de los cuales podrá ejecutar sus comandos de manera independiente en diferentes terminales.

## Autores

- **Miguel Ferrer**
- **Paula Fernández**

**Fecha**: 3 de octubre de 2024

