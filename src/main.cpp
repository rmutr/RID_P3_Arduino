//----------------------------------------------------------------------------- 
// Project          : RMUTR RID P3 Water Current Meter 
// VSCode Extension : PlatformIO IDE 1.10.0 
// Source           : https://github.com/rmutr/RID_P3_Water_Current_Meter.git 
// Board            : Node32s (Gravitech Node32Lite LamLoei) 
// Additional URLs  : https://dl.espressif.com/dl/package_esp32_index.json 
// LED_BUILTIN      : Pin 2 


//------------------------------------------------------ 
#include <Arduino.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <LiquidCrystal_PCF8574.h>

#define PIN_RX_0             3 
#define PIN_TX_0             1 
#define PIN_RX_1            16 
#define PIN_TX_1            17 

#define PIN_PROCESS          2 
#define PIN_EXP_INT         14 
#define PIN_BTN_START       13 
#define PIN_PULSEIN         12 

#define P_BUFF_MAX         100 

#define PNP_OFF              0 
#define PNP_ON               1 
#define NPN_OFF              1 
#define NPN_ON               0 

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
#define CLCD_ADD_27       0x27 
#define CLCD_ADD_3F       0x3F 

LiquidCrystal_PCF8574 clcd(CLCD_ADD_27); 


//------------------------------------------------------ 
void setup() { 
  pinMode(PIN_PROCESS, OUTPUT); 

  digitalWrite(PIN_PROCESS, NPN_OFF); 

  Serial.begin (115200, SERIAL_8N1, PIN_RX_0, PIN_TX_0); 
  Serial1.begin(115200, SERIAL_8N1, PIN_RX_1, PIN_TX_1); 
  delay(50); 
  Serial.println("RID Project 2");

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); 
  display.setTextSize(1); 
  display.setTextColor(WHITE); 

  display.clearDisplay(); 
  display.setCursor(0, GLCD_LINE_0); 
  display.println(F("RID Project 2")); 
  display.display(); 

  clcd.begin(16, 2); 
  clcd.init(); 
  clcd.setBacklight(255); 
  clcd.setCursor(0, 0); 
  clcd.print("RID Project 2   "); 
  clcd.setCursor(0, 1); 
  clcd.print("tmr_cnt =  0    "); 

//------------------------------------------------------ 
  tmr_cnt = 0; 
  status_process = 0; 
  status_process_mon = 0; 
  error = 0; 

} 

void loop() { 

  if (tmr_cnt < 5) { 
    digitalWrite(PIN_PROCESS, NPN_OFF); 
    sprintf(buff_str, "t-cnt = %2d      ", tmr_cnt); 
  } else { 
    digitalWrite(PIN_PROCESS, NPN_ON); 
    sprintf(buff_str, "t-cnt = %2d [ON] ", tmr_cnt); 
  } 

  Serial.println(buff_str); 

  display.clearDisplay(); 
  display.setCursor(0, GLCD_LINE_0); 
  display.println(F("RID Project 2")); 
  display.setCursor(0, GLCD_LINE_2); 
  display.println(F(buff_str)); 
  display.display(); 

  clcd.setCursor(0, 1); 
  clcd.print(buff_str); 

  while ((micros() - t_old) < 100000L); t_old = micros(); 
  tmr_cnt++; if (tmr_cnt >= 10) { tmr_cnt = 0; } 
} 
