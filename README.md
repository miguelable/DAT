# Servidor Web en PHP con Sockets

Este proyecto implementa un servidor web básico en PHP utilizando sockets para la comunicación. El servidor es capaz de aceptar conexiones de clientes, procesar sus solicitudes HTTP y servir archivos ubicados en su directorio. Si no se especifica un archivo, el servidor devuelve una página HTML que muestra un listado de los archivos disponibles.

## Funcionamiento General

1. **Creación del socket**: El servidor se configura para escuchar conexiones TCP en la IP `127.0.0.1` y el puerto `80`.
2. **Escuchar y aceptar conexiones**: Una vez creado y asociado el socket a una dirección IP y puerto, el servidor escucha conexiones entrantes. Cuando un cliente se conecta, el servidor acepta la conexión y gestiona la solicitud HTTP.
3. **Leer solicitudes HTTP**: La función que maneja al cliente lee la solicitud HTTP, extrae la URL solicitada y responde en consecuencia:
   - Si no se especifica ningún archivo en la URL, el servidor muestra una página principal con un listado de archivos disponibles en el directorio.
   - Si el archivo solicitado existe, el servidor lo envía en la respuesta HTTP.
   - Si el archivo no existe, el servidor responde con un error `404 Not Found`.
4. **Descarga de archivos**: Si se solicita un archivo con la opción `&download=true`, el servidor lo envía con los encabezados necesarios para forzar su descarga en lugar de mostrarlo en el navegador.
5. **Servir archivos estáticos**: El servidor puede servir archivos estáticos (HTML, CSS, Markdown, imágenes, etc.) presentes en el directorio del servidor. La respuesta incluye los encabezados HTTP correspondientes, como el tipo de contenido y la longitud del archivo.
6. **Cerrar conexión**: Después de enviar la respuesta, el servidor cierra la conexión con el cliente.

## Funciones

### 1. `handle_client($client)`

- **Descripción**: Gestiona la conexión con un cliente. Lee la solicitud HTTP, extrae los parámetros (como la URL solicitada) y responde con el contenido del archivo o un mensaje de error.
- **Entradas**:
  - `$client`: El socket del cliente.
- **Salidas**: Respuesta HTTP con el contenido solicitado o un mensaje de error.

### 2. `get_main_page()`

- **Descripción**: Genera una página HTML que lista los archivos presentes en el directorio del servidor. Esta página se muestra cuando no se especifica ningún archivo en la URL.
- **Entradas**: No tiene parámetros.
- **Salidas**: Una cadena de texto con la página HTML generada que incluye enlaces para ver o descargar archivos.

### 3. `get_content_type($url)`

- **Descripción**: Devuelve el tipo de contenido HTTP (`Content-Type`) basado en la extensión del archivo solicitado (por ejemplo, `.html`, `.css`, `.ico`, etc.).
- **Entradas**:
  - `$url`: La URL o nombre del archivo solicitado.
- **Salidas**: Una cadena de texto con el encabezado `Content-Type` adecuado para la respuesta HTTP.

## Uso

1. Ejecuta el script PHP en un servidor o entorno con soporte para sockets.
2. Abre un navegador web y accede a `http://127.0.0.1` para ver la página principal que lista los archivos disponibles en el directorio.
3. Para ver un archivo, pulsa en el enlace **Ver** junto al archivo deseado.
4. Para descargar un archivo, pulsa en el enlace **Descargar**.

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
