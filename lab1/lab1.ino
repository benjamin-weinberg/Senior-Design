/*************************************************** 
  This is an example for the Adafruit Thermocouple Sensor w/MAX31855K

  Designed specifically to work with the Adafruit Thermocouple Sensor
  ----> https://www.adafruit.com/products/269

  These displays use SPI to communicate, 3 pins are required to  
  interface
  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/

#include <SPI.h>
#include "Adafruit_MAX31855.h"

// Default connection is using software SPI, but comment and uncomment one of
// the two examples below to switch between software SPI and hardware SPI:

// Example creating a thermocouple instance with software SPI on any three
// digital IO pins.
#define MAXDO   11
#define MAXCS   12
#define MAXCLK  13

// initialize the Thermocouple
Adafruit_MAX31855 thermocouple(MAXCLK, MAXCS, MAXDO);

// Example creating a thermocouple instance with hardware SPI
// on a given CS pin.
//#define MAXCS   10
//Adafruit_MAX31855 thermocouple(MAXCS);

void setup() {
  Serial.begin(9600);

  pinMode(2, OUTPUT); // least significant
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT); // most significant
  pinMode(9, OUTPUT); // software button

  digitalWrite(9,LOW);
 
  while (!Serial) delay(1); // wait for Serial
  delay(500);
}

void loop() {
   double c = thermocouple.readCelsius();
   Serial.println(c);
   bool button = digitalRead(10);
   while(isnan(c)){     
     digitalWrite(9,1);
     for(int i = 0;i<7;i++){
       digitalWrite(i+2,((i+1)%2));
     }
     delay(500);    
     for(int i = 0;i<7;i++){
       digitalWrite(i+2,(i%2));
     }
     delay(500);
     c = thermocouple.readCelsius();
     Serial.println(c);
     digitalWrite(9,0);
   }

   if (Serial.available() > 0) {
      // read the incoming byte:
      bool output = LOW;
      int incoming = Serial.read();
      if(incoming == 49) output = HIGH;
      digitalWrite(9,output);
   }
   for(int i = 0;i<7;i++){
     digitalWrite(i+2,!bitRead((int)c,i));
   }
 delay(1000);
}
