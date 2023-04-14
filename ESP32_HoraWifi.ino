//Cógido de conexión wifi y hora del ESp32

#include <ESP32Time.h>
#include <Wifi.h>

ESP32Time rtc;

//Wifi
const char* ssid = "";
const char* password = ""; 

//Configuracion Servidor NTP
const char* ntpServer = "pool.ntp.ort";
const int gmt = 0
const long gmtOffset_sec = gmt*3600;
const int daylightOffset_sec = 0;

void setup() {
  
  Serial.begin(115200);
  Serial.printf("Intentando Conectarnos a %s%", ssid);
  WiFi.begin(ssid,password);
  while (WiFi.status()!=WL_CONNECTED){
    delay (500);
    Serial.print(".");
  }
  Serial.pintln("CONECTADO");
  configTime(gmtOffset_sec,daylightOffset_sec,ntpServer);
  struct tm timeinfo;
  if(getLocalTime(&timeinfo)){
    rtc.setTimeStruct(timeinfo);
  }
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);

}

void loop() {
  
  Serial.println(rtc.getTime("%d/%m/%Y %H:%M:%S"));
  delay(1000);

}
