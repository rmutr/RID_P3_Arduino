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
#define PIN_RX_1            18 
#define PIN_TX_1            19 

#define I2C_SCL             22 
#define I2C_SDA             21 

#define PIN_PROCESS          2 
#define PIN_BTN_START       16 
#define PIN_PULSEIN         17 

#define PIN_SEL_3           36 
#define PIN_SEL_2           39 
#define PIN_SEL_1           34 
#define PIN_SEL_0           35 

#define P_BUFF_MAX         100 

#define PNP_OFF              0 
#define PNP_ON               1 
#define NPN_OFF              1 
#define NPN_ON               0 

char buff_str[200] = {0}; 
unsigned long t_old = 0; 
int tmr_cnt = 0; 
int error = 0; 

int state_ix = 0; 
int state_ix_mon = 0; 
volatile unsigned long t_on, t_off; 
volatile unsigned long p_count, p_store, p_buff_ix, p_total; 
volatile unsigned long p_buff[P_BUFF_MAX] = {0}; 
volatile int p_buff_use = 0; 
volatile int p_buff_old = 0; 
volatile int req_reset = 0; 

int selector_pin[4] = {0}; 
int selector_val = 0; 

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

#define CLCD_LINE_0          0 
#define CLCD_LINE_1          1 

LiquidCrystal_PCF8574 clcd(CLCD_ADD_27); 

//------------------------------------------------------ 
void Interrupt_Service_Btn_Start(); 
void Interrupt_Service_PulseIn(); 


void Interrupt_Service_Btn_Start() { req_reset = 1; } 

void Interrupt_Service_PulseIn() {
  if (digitalRead(PIN_PULSEIN) == HIGH) {
    t_on = micros();
  } else {
    t_off = micros();
    if ((t_off - t_on) > 200) { p_count++; }
  }
}


//------------------------------------------------------ 
void setup() { 
  pinMode(PIN_PROCESS,   OUTPUT); 
  pinMode(PIN_BTN_START, INPUT_PULLUP); 
  pinMode(PIN_PULSEIN,   INPUT_PULLUP); 

  digitalWrite(PIN_PROCESS,   HIGH); 
  digitalWrite(PIN_BTN_START, HIGH); 
  digitalWrite(PIN_PULSEIN,   HIGH); 

  selector_pin[3] = PIN_SEL_3; 
  selector_pin[2] = PIN_SEL_2; 
  selector_pin[1] = PIN_SEL_1; 
  selector_pin[0] = PIN_SEL_0; 

  for (int bix = 0; bix < 4; bix++) { 
    pinMode(selector_pin[bix], INPUT_PULLUP); 
    digitalWrite(selector_pin[bix], HIGH); 
  } 

  attachInterrupt(digitalPinToInterrupt(PIN_BTN_START) 
    , Interrupt_Service_Btn_Start, FALLING); 

  attachInterrupt(digitalPinToInterrupt(PIN_PULSEIN) 
    , Interrupt_Service_PulseIn, CHANGE); 

  Serial.begin (115200, SERIAL_8N1, PIN_RX_0, PIN_TX_0); 
  Serial1.begin(115200, SERIAL_8N1, PIN_RX_1, PIN_TX_1); 

  delay(50); 

//------------------------------------------------------ 
  tmr_cnt = 0; 
  error = 0; 

  t_old        = 0; 
  t_on         = 0; 
  t_off        = 0; 
  state_ix     = 0; 
  state_ix_mon = 0; 
  p_store      = 0; 
  p_count      = 0; 

  p_buff_ix    = 0; 
  p_total      = 0; 
  p_buff_use   = 0; 
  p_buff_old   = 0; 

  for (int bix = 0; bix < 40; bix++) { p_buff[bix] = 0; } 

  req_reset    = 0; 

} 

void loop() { 
//------------------------------------------------------ 
  p_store = p_count; 
  p_count = 0; 

  if (req_reset == 1) { 
    req_reset = 0; 
    if (state_ix > 1) { 
      state_ix = 1; 
    } 
  } 

  selector_val = 0; 
  for (int bix = 0; bix < 4; bix++) { 
    if (digitalRead(selector_pin[bix]) == 0) { 
      selector_val += pow(2, bix); 
    } 
  } 

  if (state_ix != 2) { 
    switch (selector_val) { 
      default: 
      case 0: p_buff_use =   5; break; 
      case 1: p_buff_use =  10; break; 
      case 2: p_buff_use =  15; break; 
      case 3: p_buff_use =  20; break; 
      case 4: p_buff_use =  25; break; 
      case 5: p_buff_use =  30; break; 
      case 6: p_buff_use =  40; break; 
      case 7: p_buff_use =  50; break; 
      case 8: p_buff_use =  60; break; 
      case 9: p_buff_use = 100; break; 
    } 

    if (p_buff_old != p_buff_use) { 
      p_buff_old = p_buff_use; 
      p_total = 0; 
    } 
  } 

//------------------------------------------------------ 
  state_ix_mon = state_ix; 
  switch (state_ix) { 
    default: 
    case 0: //<- Initialize 
      Serial.println("\n\nRID Project 3"); 

      display.begin(SSD1306_SWITCHCAPVCC, 0x3C); 
      display.setTextSize(1); 
      display.setTextColor(WHITE); 

      display.clearDisplay(); 
      display.setCursor(0, GLCD_LINE_0); 
      display.println(F("RID Project 3")); 
      display.display(); 

      clcd.begin(16, 2); 
      clcd.init(); 
      clcd.setBacklight(255); 

      p_buff_ix = p_buff_use; 
      state_ix = 3; 
      break; 

    case 1: //<- Clear 
      p_buff_ix = 0; 
      p_total = 0; 

      for (int bix = 0; bix < P_BUFF_MAX; bix++) { 
        p_buff[bix] = 0; 
      } 

      state_ix++; 
      break; 

    case 2: //<- Run.. 
      if (p_buff_ix < p_buff_use) { 
        p_buff[p_buff_ix] = p_store; 
        p_buff_ix++; 

        p_total = 0; 
        for (int bix = 0; bix < p_buff_use; bix++) { 
          p_total += p_buff[bix]; 
        } 
      } else { 
        state_ix++; 
      } 
      break; 

    case 3: //<- Hold 
      break; 
  } 

//------------------------------------------------------ 
  if (state_ix > 0) { 
    sprintf(buff_str, "%d", state_ix_mon); 
    Serial.print(buff_str); 

    Serial.print(" | "); 

    if (state_ix_mon != 2) { 
      sprintf(buff_str, "Cnt %6lu /%3ds", p_total, p_buff_use); 
    } else { 
      sprintf(buff_str, "Cnt %6lu /%3ds", p_total, p_buff_ix); 
    } 

    Serial.print(buff_str); 

    if (state_ix_mon != 2) { 
      sprintf(buff_str, "Cnt %6lu /%ds  ", p_total, p_buff_use); 
    } else { 
      sprintf(buff_str, "Cnt %6lu /%ds  ", p_total, p_buff_ix); 
    } 

    clcd.setCursor(0, CLCD_LINE_0); 
    clcd.print(buff_str); 

    Serial.print(" | "); 

    sprintf(buff_str, "Freq %5lu Hz  ", p_store); 
    Serial.print(buff_str); 
    clcd.setCursor(0, CLCD_LINE_1); 
    clcd.print(buff_str); 

    Serial.println(); 
  } 

//------------------------------------------------------ 
  while ((micros() - t_old) < 1000000L); t_old = micros(); 
  tmr_cnt++; if (tmr_cnt >= 10) { tmr_cnt = 0; } 

} 
