/*
  Blink

  Turns an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the UNO, MEGA and ZERO
  it is attached to digital pin 13, on MKR1000 on pin 6. LED_BUILTIN is set to
  the correct LED pin independent of which board is used.
  If you want to know what pin the on-board LED is connected to on your Arduino
  model, check the Technical Specs of your board at:
  https://www.arduino.cc/en/Main/Products

  modified 8 May 2014
  by Scott Fitzgerald
  modified 2 Sep 2016
  by Arturo Guadalupi
  modified 8 Sep 2016
  by Colby Newman

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/Blink
*/
#include <Arduino.h>
#include <FastLED.h>

#define POWER_LED 13
#define TEST_LED 1
#define NUMPIXELS 369
#define DATA_PIN 2


#include "secret.h"
const char* ssid = STASSID;
const char* password = STAPSK;
const char* host = "OUTDOOR";
#include "WebServer.h"


CRGB leds[NUMPIXELS];

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(115200);
  pinMode(POWER_LED, OUTPUT);

  WiFi.mode(WIFI_STA);

  WiFi.begin(ssid, password);
  //Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(POWER_LED, !digitalRead(POWER_LED));
    delay(300);
    Serial.print(".");
  }
  digitalWrite(POWER_LED, HIGH);

  
  #ifdef __MY_WEB_SERVER_H__
  server_setup();
  #endif

  FastLED.setBrightness(0xee);
  FastLED.addLeds<WS2812B, DATA_PIN, EOrder::GRB>(leds, NUMPIXELS);

  for (int i=0; i<NUMPIXELS; i++) leds[i] = CRGB(0,0,0);
}

inline uint16_t LEDIdx(int i){
  if (i>295) return i + 6;
  return i + 2;
}

inline uint16_t LEDCount(){
  return NUMPIXELS - 6;
}

double playTime = 0;
// the loop function runs over and over again forever
void loop() {
  static long ledup = millis();
  static int ledcol = 0;
  
  

  #ifdef __MY_WEB_SERVER_H__
  server_loop();
  #endif

  #if TEST_LED==1
  bool staticLight = (LEDMode == 0);
  if (!LEDOn || (staticLight && LEDMode == lastMode)){
    delay(100);
    playTime = 0;    
  } else {
    unsigned long now = millis();
    unsigned long delta = now - ledup;
    static int pos = 0;    
    if (delta > 20){   
      playTime += delta * LEDSpeed / 2000.0;
      ledup = now;
      ledcol = ((int)round(playTime)) % 0x100;
      int sub = fmax(2, LEDSpeed * 2);
      if (ledcol > 0xff) ledcol-=0xff;
      if (LEDMode == 5){
          const int LOAD_STEPS = 40;
          const int count = 12;
          int offset = 0;
          if (pos> (count+2) * LOAD_STEPS) {
            offset = pos - (count+2) * LOAD_STEPS;
          }
          if (offset > LEDCount()/2 - count + 20 || lastMode != LEDMode) pos = 0;          
          if (pos==0){
            for (int k=0; k<NUMPIXELS; k++) leds[k] = CRGB(0,0,0);
          }

          if (offset>0){
            pos += 1 + LEDSpeed/2;
            for (int i=0; i<LEDCount()/2; i++){
              const CRGB cc = (i>=offset && i<offset+count*2) ? color : CRGB(0,0,0);
              leds[LEDIdx(i)] = cc;
              leds[LEDIdx(LEDCount()-i-1)] = cc;
            }
          } else {
            // int b = pos;
            // for (int i=0; i<16; i++){            
            //   leds[LEDIdx(20+i)] = CRGB(0,0, b%2 == 0?0:0xff);
            //   b/=2;
            // }            
            
            for (int i=1; i<8; i++) {
              if ( pos >= (i-1) * LOAD_STEPS && pos <= i * LOAD_STEPS) {
                const double ppos = pos - (i-1) * LOAD_STEPS;
                CHSV c = rgb2hsv_approximate(color);
                c.v = fmax(0, fmin(0xff, (ppos / LOAD_STEPS) * 0xFF));
                leds[LEDIdx(i-1)] = c;
                leds[LEDIdx(LEDCount()-i)] = c;

                break;
              }
            }
            
            pos += 1 + LEDSpeed/5;
          }
      } else {
        for (int i=0; i<LEDCount(); i++){
          if (LEDMode == 0){
            leds[LEDIdx(i)] = color;
          } else if (LEDMode == 1) {
            leds[LEDIdx(i)] = CHSV((ledcol + (0xff*LEDIdx(i))/LEDCount()) % 0x100, 0xFF, color.v);
          } else if (LEDMode == 2) {
            leds[LEDIdx(i)] = CHSV(ledcol % 0x100, 0xFF, color.v);          
          } else if (LEDMode == 3 || LEDMode == 4){
            if (random(10000)>9985 - LEDSpeed){
              if (LEDMode == 3) {
                leds[LEDIdx(i)] = color;
              } else {
                leds[LEDIdx(i)] = CHSV(random(0xff), 0xFF, color.v);
              }
            } else {
              const int lsub = sub + random(fmax(3, sub/2));
              //const CHSV c = rgb2hsv_approximate(leds[LEDIdx(i)]);
              //leds[LEDIdx(i)] = CHSV(c.h, c.s, fmax(0, c.v - sub));
              const CRGB c = leds[LEDIdx(i)];
              leds[LEDIdx(i)] = CRGB(fmax(0, c.r - lsub), fmax(0, c.g - lsub), fmax(0, c.b - lsub));
            }
          } else if (LEDMode == 5){
            //nothing to loop;
          } else {
            
          }
        }
      }
      lastMode = LEDMode;
      FastLED.show();
    }
  }
  #endif

  
                         // wait for a second
}
