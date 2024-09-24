# Práctica 1: Programación de Servidor Web
Este proyecto implementa un servidor y un cliente en PHP para manejar descargas de archivos a través de sockets TCP. El servidor responde a solicitudes HTTP GET enviadas por el cliente para descargar archivos desde su directorio base.

## Descripción de los Archivos y Funciones
### `main.php`
Archivo principal para ejecutar tanto el cliente como el servidor según los argumentos proporcionados.

#### Modo Servidor:
Inicia el servidor en `127.0.0.1` y el puerto `8888` con el comando:

```bash
php main.php --server
```

#### Modo Cliente:
Inicia el cliente en la IP y puertos indicados en la URL ( o con los valores del servidor si no se especifican ). Además extrae la ruta y el nombre del archivo a descragar. El comando en este caso será:

```bash
php main.php http://127.0.0.1:8888/archivo.txt descargado.txt
```

### `client.php`
Contiene la lógica del cliente para conectarse al servidor y descargar archivos.

#### Función Principal: `run_client($ip, $puerto, $path, $output_file)`
- **Descripción**: Crea un socket TCP, se conecta al servidor, envía una solicitud GET para el archivo especificado y guarda la respuesta en un archivo local.
- **Parámetros**:
  - `$ip`: Dirección IP del servidor.
  - `$puerto`: Puerto en el que escucha el servidor.
  - `$path`: Ruta del archivo solicitado en el servidor.
  - `$output_file`: Nombre del archivo donde se guardará el contenido descargado.

### `server.php`
Implementa el servidor que gestiona las solicitudes de los clientes.

#### Función Principal: `run_server($ip, $puerto)`
- **Descripción**: Configura y ejecuta el servidor que escucha conexiones entrantes en la dirección IP y puerto especificados.
- **Parámetros**:
  - `$ip`: Dirección IP en la que el servidor escuchará las conexiones.
  - `$puerto`: Puerto en el que el servidor escuchará conexiones.

#### Función Auxiliar: `handle_client($client)`
- **Descripción**: Gestiona cada conexión de cliente. Lee la solicitud HTTP, verifica si el archivo solicitado existe y lo envía al cliente o devuelve un mensaje de error.
- **Parámetro**:
  - `$client`: El socket del cliente.

#### Función Auxiliar: `extract_header($request)`
- **Descripción**: Extrae la información relevante de la solicitud HTTP, como la ruta del archivo solicitado.
- **Parámetro**:
  - `$request`: La solicitud HTTP completa como cadena de texto.
- **Salida**: Array asociativo con los detalles extraídos de la solicitud.


## Uso

1. Ejecuta el servidor mediente la consola con php main.php --server.
2. Ejecuta la petición en otro terminal indicando IP, puerto, directorio y nombre del archivo que desea a la salida : php main.php http://127.0.0.1:8888/archivo.txt descargado.txt
3. Comprobación de la descarga en la carpeta local.

### Ejemplo:

- Desde un terminal ejecuta el comando: php main.php --server
- Desde un segundo terminal ejecuta: php main.php http://127.0.0.1:8888/archivo.txt
- Al no indicar el segundo argumento , el cual corresponde con el nombre deseado para el archivo descargado, se tomara el nombre del archivo original.

## Autores

- **Miguel Ferrer**
- **Paula Fernández**

**Fecha**: 24 de septiembre de 2024
