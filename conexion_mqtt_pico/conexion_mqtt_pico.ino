/*
 * https://github.com/mlesniew/PicoMQTT
*/

#include <WiFi.h>
#include <PicoMQTT.h>

const byte PIN_LED = 2;

const char* SSID_WF = "colgate";
const char* CLAVE_WF = "colgate2023";
const char* BROKER_MQTT = "broker.hivemq.com";
const char* TOPICO_MQTT = "IDEAS/OF1/sensor8";
const char* MENSAJE_MQTT = "Sensor enviando datos!";

const int FRECUENCIA_ENVIO = 10000;
const long BAUDIOS_SERIE = 115200;

unsigned long ultimaMarcaTiempo = 0;

PicoMQTT::Client mqtt(BROKER_MQTT);

void setup() {
  Serial.begin(BAUDIOS_SERIE);

  pinMode(PIN_LED, OUTPUT);

  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID_WF, CLAVE_WF);

  Serial.printf("Conectando a %s ", SSID_WF);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println(" OK!");
  Serial.println(WiFi.localIP());
  digitalWrite(PIN_LED, HIGH);
  
  mqtt.begin();
}

void loop() {
  mqtt.loop();

  if (millis() - ultimaMarcaTiempo >= FRECUENCIA_ENVIO) {
    mqtt.publish(TOPICO_MQTT, MENSAJE_MQTT);
    Serial.println("mensaje enviado");
    ultimaMarcaTiempo = millis();
  }
}
