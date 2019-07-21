#ifndef __MY_WEB_SERVER_H__
#define __MY_WEB_SERVER_H__

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ESP8266mDNS.h>
#include <FS.h> 



#if WITH_HTTPS==1
#include <ESP8266WebServerSecure.h>
static const uint8_t rsakey[] PROGMEM = {
    #include "key.h"
};
static const uint8_t x509[] PROGMEM = {
    #include "x509.h"
};
ESP8266WebServer webServerSecure(443);
#else
#include <ESP8266WebServer.h>
ESP8266WebServer webServer(80);
#endif


#include "Relay.h"
#include "OTA.h"


String getContentType(String filename) { // convert the file extension to the MIME type
  if (filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  return "text/plain";
}

bool handleFileRead(String path) { // send the right file to the client (if it exists)
  Serial.println("handleFileRead: " + path);
  if (path.endsWith("/")) path += "index.html";         // If a folder is requested, send the index file
  String contentType = getContentType(path);            // Get the MIME type
  if (SPIFFS.exists(path)) {                            // If the file exists
    File file = SPIFFS.open(path, "r");                 // Open it
    size_t sent = webServer.streamFile(file, contentType); // And send it to the client
    file.close();                                       // Then close the file again
    return true;
  }
  Serial.println("\tFile Not Found");
  return false;                                         // If the file doesn't exist, return false
}

void handleNotFound() {
  digitalWrite(POWER_LED, LOW);

  if (handleFileRead(webServer.uri())){
    digitalWrite(POWER_LED, HIGH);
    return;
  }

  String message = "File Not Found\n\n";
  message += "URI: ";
  message += webServer.uri();
  message += "\nMethod: ";
  message += (webServer.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += webServer.args();
  message += "\n";
  for (uint8_t i = 0; i < webServer.args(); i++) {
    message += " " + webServer.argName(i) + ": " + webServer.arg(i) + "\n";
  }
  webServer.send(404, "text/plain", message);
  digitalWrite(POWER_LED, HIGH);
}

void server_setup(void) {
  pinMode(POWER_LED, OUTPUT);
  digitalWrite(POWER_LED, HIGH);
  SPIFFS.begin(); 
  
  // if (MDNS.begin("outdoor")) {
  //   Serial.println("MDNS responder started");
  // } else {
  //   Serial.println("Error setting up MDNS responder!");
  // }

#if WITH_HTTPS==1
   webServer.setServerKeyAndCert_P(rsakey, sizeof(rsakey), x509, sizeof(x509));
#endif

  #ifdef __OTA_H__
    ota_server_setup();
  #endif
  #ifdef __WEB_RELAY_H__
    rly_server_setup();
  #endif
  
  webServer.onNotFound(handleNotFound);
   

  webServer.begin();
#if WITH_HTTPS==1
  Serial.print("HTTPS server started on ");
#else
Serial.print("HTTP server started on ");
#endif
  Serial.println(WiFi.localIP());
}



void server_loop(void) {
  #ifdef __OTA_H__
    if (ota_server_loop())
      return;
  #endif

  #ifdef __WEB_RELAY_H__
    if (rly_server_loop())
      return;
  #endif
  
  if (WiFi.status() != WL_CONNECTED){
    for (int i=0; i<3; i++){
      digitalWrite(POWER_LED, LOW);
      delay(200);
      digitalWrite(POWER_LED, HIGH);
      delay(100);
    }
    delay(1000);
  } else {
    webServer.handleClient();
    delay(100);
  }
}

#endif