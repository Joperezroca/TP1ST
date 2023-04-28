//Librerías
#include "DHT.h"               
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include <ESP32Time.h>

//Pantalla Display en pixeles
#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64 

//Sensor
#define DHTPIN 23 
#define DHTTYPE DHT11 

//Pines
#define PIN_BOTON_MINUTO 35 
#define PIN_BOTON_HORA 34
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
ESP32Time rtc;

int mins = 0;
int hours = 24;

void setup() 
{
  
  Serial.begin(9600);
  pinMode(PIN_BOTON_MINUTO, INPUT_PULLUP);    
  pinMode(PIN_BOTON_HORA, INPUT_PULLUP);  
  rtc.setTime(mins, hours, 28, 4, 2023);

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

      display.print(rtc.getHour(true)); 
      display.print(":");                
      display.print(rtc.getMinute());

      Serial.println(rtc.getHour(true));
      Serial.println(":");                 
      Serial.println(rtc.getMinute()); 

      display.display();
        
      if(digitalRead(PIN_BOTON_HORA) == LOW && digitalRead(PIN_BOTON_MINUTO) == LOW)
      {
        estado = LIMPIAR_1;
      }
      
}
    break;

    //Espera a que se suelten los botones para pasar a la siguiente pantalla
    case LIMPIAR_1:  
      Serial.println("PRIMER ESPERA"); 
      if(digitalRead(PIN_BOTON_HORA) == HIGH && digitalRead(PIN_BOTON_MINUTO) == HIGH)
      {
        estado = PANTALLA_CAMBIOS;
      }
    break;

    //Pantalla que permite cambiar la hora
    case PANTALLA_CAMBIOS: 

      Serial.println("PANTALLA CAMBIOS");
      display.clearDisplay();
      display.setCursor(0, 10);
      display.print(hours);
      display.print(":");
      display.print(mins);
      display.display();     
      
      if(digitalRead(PIN_BOTON_HORA) == LOW) 
      {
        estadoBoton1 = true;
      }
      if(digitalRead(PIN_BOTON_HORA) == HIGH && estadoBoton1 == true) 
      {
        estadoBoton1 = false;
        hours = hours + 1;

      }    

      if(digitalRead(PIN_BOTON_MINUTO == LOW)
      {
        estadoBoton2 = true;
      }
      if(digitalRead(PIN_BOTON_MINUTO) == HIGH && estadoBoton2 == true;)
      {
        estadoBoton2 = false;
        mins = mins + 1;
      }    
      
      if(digitalRead(PIN_BOTON_HORA) == LOW && digitalRead(PIN_BOTON_MINUTO) == LOW)  //cambio al estado de espera
      {
        estado = LIMPIAR_2;
      }
    break;

    //Espera a que se suelten los botones para volver a la pantalla inicial
    case LIMPIAR_2: 
    
      Serial.println("SEGUNDA ESPERA");
      if(digitalRead(PIN_BOTON_HORA) == HIGH && digitalRead(PIN_BOTON_MINUTO) == HIGH) 
      {
        estado = PANTALLA_INICIAL;
        rtc.setTime(segs, mins, hours, 11, 4, 2023);      
      }
    
    break;   
  }

}