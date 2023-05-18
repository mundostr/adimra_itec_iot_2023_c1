#include <WiFi.h>
#include <time.h>

const byte PIN_LED = 2;

const char* SSID_WF = "colgate";
const char* CLAVE_WF = "colgate2023";
const char* SERVIDOR_NTP = "pool.ntp.org";
const int OFFSET_GMT = 3600 * -3;
const long BAUDIOS_SERIE = 115200;

void mostrarHoraLocal() {
  configTime(OFFSET_GMT, 0, SERVIDOR_NTP);
  
  struct tm infoTiempo; 
  if (getLocalTime(&infoTiempo)) {
    Serial.println(&infoTiempo, "Hora local-> %H:%M:%S");
  }
}

void setup() {
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);
  
  Serial.begin(BAUDIOS_SERIE);
  WiFi.begin(SSID_WF, CLAVE_WF);

  Serial.printf("Conectando a %s ", SSID_WF);
  while(WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  digitalWrite(PIN_LED, HIGH);
  Serial.println(" OK!");
  Serial.println(WiFi.localIP());
}

void loop() {
  mostrarHoraLocal();
  delay(10000);
}
