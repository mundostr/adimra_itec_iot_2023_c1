// Inclusión librerías
#include <DHT.h>
#include <Wire.h>
#include <WiFi.h>
#include <PicoMQTT.h>
#include <Adafruit_BMP280.h>

// Definición de pines
#define PIN_RELAY 13
#define PIN_SENSOR_HUMEDAD 16

// Definición de constantes
#define BAUDIOS_SERIE 115200
#define DIR_I2C_SENSOR_BMP 0x76
#define FREC_SENSOR_BMP 3000
#define WIFI_SSID "ITEC PUBLICA"
#define WIFI_CLAVE ""
#define MQTT_BROKER "f79f3acc160242ddb508cb6bc61b51e2.s1.eu.hivemq.cloud"
#define MQTT_PUERTO 8883
#define MQTT_USUARIO "adimraiot"
#define MQTT_CLAVE "Adimra2023"
#define MQTT_IDCLIENTE "cliente23"
#define MQTT_TOPICO_ENVIO "cperren/cetral_meteo"
#define MQTT_TOPICO_RECEPCION "cperren/oficina_ideas/iluminacion"

// Definición de variables
char datos_mqtt[100];
bool iluminacion_activa = false;
unsigned long timerSensorBmp = 0;

// Definición de objetos
Adafruit_BMP280 sensor_bmp;
DHT sensor_dht(PIN_SENSOR_HUMEDAD, DHT11);
PicoMQTT::Client cliente_mqtt(MQTT_BROKER, MQTT_PUERTO, MQTT_IDCLIENTE, MQTT_USUARIO, MQTT_CLAVE);

void setup() {
  // Inicialización puerto serie
  Serial.begin(BAUDIOS_SERIE);
  Serial.println("SISTEMA INICIADO");

  // Seteo de pines
  pinMode(PIN_RELAY, OUTPUT);
  digitalWrite(PIN_RELAY, HIGH);
  delay(3000);
  digitalWrite(PIN_RELAY, LOW);
  Serial.println("Relay: OK!");

  // Activación sensor BMP280
  if (!sensor_bmp.begin(DIR_I2C_SENSOR_BMP)) {
    Serial.println("Sensor BMP280: ERROR");
    while (1);
  }
  Serial.println("Sensor BMP280: OK!");

  sensor_bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,
                         Adafruit_BMP280::SAMPLING_X2,
                         Adafruit_BMP280::SAMPLING_X16,
                         Adafruit_BMP280::FILTER_X16,
                         Adafruit_BMP280::STANDBY_MS_500);

  // Activación sensor DHT11
  sensor_dht.begin();
  Serial.println("Sensor DHT11: OK!");

  // Activación conexión WiFi
  Serial.printf("Conectando a %s: ", WIFI_SSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_CLAVE);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  Serial.println("OK!");

  // Activación conexión MQTT y suscripción a tópico de control
  cliente_mqtt.subscribe(MQTT_TOPICO_RECEPCION, [](const char *topic, const char *payload) {
    if (strcmp(payload, "1") == 0 && !iluminacion_activa) {
      iluminacion_activa = true;
      digitalWrite(PIN_RELAY, HIGH);
      Serial.println("Iluminacion activada");
    } else if (iluminacion_activa) {
      iluminacion_activa = false;
      digitalWrite(PIN_RELAY, LOW);
      Serial.println("Iluminacion desactivada");
    }
  });
  cliente_mqtt.begin();
  Serial.println("Broker MQTT: OK!");
}

void loop() {
  cliente_mqtt.loop();

  if (millis() - timerSensorBmp >= FREC_SENSOR_BMP) {
    float humedad = 100.0;
    snprintf(datos_mqtt, sizeof(datos_mqtt), "{ \"temperatura\": %.1f, \"presion\": %.1f, \"humedad\": %.1f, \"iluminacion\": %i }", sensor_bmp.readTemperature(), sensor_bmp.readPressure() * 0.01, humedad, iluminacion_activa);
    Serial.println(datos_mqtt);
    cliente_mqtt.publish(MQTT_TOPICO_ENVIO, datos_mqtt);
    timerSensorBmp = millis();
  }
}
