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
#define TEST_LED 0
#define NUMPIXELS 10
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
  FastLED.addLeds<WS2812B, DATA_PIN>(leds, NUMPIXELS);
}

// the loop function runs over and over again forever
void loop() {
  #if TEST_LED==1
  static long ledup = millis();
  static int ledcol = 0;
  if ((millis() - ledup) > 50){
    ledup = millis();
    ledcol += 10;
    Serial.print("Color: ");
    Serial.println(ledcol);
    if (ledcol > 0xff) ledcol-=0xff;
    for (int i=0; i<NUMPIXELS; i++){
      leds[i] = CHSV((ledcol + (0xff*i)/NUMPIXELS) % 0xFF, 0xFF, 0xFF);
    }
    FastLED.show();
  }
  #endif

  #ifdef __MY_WEB_SERVER_H__
  server_loop();
  #endif
                         // wait for a second
}
