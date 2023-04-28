//Librerías
#include "DHT.h"               
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include <ESP32Time.h>
#include <WiFi.h>

//Pantalla Display en pixeles
#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64 

//Sensor
#define DHTPIN 23 
#define DHTTYPE DHT11 

//Pines
#define PIN_BOTON_SUBIR 35 
#define PIN_BOTON_BAJAR 34
bool estadoBoton1 = false;
bool estadoBoton2 = false;

//Máquina de Estados
#define PANTALLA_INICIAL 1    
#define LIMPIAR_1 2
#define PANTALLA_CAMBIOS 3
#define LIMPIAR_2 4

//Estado inicial de la máquina
int estado = 1;  

// Inicializar el Display y el Sensor
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
DHT dht(DHTPIN, DHTTYPE);

//Inicializar rtc
int gmt;
struct tm timeinfo;
ESP32Time rtc;


//Reloj
long unsigned int timestamp; 
const char *ntpServer = "south-america.pool.ntp.org";
long gmtOffset_sec = -10800;
const int daylightOffset_sec = 0;

//WiFi
const char* ssid = "ORT-IoT";           
const char* password = "OrtIOTnew22$2";

//Funciones
void pedir_lahora(void);
void setup_rtc_ntp(void);

void setup() 
{
  
  Serial.begin(9600);
  pinMode(PIN_BOTON_SUBIR, INPUT_PULLUP);    
  pinMode(PIN_BOTON_BAJAR, INPUT_PULLUP);  
  
  Serial.println("Connecting to Wi-Fi...");

  //Llamo a las funciones
  initWiFi(); 
  setup_rtc_ntp();

  //Oled funcionamiento
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) 
  { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  
  //Parametros OLED
  delay(2000);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);


}

void loop() 
{
  
  switch(estado)
  {
    //Pantalla que muestra hora y temperatura
    case PANTALLA_INICIAL: 
   
{      
      Serial.println("PANTALLA INICIAL"); 
      delay(2000);

      float h = dht.readHumidity();
      float t = dht.readTemperature();
      display.clearDisplay();
      display.setCursor(0, 10);
      display.print("Humedad: ");
      display.print(h);
      display.print(" %");
      display.setCursor(0, 20);
      display.print("Temperatura: ");
      display.print(t);

      pedir_lahora();
        
      if(digitalRead(PIN_BOTON_BAJAR) == LOW && digitalRead(PIN_BOTON_SUBIR) == LOW)
      {
        estado = LIMPIAR_1;
      }
      
}
    break;

    //Espera a que se suelten los botones para pasar a la siguiente pantalla
    case LIMPIAR_1:  
      Serial.println("PRIMER ESPERA"); 
      if(digitalRead(PIN_BOTON_BAJAR) == HIGH && digitalRead(PIN_BOTON_SUBIR) == HIGH)
      {
        estado = PANTALLA_CAMBIOS;
      }
    break;

    //Pantalla que permite cambiar la hora
    case PANTALLA_CAMBIOS: 

      Serial.println("PANTALLA CAMBIOS");
      display.clearDisplay();
      display.setCursor(0, 10);
      display.print(gmtOffset_sec);
      display.display();    
      
      if(digitalRead(PIN_BOTON_BAJAR) == LOW) 
      {
        estadoBoton1 = true;
      }
      if(digitalRead(PIN_BOTON_BAJAR) == HIGH && estadoBoton1 == true) 
      {
        estadoBoton1 = false;
        gmtOffset_sec = gmtOffset_sec - 3600;

      }    

      if(digitalRead(PIN_BOTON_SUBIR) == LOW)
      {
        estadoBoton2 = true;
      }
      if(digitalRead(PIN_BOTON_SUBIR) == HIGH && estadoBoton2 == true;)
      {
        estadoBoton2 = false;
        gmtOffset_sec = gmtOffset_sec + 3600;
      }    
      
      if(digitalRead(PIN_BOTON_BAJAR) == LOW && digitalRead(PIN_BOTON_SUBIR) == LOW)  //cambio al estado de espera
      {
        estado = LIMPIAR_2;
      }
    break;

    //Espera a que se suelten los botones para volver a la pantalla inicial
    case LIMPIAR_2: 
    
      Serial.println("SEGUNDA ESPERA");
      if(digitalRead(PIN_BOTON_BAJAR) == HIGH && digitalRead(PIN_BOTON_SUBIR) == HIGH) 
      {
        estado = PANTALLA_INICIAL;
        setup_rtc_ntp();      
      }
    
    break;   
  }

}

//Función conexión WiFi
void initWiFi() 
{                                         
  WiFi.begin(ssid , password );
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
  Serial.println();
}

//Funciones Tiempo
void setup_rtc_ntp(void)
{
  
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  timestamp = time(NULL);
  rtc.setTime(timestamp + gmtOffset_sec);
}


void pedir_lahora(void)
{
  if (!getLocalTime(&timeinfo))
  {
    Serial.println("Veo la hora del rtc interno ");
    timestamp = rtc.getEpoch() - gmtOffset_sec;
    timeinfo = rtc.getTimeStruct();
    display.clearDisplay();
    display.print(&timeinfo, "%A, %B %d %Y %H:%M:%S");
    display.display();
  }
  else
  {
    Serial.print("NTP Time:");
    timestamp = time(NULL);
    display.clearDisplay();    
    display.print(&timeinfo, "%A, %B %d %Y %H:%M:%S");
    display.display();
  }

  return;
}
