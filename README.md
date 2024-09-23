# Práctica 1: Programación de Servidor Web
Este proyecto implementa un servidor y un cliente en PHP para manejar descargas de archivos a través de sockets TCP. El servidor responde a solicitudes HTTP GET enviadas por el cliente para descargar archivos desde su directorio base.

## Funcionamiento General

1. **Creación del socket**: El servidor se configura para escuchar conexiones TCP en la IP `127.0.0.1` y el puerto `80`.
2. **Escuchar y aceptar conexiones**: Una vez creado y asociado el socket a una dirección IP y puerto, el servidor escucha conexiones entrantes. Cuando un cliente se conecta, el servidor acepta la conexión y gestiona la solicitud HTTP.
3. **Leer solicitudes HTTP**: La función que maneja al cliente lee la solicitud HTTP, extrae la URL solicitada y responde en consecuencia:
   - Si el archivo solicitado existe, el servidor lo envía en la respuesta HTTP.
   - Si el archivo no existe, el servidor responde con un error `404 Not Found`.
4. **Descarga de archivos**: El cliente solicita un archivo mediente una URL de la forma:  http://IP:PUERTO/ruta/al/archivo.txt archivo.txt , siendo la ip y el puerto las del servidor. El segundo argumento es el nombre del archivo descargado por el cliente.
5. **Servir archivos estáticos**: El servidor puede servir archivos presentes en el directorio del servidor. La respuesta incluye los encabezados HTTP correspondientes, como el tipo de contenido y la longitud del archivo.
6. **Cerrar conexión**: Después de enviar la respuesta, el servidor cierra la conexión con el cliente.

## Funciones

### 1. `handle_client($client)`

- **Descripción**: Gestiona la conexión con un cliente. Lee la solicitud HTTP, extrae los parámetros (como la URL solicitada) y responde con el contenido del archivo o un mensaje de error.
- **Entradas**:
  - `$client`: El socket del cliente.
- **Salidas**: Respuesta HTTP con el contenido solicitado o un mensaje de error.

### 2. `run_client($ip, $puerto, $path, $output_file)`

- **Descripción**: Gestiona la conexión con un cliente. Lee la solicitud HTTP, extrae los parámetros (como la URL solicitada) y responde con el contenido del archivo o un mensaje de error.
- **Entradas**:
  - $ip: Dirección IP del servidor al que conectarse.
  - $puerto: Puerto en el que escucha el servidor.
  - $path: Ruta del archivo solicitado en el servidor.
  - $output_file: Nombre del archivo donde se guardará el contenido descargado.
- **Salidas**: Respuesta HTTP con el contenido solicitado o un mensaje de error.
- 
### 3. `run_server($ip, $puerto)`

- **Descripción**: Configura y ejecuta un servidor que escucha conexiones entrantes en una dirección IP y puerto especificados. Acepta las conexiones de los clientes y llama a handle_client() para gestionar cada solicitud.
- **Entradas**:
  - $ip: Dirección IP en la que el servidor escuchará las conexiones.
  - $puerto: Puerto en el que el servidor escuchará conexiones.
- **Salidas**: Respuesta HTTP con el contenido solicitado o un mensaje de error.

### 4. `extract_header($request)`

- **Descripción**: Extrae la información relevante de la solicitud HTTP recibida, como la ruta solicitada. Esta información se utiliza para identificar qué archivo debe ser servido al cliente.
- **Entradas**:
  - $request: La solicitud HTTP completa enviada por el cliente, como una cadena de texto.
- **Salidas**:  Un array asociativo que contiene detalles extraídos de la solicitud


## Uso

1. Ejecuta el servidor mediente la consola con php main.php --server.
2. Ejecuta la petición en otro terminal : php main.php http://127.0.0.1:8888/archivo.txt descargado.txt
3. Comprobación de la descarga 

### Ejemplos:

- Accede a `http://127.0.0.1` para ver la página de archivos.
- Accede a `http://127.0.0.1/archivo.txt` para visualizar un archivo específico.
- Accede a `http://127.0.0.1/archivo.txt&download=true` para descargar un archivo.

## Mejoras Recientes

- La página principal ahora carga un archivo CSS externo para estilos más limpios y separa el código de presentación.
- Se optimizó la función `get_main_page` para reducir su complejidad y mejorar la legibilidad del código.
- Se mejoró el manejo de los encabezados HTTP, como el tipo de contenido y las opciones de descarga.

## Autores

- **Miguel Ferrer**
- **Paula Fernández**

**Fecha**: 16 de septiembre de 2024
