#ifndef __WEB_RELAY_H__
#define __WEB_RELAY_H__

#define RLY0 12
#define RLY1 5
#define RLY2 4
#define RLY3 15

#define SW0 0
#define SW1 9
#define SW2 10
#define SW3 14

#define DEBOUNCE_TIME_MS 500
#define WAIT_FOR_EVAL 100

#define MMIN(__V1__, __V2__) (__V1__<__V2__?__V1__:__V2__)
#define MMAX(__V1__, __V2__) (__V1__>__V2__?__V1__:__V2__)
bool HAD_SW_INTERRUPT[] = {false, false, false, false};
unsigned long debouncetime[4] = {millis(), millis(), millis(), millis()};

#define SW_METHOD(__PIN__, __RLY__) void ICACHE_RAM_ATTR SwitchDown##__PIN__ ();\
void SwitchDown##__PIN__(){\
  unsigned long now = millis();\  
  if (now - debouncetime[__PIN__] > DEBOUNCE_TIME_MS) {\
    Serial.println("DOWN");\
    HAD_SW_INTERRUPT[__PIN__] = true;\
    debouncetime[__PIN__] = now;\
  }\
}

#define CHECK_STATE(__PIN__, __RLY__) if (HAD_SW_INTERRUPT[__PIN__] && (now - debouncetime[__PIN__] < WAIT_FOR_EVAL)) {\
    HAD_SW_INTERRUPT[__PIN__] = false;\
    Serial.println("PROC");\
    toggle(__PIN__, __RLY__);\
}



uint8_t LEDMode = 1;
uint8_t lastMode = 0xff;
double LEDSpeed = 20.0;
boolean LEDOn = false;
CHSV color = CHSV(0xFF, 0x0, 0xFF);


void postJSON(int nr, bool on){
  String s;
  if (nr!=3) {
    s = String("{\"on\":") + String(on?"true":"false") + ", \"relay\":" + nr + "}";
  } else {
      s = "{\"on\": " + String(on?"true":"false")  + ", \"relay\":3, \"mode\":" + String(LEDMode) + ", \"h\":" + String(color.h) + ", \"s\":" + String(color.s) + ", \"v\":" + String(color.v) + ", \"speed\":" + String(LEDSpeed) + "}";
  }
  http.begin("192.168.55.21", 8081, String("/theoutside/g") + String(nr) + String("/cb"));
  http.addHeader("Content-Type", "application/json");
  int httpCode = http.POST(s);
  http.end();
}

SW_METHOD(0, RLY0)
SW_METHOD(1, RLY1)
SW_METHOD(2, RLY2)
SW_METHOD(3, RLY3)

void turnOn(int nr, int rly){
    digitalWrite(rly, HIGH);   
    postJSON(nr, true); 
    if (rly==RLY3) LEDOn = true;
}

void turnOff(int nr, int rly){
    digitalWrite(rly, LOW);
    postJSON(nr, false);
    if (rly==RLY3) LEDOn = false;
}

bool isOn(int rly){
    bool on = digitalRead(rly) == HIGH;
    if (rly==RLY3) LEDOn = on;
    return on;
}

void toggle(int nr, int rly){
    bool on = !isOn(rly);
    digitalWrite(rly, on);
    if (rly==RLY3) LEDOn = on;
    postJSON(nr, on);
}

void allOff(){
    turnOff(0, RLY0);
    turnOff(1, RLY1);
    turnOff(2, RLY2);
    turnOff(3, RLY3);
}

void sendLEDState(){
        webServer.send(200, "text/json", "[{\"on\": " + (isOn(RLY3)?String("true"):String(false)) + ", \"relay\":3, \"mode\":" + String(LEDMode) + ", \"h\":" + String(color.h) + ", \"s\":" + String(color.s) + ", \"v\":" + String(color.v) + ", \"speed\":" + String(LEDSpeed) + "}]");    
}


