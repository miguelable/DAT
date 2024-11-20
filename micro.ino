// Autor: Miguel Ferrer y Paula Fernandez
#define LED_RGB

#ifdef LED_RGB
#include <Arduino.h>
#include <NeoPixelBus.h>
#endif
#include <NTPClient.h>
#include <WiFiClientSecure.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>
#include <freertos/semphr.h>

#ifdef LED_RGB
#define PIXEL_COUNT 1
#endif
#define POT_PIN 34
#define LED_PIN 4
#define ID 34
#define SERVER_PORT 54472
#ifndef LED_RGB
#define THRESHOLD 2048
#endif
// wifi credentials
const char* ssid = "GL-MT300N-V2-0bb";
const char* password = "goodlife";
WiFiClientSecure client;
IPAddress serverIP;

// Certificado del servidor (certificado público)
const char* server_cert = R"EOF(
-----BEGIN CERTIFICATE-----
MIID9zCCAt+gAwIBAgIUZKPYCVsl3aqE4EZDs/1agNCfFWcwDQYJKoZIhvcNAQEL
BQAwgYoxCzAJBgNVBAYTAkFVMQswCQYDVQQIDAJFUzERMA8GA1UEBwwIUGFtcGxv
bmExEDAOBgNVBAoMB0dydXBvMDMxDDAKBgNVBAsMA0RBVDEPMA0GA1UEAwwGTWln
dWVsMSowKAYJKoZIhvcNAQkBFhtmZXJyZXIuMTIxMjQyQGUudW5hdmFycmEuZXMw
HhcNMjQxMTIwMTczNDI0WhcNMjUxMTIwMTczNDI0WjCBijELMAkGA1UEBhMCQVUx
CzAJBgNVBAgMAkVTMREwDwYDVQQHDAhQYW1wbG9uYTEQMA4GA1UECgwHR3J1cG8w
MzEMMAoGA1UECwwDREFUMQ8wDQYDVQQDDAZNaWd1ZWwxKjAoBgkqhkiG9w0BCQEW
G2ZlcnJlci4xMjEyNDJAZS51bmF2YXJyYS5lczCCASIwDQYJKoZIhvcNAQEBBQAD
ggEPADCCAQoCggEBALyFjQ5SjSVnuOkT7ATSalnqldf7dsFmzPmA4raNJYCnrV5V
kw1pePUrWYUrJIaBeyiL9bl94NOT+NdzDujj/lHiEINiqEApesLYzQ9nDFDS36FC
yJ4kSvg3vDf4qy11J+FNrkZQQM40RePNpEGis3jDfXmJNHplmfiaT+dVC04GRlYh
8ejt+L0Ms2Po6SeePG4qofCr6PtRYeYOtQAFKSMut6ihE4oH6FDSWb4pqhaL3/Gi
8x7I6+2NxCEdgifbJrPcHFXtL4oLk6NJUbxw9Z/+BhaIC760IxjFlF5WfwggwGs3
2IzJxu8DhGkW1Ob4zcj0yVbXDmDfw0KiSl6j7lkCAwEAAaNTMFEwHQYDVR0OBBYE
FJf3xjP6AHuRnNzMmlHL6ZlD6uUQMB8GA1UdIwQYMBaAFJf3xjP6AHuRnNzMmlHL
6ZlD6uUQMA8GA1UdEwEB/wQFMAMBAf8wDQYJKoZIhvcNAQELBQADggEBAGt9T6Ck
XVrrfbo4oMfIqV6KcVvLzowrdGQxft4wKm/MOxC1Jp1griqg8SnMmFupePOxgY0R
Y6MSqGpBrH3Sz435WI+3+mPZunvKJYpqz07gNZA1xh7TgiPn7M14fP0HXPHwUpTb
f/CGQHAPY6MrQ++56Ej4ciHyAP9klPfB16XX7wYmtLpIC8ds+C4msl3TgPRl0SvT
ea75O+JV+uaM5xdA8D7QFaTGsGj8e1Ps+XqclVEsmUT2RYP5cpCka4xVV15zyTYJ
DfsG1AMLXq0xX6D66OzZyjpJygOApCpThPkYz3QEbBF4aJthmZ5vQr67L/A7fPYP
FnAxRrpGfjne7Ks=
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
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 3600;  // Ajuste de zona horaria (Ejemplo: GMT+1 Madrid)
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, ntpServer, gmtOffset_sec);

