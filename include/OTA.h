#ifndef __OTA_H__
#define __OTA_H__

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

unsigned long ota_timeout = 60*1000;
unsigned long ota_timeout_start;
bool ota_on;

void ota_setup() {  
  /* switch on led */
  pinMode(POWER_LED, OUTPUT);
  digitalWrite(POWER_LED, LOW);

  Serial.println("Booting");  
  
  /* switch off led */
  digitalWrite(POWER_LED, HIGH);

  ArduinoOTA.setHostname(host);
  ArduinoOTA.setPort(OTA_PORT);
  ArduinoOTA.setPassword(OTA_PASS);

  ArduinoOTA.onStart([]() { 
    Serial.println("START");
    digitalWrite(POWER_LED, LOW);
  });

  ArduinoOTA.onEnd([]() { 
    Serial.println("END");
    digitalWrite(POWER_LED, LOW);
  });

  ArduinoOTA.onError([](ota_error_t error) {
    (void)error;
    Serial.print("ERROR: ");
    Serial.println(error);

    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
    ESP.restart();
  });

  /* setup the OTA server */
  ArduinoOTA.begin();
  
  Serial.println("Ready");
  Serial.print("IP address: ");
}

void ota_loop() {
  Serial.print("Checking for OTA on ");
  Serial.println(WiFi.localIP());
  ArduinoOTA.handle();
}

void start_ota(){
    webServer.stop();

    #ifdef __WEB_RELAY_H__
      allOff();
    #endif    
    delay(100);

    #ifdef __OTA_H__
      ota_setup();
    #endif
    ota_timeout_start = millis();
    ota_on = true;
}

void ota_server_setup() {
  webServer.on("/ota", []() {
    start_ota();
    webServer.send(200, "text/json", "{\"ok\":true}");
  });
  ota_timeout_start = millis();
  ota_on = false;
}

bool ota_server_loop(){
  if (ota_on){
    if (millis() - ota_timeout_start < ota_timeout) {
      #ifdef __OTA_H__
        ota_loop();
      #endif
      delay(1000);
      digitalWrite(POWER_LED, !digitalRead(POWER_LED));
      return true;
    } else {
      ESP.restart();
      ota_on = false;
      digitalWrite(POWER_LED, HIGH);
    }
  }
  return false;
}


#endif