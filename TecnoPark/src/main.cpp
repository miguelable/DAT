#include <Arduino.h>

#define LED_PIN 4

void setup() {
  Serial.begin(115200); // UART1 para logs
  Serial.println("Iniciando logs en UART1");
  pinMode(LED_PIN, OUTPUT);
  Serial.println("Iniciando LED");
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(LED_PIN, HIGH);
  delay(1000);
  digitalWrite(LED_PIN, LOW);
  delay(1000);
}
