# Servidor Web en PHP con Sockets

Este proyecto implementa un servidor web básico en PHP utilizando sockets para la comunicación. El servidor es capaz de aceptar conexiones de clientes, procesar sus solicitudes HTTP y servir archivos ubicados en su directorio. Si no se especifica un archivo, el servidor devuelve una página que muestra un listado de los archivos disponibles.

## Funcionamiento General

1. **Creación del socket**: El servidor se configura para escuchar conexiones TCP en la IP `127.0.0.1` y el puerto `80`.
2. **Escuchar y aceptar conexiones**: Una vez creado y asociado el socket a una dirección IP y puerto, el servidor escucha conexiones entrantes. Cuando un cliente se conecta, el servidor acepta la conexión y gestiona la solicitud HTTP.

3. **Leer solicitudes HTTP**: La función encargada de manejar al cliente lee la solicitud HTTP, extrae la URL solicitada y responde en consecuencia:

   - Si el archivo solicitado existe, el servidor envía su contenido.
   - Si el archivo no existe, el servidor responde con un error `404 Not Found`.
   - Si no se especifica un archivo, el servidor responde con una página HTML que lista los archivos disponibles en el directorio.

4. **Servir archivos**: El servidor es capaz de servir archivos estáticos (como HTML, iconos o markdown) presentes en el mismo directorio donde está el servidor. La respuesta incluye los encabezados HTTP correspondientes, como el tipo de contenido y la longitud.

5. **Cerrar conexión**: Después de enviar la respuesta, el servidor cierra la conexión con el cliente.

## Funciones

### 1. `handle_client($client)`

- **Descripción**: Maneja la conexión con un cliente. Lee la solicitud HTTP, extrae los parámetros y responde con el contenido de un archivo o un mensaje de error.
- **Entradas**:
  - `$client`: El socket del cliente.
- **Salidas**: Respuesta HTTP con el contenido solicitado o un mensaje de error.

### 2. `get_main_page()`

- **Descripción**: Genera y devuelve una página HTML que lista los archivos en el directorio del servidor. Esta página se envía cuando no se especifica ningún archivo en la solicitud del cliente.
- **Entradas**: No tiene parámetros.
- **Salidas**: Cadena de texto con la página HTML generada.

## Uso

1. Ejecuta el script PHP en un servidor o entorno con soporte para sockets.
2. Abre un navegador y accede a `http://127.0.0.1` para ver la página principal que lista los archivos.
3. Si deseas acceder a un archivo específico, puedes hacerlo añadiendo la ruta del archivo en la URL.

## Autores

- **Miguel Ferrer**
- **Paula Fernández**

**Fecha**: 14 de septiembre de 2024
