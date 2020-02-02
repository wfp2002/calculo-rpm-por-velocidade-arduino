#define GPS_RX 3
#define GPS_TX 2
#define GPS_Serial_Baud 4800
#include <SoftwareSerial.h>
#include "TinyGPS++.h"
//Libs referente ao display
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

Adafruit_PCD8544 display = Adafruit_PCD8544(11,10,9,8,7);
// Software SPI (slower updates, more flexible pin options):
// pin 7 - Serial clock out (SCLK)
// pin 6 - Serial data out (DIN)
// pin 5 - Data/Command select (D/C)
// pin 4 - LCD chip select (CS)
// pin 3 - LCD reset (RST)


SoftwareSerial gpsSerial(GPS_RX, GPS_TX);
TinyGPSPlus gps;
 
float gpsLat0;
float gpsLong0;
float gpsLatUltimoLido = 0.00;
float gpsLongUltimoLido = 0.00;
float somaDistancia = 0.00;

const double larguraPneu = 165.00;
const double perfilPneu = 70.00; //Perfil original 75, tirando 5 para compensar o amassado no asfalta (Diametro Dinamico)
const double aroPneu = 14.00;
const double relacaoMarchaPrimeira = 3.846; 
const double relacaoMarchaSegunda = 2.038;
const double relacaoMarchaTerceira = 1.281;
const double relacaoMarchaQuarta = 0.951;
const double relacaoMarchaQuinta = 0.756;
const double relacaoMarchaRe = 3.615;
const double relacaoMarchaDiferencial = 4.73;



/* tabela de cursos de direcao 
Este - Nordeste   ENE
Este - Sudeste    ESE
Leste             E
Nordeste          NE
Noroeste          NW
Norte - Nordeste  NNE
Norte - Noroeste  NNW
Oeste             W
Oeste - Noroeste  WNW
Oeste - Sudoeste  WSW
Sudeste           SE
Sudoeste          SW
Sul               S
Sul - Sudeste     SSE
Sul - Sudoeste    SSW
*/

void setup()
{

  Serial.begin(115200);
  gpsSerial.begin(GPS_Serial_Baud);
  display.begin();
  display.setContrast(50);
  display.display(); // show splashscreen
  delay(2000);
  display.clearDisplay();   // clears the screen and buffer
}
 
void loop() {

  //int POT = analogRead(0);                          // Potenciometro - pino central no pino A0
  //double velocidade = (map(POT, 0, 1023, 200, 1));

  bool newData = false;
  unsigned long chars;
  // For one second we parse GPS data and report some key values
  for (unsigned long start = millis(); millis() - start < 200;)
  {
    while (gpsSerial.available())
    {
      char c = gpsSerial.read();
      // Serial.write(c); //apague o comentario para mostrar os dados crus
      if (gps.encode(c)) // Atribui true para newData caso novos dados sejam recebidos
        newData = true;
    }
  }
  
  
  if (newData)
  {

    //colegio avante 970m carro, ape 600m
    const double AVANTE_LAT = -22.692943;
    const double AVANTE_LNG = -46.988746;

    //Gerando o ponto inicial fixo do momento que pegou sinal do GPS.
    if (gpsLat0 == 0.0)
    {
      gpsLat0 = gps.location.lat();
      gpsLong0 = gps.location.lng();
    }

    if (gpsLatUltimoLido == 0.0)
    {
      somaDistancia = 0.00;
      gpsLatUltimoLido = gps.location.lat();
      gpsLongUltimoLido = gps.location.lng();      
    } else {
      somaDistancia = somaDistancia + (gps.distanceBetween(gps.location.lat(),gps.location.lng(),gpsLatUltimoLido,gpsLongUltimoLido) / 1000.0);
      gpsLatUltimoLido = gps.location.lat();
      gpsLongUltimoLido = gps.location.lng();
    }
    
    double distanceKmAvante = gps.distanceBetween(gps.location.lat(),gps.location.lng(),AVANTE_LAT,AVANTE_LNG) / 1000.0;
    double courseToAvante = gps.courseTo(gps.location.lat(),gps.location.lng(),AVANTE_LAT,AVANTE_LNG);
    double distanceKmPercorrida = gps.distanceBetween(gps.location.lat(),gps.location.lng(),gpsLat0,gpsLong0) / 1000.0;

    double velocidade = gps.speed.kmph();
    //double velocidade = random(170);

/*
    Serial.println("########################################################################");
    Serial.print("Velocidade: " + String(gps.speed.kmph()) + " RPM Primeira: ");
    Serial.println((velocidade * 1000 * relacaoMarchaPrimeira * relacaoMarchaDiferencial) / (60 * 3.1416 * (larguraPneu * perfilPneu * 0.00002 + aroPneu * 0.0254)),0);

    Serial.print("Velocidade: " + String(gps.speed.kmph()) + " RPM Segunda: ");
    Serial.println((velocidade * 1000 * relacaoMarchaSegunda * relacaoMarchaDiferencial) / (60 * 3.1416 * (larguraPneu * perfilPneu * 0.00002 + aroPneu * 0.0254)),0);

    Serial.print("Velocidade: " + String(gps.speed.kmph()) + " RPM Terceira: ");
    Serial.println((velocidade * 1000 * relacaoMarchaTerceira * relacaoMarchaDiferencial) / (60 * 3.1416 * (larguraPneu * perfilPneu * 0.00002 + aroPneu * 0.0254)),0);

    Serial.print("Velocidade: " + String(gps.speed.kmph()) + " RPM Quarta: ");
    Serial.println((velocidade * 1000 * relacaoMarchaQuarta * relacaoMarchaDiferencial) / (60 * 3.1416 * (larguraPneu * perfilPneu * 0.00002 + aroPneu * 0.0254)),0);

    Serial.print("Velocidade: " + String(gps.speed.kmph()) + " RPM Quinta: ");
    Serial.println((velocidade * 1000 * relacaoMarchaQuinta * relacaoMarchaDiferencial) / (60 * 3.1416 * (larguraPneu * perfilPneu * 0.00002 + aroPneu * 0.0254)),0);
*/

    display.clearDisplay();
    display.setTextSize(3);
    display.setTextColor(BLACK);
    display.setCursor(0,0);
    display.println(" " + String(velocidade,0));
    display.println((velocidade * 1000 * relacaoMarchaTerceira * relacaoMarchaDiferencial) / (60 * 3.1416 * (larguraPneu * perfilPneu * 0.00002 + aroPneu * 0.0254)),0);
    display.display();
    
    //display.setTextSize(2);
    //display.setTextColor(BLACK);
    //display.println(velocidade,0);

    //display.setTextSize(1);
    //display.setTextColor(BLACK);
    //display.println("RPM");
    //display.setTextSize(2);
    //display.setTextColor(BLACK);
    //display.println((velocidade * 1000 * relacaoMarchaQuinta * relacaoMarchaDiferencial) / (60 * 3.1416 * (larguraPneu * perfilPneu * 0.00002 + aroPneu * 0.0254)),0);
    

  }
}
