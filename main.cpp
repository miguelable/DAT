// App to subscribe to publish and subscribe to a mqtt topic
// Publish data from the Gateway to the MQTT broker
// mosquitto_pub -h localhost -p 8883 --cafile /etc/mosquitto/certs/mqtt-server.crt -t grupo01/4636/desiredStatus -m
// "{"desired_status":0}"

// Subscribe to the topic on the Gateway
// mosquitto_sub -h localhost -p 8883 --cafile /etc/mosquitto/certs/mqtt-server.crt --insecure -t grupo01/4636/potValue
// mosquitto_sub -h localhost -p 8883 --cafile /etc/mosquitto/certs/mqtt-server.crt --insecure -t grupo01/4636/ledStatus

// Publish data to the device to change led
// mosquitto_pub -h webalumnos.tlm.unavarra.es -p 10320 -t 'grupo01/4636/desiredStatus' -m '{"desired_status":0}'
// mosquitto_pub -h webalumnos.tlm.unavarra.es -p 10320 -t 'grupo01/4636/desiredStatus' -m '{"desired_status":1}'

#define LED_RGB

#ifdef LED_RGB
#include <Arduino.h>
#include <NeoPixelBus.h>
#endif
#include <ArduinoJson.h>
#include <NTPClient.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <WiFiUdp.h>
#include <freertos/semphr.h>

#ifdef LED_RGB
#define PIXEL_COUNT 1
#endif
#define POT_PIN 34
#define LED_PIN 4
#define ID 4636
#define MQTTSERVER "192.168.8.1"
#define MQTTPORT 8883
#ifndef LED_RGB
#define THRESHOLD 2048
#endif
// wifi credentials
const char*      ssid     = "GL-MT300N-V2-0bb";
const char*      password = "goodlife";
WiFiClientSecure client;
IPAddress        serverIP = IPAddress(192, 168, 8, 1);

// MQTT client
PubSubClient mqtt(client);

// Topics to publish
String potValueTopic  = String("grupo01/") + ID + "/potValue";
String ledStatusTopic = String("grupo01/") + ID + "/ledStatus";

// Topics to subscribe
String desiredStatusTopic = String("grupo01/") + ID + "/desiredStatus";

// Certificado del servidor (certificado público)
const char* ca_Cert = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDpzCCAo8CFHhZVxXafcgMu5vPsHLZYBpoj8uqMA0GCSqGSIb3DQEBCwUAMIGP
MQswCQYDVQQGEwJFUzEQMA4GA1UECAwHTmF2YXJyYTERMA8GA1UEBwwIUGFtcGxv
bmExDTALBgNVBAoMBFVQTkExDDAKBgNVBAsMA0RBVDESMBAGA1UEAwwJbG9jYWxo
b3N0MSowKAYJKoZIhvcNAQkBFhtmZXJyZXIuMTIxMjQyQGUudW5hdmFycmEuZXMw
HhcNMjQxMjEyMTcyNTIzWhcNMjUxMjEyMTcyNTIzWjCBjzELMAkGA1UEBhMCRVMx
EDAOBgNVBAgMB05hdmFycmExETAPBgNVBAcMCFBhbXBsb25hMQ0wCwYDVQQKDARV
UE5BMQwwCgYDVQQLDANEQVQxEjAQBgNVBAMMCWxvY2FsaG9zdDEqMCgGCSqGSIb3
DQEJARYbZmVycmVyLjEyMTI0MkBlLnVuYXZhcnJhLmVzMIIBIjANBgkqhkiG9w0B
AQEFAAOCAQ8AMIIBCgKCAQEAtqonOOWPfm8oxMTxRdAkGupXdj7iThpm7hCYCdaC
Uv7787Yv7wO8DMNXYu79xXUd4G7iVjDx+p5RWojBUkU26qWve9gbaIJa9J56cBeP
naEjQ6xRTJq6D7u6TKD8S18XFP/4NEsZuPUp3+cL5K+s/CDeMTF5woDPtEiYg9cz
Cyf7py3N3qQ57MIfxI3aJNf/xA/c+n3Neqo25xGHxoPDVsDvNLr0s2EVWbJXhaIh
NmXtHAs50ftBFJxYHRoZLjr4JoOPFxwh4PIsuZadPrajTNyKb7Q3KLLGGB8Ea/m1
TI+wUxwHx5np45OdwiC9QfZweWmSu/FB/3mgLD51+eojOQIDAQABMA0GCSqGSIb3
DQEBCwUAA4IBAQAtEDgbI77onhbQzyQ1GVpN7K5USky1gBTPZoaywg2GSEZbYmUk
4yJ5J+EimbSDtMEPg5Db+gEVJG/HYBiqsYur7pmEgyR1Z2U0+TZgiVX9GyhomlSg
u7KntSGJrKCLkIzIYa4WmbVF3YcX54PlLLiJqK+u2omZTx355RJllmbl+zh2Q5/i
MEabp2xaIZhU4VdU0X4UpPNNGh+J4yqugRNs1FsIPzwNT6rLf7EGq9HJb4TLzXwh
8nRlkTA5WvARLNR4kHjvlApgkZF+aCgXF3rKys+fJCssoZOB1FJ7dsd1BCeD/0Fi
a6Tzmg6mrfc/Z8KGf6Tjl1R5KFY/FGrqJnRd
-----END CERTIFICATE-----
)EOF";

