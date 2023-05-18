#include <WiFi.h>
#include <PubSubClient.h>

const char* SSID_WF = "riot2023";
const char* CLAVE_WF = "riot2023";

const char* NOMBRE_MQTT = "cliente_cperren";
const char* BROKER_MQTT = "broker.hivemq.com";
const char* USUARIO_MQTT = "";
const char* CLAVE_MQTT = "";
const char* TOPICO_MQTT = "cperren/sensor18";
const char* MENSAJE_MQTT = "Este es el mensaje";
const int PUERTO_MQTT = 1883;

const long BAUDIOS_SERIE = 115200;

WiFiClient cliente_esp;
PubSubClient cliente_mqtt(cliente_esp);

void setup() {
  Serial.begin(BAUDIOS_SERIE);

  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID_WF, CLAVE_WF);

  Serial.printf("Conectando a %s ", SSID_WF);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println(" OK!");
  Serial.println(WiFi.localIP());

  cliente_mqtt.setServer(BROKER_MQTT, PUERTO_MQTT);
  while (!cliente_mqtt.connected()) {
    if (cliente_mqtt.connect(NOMBRE_MQTT, USUARIO_MQTT, CLAVE_MQTT)) {
      Serial.println("Conectado al broker");
    } else {
      Serial.print("ERROR al conectar al broker: ");
      Serial.print(cliente_mqtt.state());
      Serial.println(" reintento en 5 segs...");
      delay(5000);
    }
  }
}

void loop() {
  cliente_mqtt.publish(TOPICO_MQTT, MENSAJE_MQTT);
  delay(10000);
}
