# Simulador de Envío de Datos al Gateway

Este script de Bash simula el envío de datos de potencia aleatorios desde una sonda a un gateway, utilizando comunicación en formato JSON. Los datos se envían a través de `curl` a un servidor local y se verifica continuamente si hay un cambio en el estado deseado de un LED en función de la respuesta del servidor.

## Requisitos

Este script requiere de las herramientas `bc`, `curl` y `jq`. Si no están instaladas, el script intentará instalarlas automáticamente.

## Funcionalidades

El script realiza las siguientes operaciones:

1. **Verificación e Instalación de Dependencias**  
   Primero, el script verifica si las herramientas `bc` y `curl` están disponibles en el sistema:

   - Si no están instaladas, las instala usando `sudo apt-get install`.

2. **Variables de Configuración**  
   Define tres variables clave:

   - `ID`: Identificador de la sonda, asignado a 48.
   - `ESTADO_LED`: Estado actual del LED (1 por defecto).
   - `AUTH_TOKEN`: Token de autorización para el envío de datos, usado para autenticar la solicitud.

3. **Simulación de Envío de Datos**  
   El script entra en un bucle infinito donde:

   - Genera un valor de potencia aleatorio entre 0 y 4095.
   - Obtiene la fecha y hora actuales en formato ISO (`YYYY-MM-DDTHH:MM:SS`).
   - Crea un objeto JSON con los datos de potencia, el estado del LED y la marca temporal.

4. **Envío de Datos al Gateway**  
   Envía los datos JSON al servidor en `https://localhost:8443/gateway.php` usando `curl` con:

   - La cabecera `Authorization: Bearer`, incluyendo el token `AUTH_TOKEN`.
   - La cabecera `Content-Type: application/json` para indicar el formato de los datos.

5. **Recepción de Respuesta y Cambio de Estado del LED**  
   Procesa la respuesta del servidor para verificar si hay un cambio en el estado deseado del LED:

   - Extrae el valor `desired_status` de la respuesta JSON.
   - Si `desired_status` es diferente del estado actual (`ESTADO_LED`), actualiza el estado del LED y muestra un mensaje de cambio.

6. **Intervalo de Ejecución**  
   El script espera 2 segundos antes de iniciar el siguiente ciclo de envío de datos.

## Ejecución

Para ejecutar el script, asegúrate de tener permisos de administrador para instalar las dependencias. Ejecuta el script con:

```bash
cd Sensor
./enviar_datos.sh
```
