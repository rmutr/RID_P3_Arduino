#include <Arduino.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define PIN_RX_0             3 
#define PIN_TX_0             1 
#define PIN_RX_1            16 
#define PIN_TX_1            17 

char buff_str[200] = {0}; 
unsigned long t_old = 0; 
int tmr_cnt = 0; 

int status_process = 0; 
int status_process_mon = 0; 
int error = 0; 

//------------------------------------------------------ 
#define SCREEN_WIDTH       128 
#define SCREEN_HEIGHT       64 
#define OLED_RESET           4 
#define GLCD_LINE_1         20 

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define GLCD_LINE_0          5 
#define GLCD_LINE_1         20 
#define GLCD_LINE_2         35 
#define GLCD_LINE_3         50 





//------------------------------------------------------ 
void setup() { 
  Serial.begin (115200, SERIAL_8N1, PIN_RX_0, PIN_TX_0); 
  Serial1.begin(115200, SERIAL_8N1, PIN_RX_1, PIN_TX_1); 

  Serial.println("RID Project 2");

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed")); 
    for(;;); // Don't proceed, loop forever 
  } 

  display.setTextSize(1); 
  display.setTextColor(WHITE); 

  display.clearDisplay(); 
  display.setCursor(0, GLCD_LINE_0); 
  display.println(F("RID Project 2")); 
  display.display(); 

//------------------------------------------------------ 
  tmr_cnt = 0; 
  status_process = 0; 
  status_process_mon = 0; 
  error = 0; 

} 

void loop() { 
  sprintf(buff_str, "tmr_cnt = %2d ", tmr_cnt); 

  Serial.println(buff_str); 

  display.clearDisplay(); 
  display.setCursor(0, GLCD_LINE_0); 
  display.println(F("RID Project 2")); 
  display.setCursor(0, GLCD_LINE_2); 
  display.println(F(buff_str)); 
  display.display(); 

  while ((micros() - t_old) < 100000L); t_old = micros(); 
  tmr_cnt++; if (tmr_cnt >= 10) { tmr_cnt = 0; } 
} 