void rly_server_setup() {
    pinMode(RLY0, OUTPUT);
    pinMode(RLY1, OUTPUT);
    pinMode(RLY2, OUTPUT);
    pinMode(RLY3, OUTPUT);

    pinMode(SW0, INPUT_PULLUP);
    pinMode(SW1, INPUT_PULLUP);
    pinMode(SW2, INPUT_PULLUP);
    pinMode(SW3, INPUT_PULLUP);
    
    attachInterrupt(digitalPinToInterrupt(SW0), SwitchDown0, FALLING);
    attachInterrupt(digitalPinToInterrupt(SW1), SwitchDown1, FALLING);
    attachInterrupt(digitalPinToInterrupt(SW2), SwitchDown2, FALLING);
    attachInterrupt(digitalPinToInterrupt(SW3), SwitchDown3, FALLING);

    webServer.on("/on", []() {
        turnOn(0, RLY0);
        turnOn(1, RLY1);
        turnOn(2, RLY2);
        turnOn(3, RLY3);
        webServer.send(200, "text/json", "[{\"on\":true, \"relay\":0}, {\"on\":true, \"relay\":1}, {\"on\":true, \"relay\":2}, {\"on\":true, \"relay\":3}]");
    });

    webServer.on("/on/0", []() {
        turnOn(0, RLY0);
        webServer.send(200, "text/json", "[{\"on\":true, \"relay\":0}]");
    });

    webServer.on("/on/1", []() {
        turnOn(1, RLY1);
        webServer.send(200, "text/json", "[{\"on\":true, \"relay\":1}]");
    });

    webServer.on("/on/2", []() {
        turnOn(2, RLY2);
        webServer.send(200, "text/json", "[{\"on\":true, \"relay\":2}]");
    });

    webServer.on("/on/3", []() {
        turnOn(3, RLY3);
        webServer.send(200, "text/json", "[{\"on\":true, \"relay\":3}]");
    });

    webServer.on("/off", []() {
        allOff();
        webServer.send(200, "text/json", "[{\"on\":false, \"relay\":0}, {\"on\":false, \"relay\":1}, {\"on\":false, \"relay\":2}, {\"on\":false, \"relay\":3}]");
    });

    webServer.on("/off/0", []() {
        turnOff(0, RLY0);
        webServer.send(200, "text/json", "[{\"on\":false, \"relay\":0}]");
    });

    webServer.on("/off/1", []() {
        turnOff(1, RLY1);
        webServer.send(200, "text/json", "[{\"on\":false, \"relay\":1}]");        
    });

    webServer.on("/off/2", []() {
        turnOff(2, RLY2);
        webServer.send(200, "text/json", "[{\"on\":false, \"relay\":2}]");
    });

    webServer.on("/off/3", []() {
        turnOff(3, RLY3);
        webServer.send(200, "text/json", "[{\"on\":false, \"relay\":3}]");
    });

    webServer.on("/state", []() {
        String lead ="{\"on\":";
        String t= "true";
        String f= "false";
        String s = "[";
        s += lead + (isOn(RLY0)?t:f) +", \"relay\":0}, ";
        s += lead + (isOn(RLY1)?t:f) +", \"relay\":1}, ";
        s += lead + (isOn(RLY2)?t:f) +", \"relay\":2}, ";
        s += lead + (isOn(RLY3)?t:f) +", \"relay\":3}";
        s += "]";        
        webServer.send(200, "text/json", s);
    });

    webServer.on("/state/0", []() {
        if ( isOn(RLY0) ){
            webServer.send(200, "text/json", "[{\"on\":true, \"relay\":0}]");
        } else {
            webServer.send(200, "text/json", "[{\"on\":false, \"relay\":0}]");
        }
    });

    webServer.on("/state/1", []() {
        if ( isOn(RLY1) ){
            webServer.send(200, "text/json", "[{\"on\":true, \"relay\":1}]");
        } else {
            webServer.send(200, "text/json", "[{\"on\":false, \"relay\":1}]");
        }
    });

    webServer.on("/state/2", []() {
        if ( isOn(RLY2) ){
            webServer.send(200, "text/json", "[{\"on\":true, \"relay\":2}]");
        } else {
            webServer.send(200, "text/json", "[{\"on\":false, \"relay\":2}]");
        }
    });    

    webServer.on("/state/3", []() {
           sendLEDState();
    });

    webServer.on("/mode/3", HTTPMethod::HTTP_POST, []() {
        String modeStr = webServer.arg("mode");
        LEDMode = modeStr.toInt();
        modeStr = webServer.arg("speed");
        LEDSpeed = modeStr.toDouble();
        
        sendLEDState();
    });

    webServer.on("/color/3", HTTPMethod::HTTP_POST, []() {
        String str = webServer.arg("h");
        int h = str.toInt();
        str = webServer.arg("s");
        int s = str.toInt();
        str = webServer.arg("v");
        int v = MMAX(32, str.toInt());

        color = CHSV(h%0x100, s%0x100, v%0x100);
        lastMode = 0xFF;
        sendLEDState();
    });
}


void start_ota();
bool rly_server_loop(){
    unsigned long now = millis();
    if (HAD_SW_INTERRUPT[0] && HAD_SW_INTERRUPT[2] && HAD_SW_INTERRUPT[3]){
        HAD_SW_INTERRUPT[0] = false;
        HAD_SW_INTERRUPT[2] = false;
        HAD_SW_INTERRUPT[3] = false;

        if (digitalRead(SW0)==LOW && digitalRead(SW2)==LOW && digitalRead(SW3)==LOW){
            Serial.println("RESET");
            allOff();
            digitalWrite(POWER_LED, LOW);
            ESP.restart();
        }
    } else if (HAD_SW_INTERRUPT[0] && HAD_SW_INTERRUPT[3]){
        HAD_SW_INTERRUPT[0] = false;
        HAD_SW_INTERRUPT[3] = false;

        if (digitalRead(SW0)==LOW && digitalRead(SW3)==LOW){
            digitalWrite(POWER_LED, LOW);
            start_ota();
            Serial.println("OTA");
        }
    }
    CHECK_STATE(0, RLY0)
    CHECK_STATE(1, RLY1)
    CHECK_STATE(2, RLY2)
    CHECK_STATE(3, RLY3)

    return false;
}
#endif