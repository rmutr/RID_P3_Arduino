#include <Arduino.h>

char str_buf[200] = {0}; 
unsigned int cnt; 

//unsigned int pin = 36; 
//unsigned int pin = 39; 
//unsigned int pin = 34; 
//unsigned int pin = 35; 
//unsigned int pin = 32; 
//unsigned int pin = 33; 
//unsigned int pin = 25; 
//unsigned int pin = 26; 
//unsigned int pin = 27; 
//unsigned int pin = 14; 
//unsigned int pin = 12; 
//unsigned int pin = 13; 
//unsigned int pin =  9; // Bug 
  unsigned int pin = 10; 
//unsigned int pin = 11; // Bug 

//unsigned int pin = 23; 
//unsigned int pin = 22; 
//unsigned int pin =  1; // Rx 
//unsigned int pin =  3; // Tx 
//unsigned int pin = 21; 
//unsigned int pin = 19; 
//unsigned int pin = 18; 
//unsigned int pin =  5; 
//unsigned int pin = 17; 
//unsigned int pin = 16; 
//unsigned int pin =  4; 
//unsigned int pin =  0; 
//unsigned int pin =  2; 
//unsigned int pin = 15; 
//unsigned int pin =  8; // Bug 
//unsigned int pin =  7; // Bug 
//unsigned int pin =  6; // Bug 

void setup() { 
  Serial.begin(115200); 

  pinMode(pin, INPUT);
} 

void loop() { 
  Serial.print(digitalRead(pin));
  sprintf(str_buf, " %030d ", cnt);
  Serial.println(str_buf); 
  cnt++; if (cnt >= 10) { cnt = 0; } 
  delay(1000); 
} 