#ifdef LED_RGB
// Color deseado
RgbColor actualColor(0, 0, 0);
// Configurar el objeto de tira de LED
NeoPixelBus<NeoGrbFeature, NeoEsp32I2s1800KbpsMethod> strip(PIXEL_COUNT, LED_PIN);
#endif
// Variable para controlar el estado del LED
bool ledOn = true;

// Configuración de NTP
const char* ntpServer     = "pool.ntp.org";
const long  gmtOffset_sec = 3600; // Ajuste de zona horaria (Ejemplo: GMT+1 Madrid)
WiFiUDP     ntpUDP;
NTPClient   timeClient(ntpUDP, ntpServer, gmtOffset_sec);

// Variables para calcular la media
uint32_t sumPotValues = 0;
uint32_t numReadings  = 0;

// Task handles
TaskHandle_t readPotTaskHandle    = NULL;
TaskHandle_t updateTimeTaskHandle = NULL;
TaskHandle_t sendDataTaskHandle   = NULL;

#ifdef LED_RGB
// Función para obtener el color en función del valor del potenciómetro
void setColor(uint16_t potValue)
{
  // Calcular los componentes RGB basados en una progresión lineal
  uint8_t red   = map(potValue, 0, 4095, 0, 255);
  uint8_t green = map(potValue, 0, 4095, 255, 0);
  uint8_t blue  = 0; // No hay componente azul en la progresión verde-rojo

  RgbColor desiredColor(red, green, blue);
  bool     colorChanged = false;

  if (desiredColor != actualColor) {
    actualColor  = desiredColor;
    colorChanged = true;
    strip.SetPixelColor(0, actualColor);
    strip.Show();
  }
}
#endif

// Tarea para leer el valor del potenciómetro
void readPotTask(void* parameter)
{
  while (true) {
    uint16_t potValue = analogRead(POT_PIN);
    sumPotValues += potValue;
    numReadings++;
    uint16_t averagePotValue = sumPotValues / numReadings;
    // Serial.printf(">Pot value: %d\n", potValue);
    // Serial.printf(">Average: %d\n", averagePotValue);
#ifdef LED_RGB
    if (ledOn)
      setColor(potValue);
#else
    // Control del LED según el estado de ledOn
    if (ledOn) {
      digitalWrite(LED_PIN, HIGH); // Encender el LED
    }
    else {
      digitalWrite(LED_PIN, LOW); // Apagar el LED
    }
#endif
    vTaskDelay(100);
  }
}

// Tarea para actualizar la hora
void updateTimeTask(void* parameter)
{
  while (true) {
    timeClient.update();
    // Serial.printf("Hora actual: %s\n", timeClient.getFormattedTime().c_str());
    vTaskDelay(1000);
  }
}

// Function to connect to the MQTT broker
void connectToMQTT()
{
  while (!mqtt.connected()) {
    Serial.println("Conectando al servidor MQTT...");

    if (mqtt.connect("ESP32Client")) {
      Serial.println("Conectado al servidor MQTT");
      mqtt.subscribe(desiredStatusTopic.c_str()); // Subscribe to the topic
    }
    else {
      Serial.print("Error al conectar al servidor MQTT: ");
      Serial.println(mqtt.state());
      delay(5000); // Wait before retrying
    }
  }
}