// Variables para calcular la media
uint32_t sumPotValues = 0;
uint32_t numReadings = 0;

// Task handles
TaskHandle_t readPotTaskHandle = NULL;
TaskHandle_t updateTimeTaskHandle = NULL;
TaskHandle_t sendDataTaskHandle = NULL;
TaskHandle_t askDataTaskHandle = NULL;

// Mutex para proteger las secciones críticas
SemaphoreHandle_t xMutex;

#ifdef LED_RGB
// Función para obtener el color en función del valor del potenciómetro
void setColor(uint16_t potValue) {
    // Calcular los componentes RGB basados en una progresión lineal
    uint8_t red = map(potValue, 0, 4095, 0, 255);
    uint8_t green = map(potValue, 0, 4095, 255, 0);
    uint8_t blue = 0; // No hay componente azul en la progresión verde-rojo

    RgbColor desiredColor(red, green, blue);
    bool colorChanged = false;

    if (desiredColor != actualColor) {
        actualColor = desiredColor;
        colorChanged = true;
        strip.SetPixelColor(0, actualColor);
        strip.Show();
    }
}
#endif

// Tarea para leer el valor del potenciómetro
void readPotTask(void* parameter) {
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
            digitalWrite(LED_PIN, HIGH);  // Encender el LED
        } else {
            digitalWrite(LED_PIN, LOW);   // Apagar el LED
        }
#endif
        vTaskDelay(100);
    }
}

// Tarea para actualizar la hora
void updateTimeTask(void* parameter) {
    while (true) {
        timeClient.update();
        //Serial.printf("Hora actual: %s\n", timeClient.getFormattedTime().c_str());
        vTaskDelay(1000);
    }
}

// Tarea para enviar datos al servidor
void sendDataTask(void* parameter) {
    while (true) {
        // Enviar datos al servidor
        StaticJsonDocument<200> doc;
        doc["id_sonda"] = ID;
        doc["potencia"] = sumPotValues / numReadings;
        doc["timestamp"] = timeClient.getEpochTime();
        // Reiniciar la suma y el contador
        sumPotValues = 0;
        numReadings = 0;

        String json;
        serializeJson(doc, json);

        // Proteger la sección crítica con el mutex
        if (xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE) {
            // Comprobar si la conexión con el servidor está activa
            if (!client.connected()) {
                Serial.println("Cliente desconectado");
                if (!client.connect(serverIP, SERVER_PORT)) {
                    Serial.println("No se pudo conectar al servidor");
                    xSemaphoreGive(xMutex);
                    vTaskDelay(10000);
                    continue;
                } else {
                    Serial.println("Conectado al servidor");
                }
            }
            String request = "POST /potValue HTTP/1.1\r\n";
            request += "Host: " + serverIP.toString() + "\r\n";
            request += "Content-Type: application/json\r\n";
            request += "Content-Length: " + String(json.length()) + "\r\n";
            request += "\r\n";
            request += json;

            client.println(request);
            Serial.println("Datos enviados al servidor: " + json);

            xSemaphoreGive(xMutex);
        }
        vTaskDelay(10000);
    }
}

