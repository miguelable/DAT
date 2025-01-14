// Include the libraries we need
#include "esp32cam.h"
#include "secrets.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <NTPClient.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <WiFiUdp.h>

// WiFi credentials
WiFiClientSecure client;
IPAddress        serverIP;

// Configuración de NTP
WiFiUDP   ntpUDP;
NTPClient timeClient(ntpUDP, ntpServer, gmtOffset_sec);

// // Configuración de Wi-Fi
// const char* ssid     = "Pixel_3021";
// const char* password = "miguelferrer";

WiFiServer mjpegServer(81); // Puerto para la transmisión MJPEG
ESP32Cam   camera;          // Cámara ESP32

void setup()
{
  Serial.begin(115200);

  // Conexión Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado.");
  Serial.println("IP asignada: ");
  Serial.println(WiFi.localIP());

  // Inicializar cámara
  camera.init();

  // Iniciar servidor MJPEG
  mjpegServer.begin();
  Serial.println("Servidor MJPEG iniciado en el puerto 81.");
}

void loop()
{
  WiFiClient client = mjpegServer.available(); // Espera un cliente
  if (client) {
    Serial.println("Cliente conectado.");
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: multipart/x-mixed-replace; boundary=frame");
    client.println();

    while (client.connected()) {
      // Captura una imagen
      camera_fb_t* fb = camera.getImage();
      if (!fb)
        break;

      // Encabezado del frame MJPEG
      client.printf("--frame\r\n");
      client.printf("Content-Type: image/jpeg\r\n");
      client.printf("Content-Length: %u\r\n\r\n", fb->len);
      client.write(fb->buf, fb->len); // Envía la imagen
      client.println();

      camera.returnBuffer(); // Devuelve el buffer de la imagen
      delay(50);             // Controla la tasa de cuadros (ajusta para mayor fluidez)
    }
    client.stop();
    Serial.println("Cliente desconectado.");
  }
}
