#include <NTPClient.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>
#include <freertos/semphr.h>

#define POT_PIN 34
#define LED_PIN 4
#define ID 4636
#define SERVER_PORT 54471
#define THRESHOLD 2048
// wifi credentials
const char* ssid = "GL-MT300N-V2-0bb";
const char* password = "goodlife";
WiFiClient client;
IPAddress serverIP;

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

// Tarea para leer el valor del potenciómetro
void readPotTask(void* parameter) {
    while (true) {
        uint16_t potValue = analogRead(POT_PIN);
        sumPotValues += potValue;
        numReadings++;
        uint16_t averagePotValue = sumPotValues / numReadings;
        
        // Control del LED según el estado de ledOn
        if (ledOn) {
            digitalWrite(LED_PIN, HIGH);  // Encender el LED
        } else {
            digitalWrite(LED_PIN, LOW);   // Apagar el LED
        }

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
        doc["tiempo_muestra"] = timeClient.getEpochTime();
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

            client.print(request);

            Serial.println("Datos enviados al servidor: " + json);
            if (client.available()) {
                String response = client.readString().c_str();
                Serial.println("Respuesta del servidor: " + response);
            }
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
        doc["id"] = ID;
        doc["ledStatus"] = ledOn;
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
            String request = "GET /ledStatus?id=" + String(ID) + " HTTP/1.1\r\n";
            request += "Host: " + serverIP.toString() + "\r\n";
            request += "Content-Type: application/json\r\n";
            request += "\r\n";  // Fin de las cabeceras, línea en blanco para separar el cuerpo

            client.print(request);

            Serial.println("Datos del led solicitados al servidor");
            Serial.println("Datos del led actual: " + json);
            if (client.available()) {
              String response = client.readString().c_str();
              Serial.println("Respuesta del servidor: " + response);
              
              // Comparar la respuesta con "false" o "true"
              if (response.indexOf("false") != -1) {
                  ledOn = false;
              } else if (response.indexOf("true") != -1) {
                  ledOn = true;
              } else {
                  Serial.println("Respuesta desconocida.");
              }
          }
            xSemaphoreGive(xMutex);
        }
        vTaskDelay(5000);
    }
}

void setup()
{
    Serial.begin(9600);
    // set potentiometer pin as input
    pinMode(POT_PIN, INPUT);
    pinMode(LED_PIN, OUTPUT);

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
        } else if (command == '0') {
            ledOn = false;
            
        }
    }
}