// Tarea para solicitar datos al servidor
void askDataTask(void* parameter) {
    while (true) {
        // Crear el JSON con el ID de la sonda
        StaticJsonDocument<200> doc;
        doc["id_sonda"] = ID;
        doc["ledStatus"] = ledOn ? "1" : "0";
        String json;
        serializeJson(doc, json);
        // Proteger la sección crítica con el mutex
        if (xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE) {
            // Comprobar si la conexión con el servidor está activa
            if (!client.connected()) {
                Serial.println("Cliente desconectado");
                if (!client.connect(serverIP, SERVER_PORT)) {
                    Serial.println("No se pudo conectar al servidor");
                    xSemaphoreGive(xMutex);
                    vTaskDelay(5000);
                    continue;
                } else {
                    Serial.println("Conectado al servidor");
                }
            }
            // Solicitar datos al servidor
            String request = "GET /ledStatus HTTP/1.1\r\n";
            request += "Host: " + serverIP.toString() + "\r\n";
            request += "Content-Type: application/json\r\n";
            request += "Content-Length: " + String(json.length()) + "\r\n";
            request += "\r\n";
            request += json;
            client.println(request);

            Serial.println("Status enviado al servidor" + json);
            xSemaphoreGive(xMutex);
        }
        vTaskDelay(5000);
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

    // Crear el mutex
    xMutex = xSemaphoreCreateMutex();
    if (xMutex == NULL) {
        Serial.println("Failed to create mutex");
        return;
    }

    // create task to read potentiometer value every second
    if (xTaskCreate(readPotTask, "readPotTask", 2048, NULL, 1, &readPotTaskHandle) != pdPASS) {
        Serial.println("Failed to create read potentiometer task");
    } else {
        Serial.println("Read potentiometer task created");
    }
    // create task to update time every second
    if (xTaskCreate(updateTimeTask, "updateTimeTask", 2048, NULL, 1, &updateTimeTaskHandle) != pdPASS) {
        Serial.println("Failed to create update time task");
    } else {
        Serial.println("Update time task created");
    }

    serverIP = localIP;
    serverIP[3] = 1;
    Serial.printf("Server ip: %s\n", serverIP.toString().c_str());
    // Configurar el cliente seguro
    client.setCACert(server_cert);
    client.setInsecure();
    // Inicializar el servidor
    if (!client.connect(serverIP, SERVER_PORT)) {
        Serial.println("Failed to connect to server");
    } else {
        Serial.println("Connected to server");
    }

    // Create task to send data to server
    if (xTaskCreate(sendDataTask, "sendDataTask", 2048, NULL, 1, &sendDataTaskHandle) != pdPASS) {
        Serial.println("Failed to create send data task");
    } else {
        Serial.println("Send data task created");
    }

    // Create task to ask for data to server
    if (xTaskCreate(askDataTask, "askDataTask", 2048, NULL, 1, &askDataTaskHandle) != pdPASS) {
        Serial.println("Failed to create ask data task");
    } else {
        Serial.println("Ask data task created");
    }
}

void loop()
{   
    // read terminal input to switch on/off the LED
    if (Serial.available() > 0) {
        char command = Serial.read();
        if (command == '1') {
            ledOn = true;
#ifdef LED_RGB
            strip.SetPixelColor(0, actualColor);
            strip.Show();
#endif
        } else if (command == '0') {
            ledOn = false;
#ifdef LED_RGB
            strip.SetPixelColor(0, RgbColor(0, 0, 0));
            strip.Show();
#endif
        }
    }
    if (client.available()) {
        String response = client.readString().c_str();
        Serial.println("Respuesta del servidor: \n" + response);
        StaticJsonDocument<200> doc;
        deserializeJson(doc, response);
        bool desired_status = doc["desired_status"] == "1" ? true : false;
        if (desired_status != ledOn) {
            ledOn = desired_status;
            Serial.printf("Cambiando estado del LED a: %s\n", ledOn ? "ON" : "OFF");
#ifdef LED_RGB
            if (ledOn) {
                strip.SetPixelColor(0, actualColor);
                strip.Show();
            } else {
                strip.SetPixelColor(0, RgbColor(0, 0, 0));
                strip.Show();
            }
#endif
        }
    }
}