// Tarea para enviar datos al servidor
void sendDataTask(void* parameter)
{
  while (true) {
    // Enviar datos del pot por MQTT
    StaticJsonDocument<200> potValueDoc;
    potValueDoc["id_sonda"]  = ID;
    potValueDoc["potencia"]  = sumPotValues / numReadings;
    potValueDoc["timestamp"] = timeClient.getEpochTime();
    // Reiniciar la suma y el contador
    sumPotValues = 0;
    numReadings  = 0;

    String potValueJson;
    serializeJson(potValueDoc, potValueJson);

    if (!client.connected()) {
      connectToMQTT(); // Reconnect if the connection drops
    }

    // Publicar el json con valor del potenciómetro
    if (!mqtt.publish(potValueTopic.c_str(), potValueJson.c_str())) {
      Serial.println("Error al publicar el mensaje");
    }
    else
      Serial.println("Datos enviados al servidor: " + potValueJson);

    vTaskDelay(1000);

    // Enviar el estado del LED por MQTT
    StaticJsonDocument<200> ledStatusDoc;
    ledStatusDoc["id_sonda"]   = ID;
    ledStatusDoc["led_status"] = ledOn ? "1" : "0";
    String ledStatusJson;
    serializeJson(ledStatusDoc, ledStatusJson);

    if (!mqtt.publish(ledStatusTopic.c_str(), ledStatusJson.c_str())) {
      Serial.println("Error al publicar el mensaje");
    }
    else
      Serial.println("Estado del LED enviado al servidor: " + ledStatusJson);
    vTaskDelay(10000);
  }
}

// Callback para manejar mensajes MQTT
void mqttCallback(char* topic, byte* payload, unsigned int length)
{
  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println("Mensaje recibido: " + message);

  StaticJsonDocument<200> doc;
  deserializeJson(doc, message);
  bool desired_status = doc["desired_status"];
  if (desired_status != ledOn) {
    ledOn = desired_status;
    Serial.printf("Cambiando estado del LED a: %s\n", ledOn ? "ON" : "OFF");
#ifdef LED_RGB
    if (ledOn) {
      strip.SetPixelColor(0, actualColor);
      strip.Show();
    }
    else {
      strip.SetPixelColor(0, RgbColor(0, 0, 0));
      strip.Show();
    }
#endif
  }
}

void setup()
{
#ifdef LED_RGB
  Serial.begin(115200);
  // initialize strip
  strip.Begin();
  strip.Show();
#else
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
#endif
  // set potentiometer pin as input
  pinMode(POT_PIN, INPUT);

  // Conexión a la red WiFi
  Serial.println("Conectando a WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.print(".");
  }
  // Mostrar la dirección IP asignada
  IPAddress localIP = WiFi.localIP();
  Serial.printf("\nConectado a WiFi. Dirección IP: %s\n", localIP.toString().c_str());

  Serial.println("Servicio NTP inicializado");
  // Inicializar el objeto NTPClient
  timeClient.begin();

  // create task to read potentiometer value every second
  if (xTaskCreate(readPotTask, "readPotTask", 2048, NULL, 1, &readPotTaskHandle) != pdPASS) {
    Serial.println("Failed to create read potentiometer task");
  }
  else {
    Serial.println("Read potentiometer task created");
  }
  // create task to update time every second
  if (xTaskCreate(updateTimeTask, "updateTimeTask", 2048, NULL, 1, &updateTimeTaskHandle) != pdPASS) {
    Serial.println("Failed to create update time task");
  }
  else {
    Serial.println("Update time task created");
  }

  serverIP    = localIP;
  serverIP[3] = 1;
  Serial.printf("Server ip: %s\n", serverIP.toString().c_str());
  // Configurar el cliente seguro
  client.setCACert(ca_Cert);
  client.setInsecure();
  // Inicializar el cliente MQTT
  mqtt.setServer(MQTTSERVER, MQTTPORT);
  mqtt.setCallback(mqttCallback); // Configurar el callback para manejar mensajes MQTT
  // Conectar al servidor MQTT
  connectToMQTT();

  // Create task to send data to server
  if (xTaskCreate(sendDataTask, "sendDataTask", 8000, NULL, 1, &sendDataTaskHandle) != pdPASS) {
    Serial.println("Failed to create send data task");
  }
  else {
    Serial.println("Send data task created");
  }
}

void loop()
{
  // Procesar mensajes MQTT entrantes
  mqtt.loop();

  // read terminal input to switch on/off the LED
  if (Serial.available() > 0) {
    char command = Serial.read();
    if (command == '1') {
      ledOn = true;
#ifdef LED_RGB
      strip.SetPixelColor(0, actualColor);
      strip.Show();
#endif
    }
    else if (command == '0') {
      ledOn = false;
#ifdef LED_RGB
      strip.SetPixelColor(0, RgbColor(0, 0, 0));
      strip.Show();
#endif
    }
  }
}