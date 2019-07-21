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
    toggle(__RLY__);\
}

SW_METHOD(0, RLY0)
SW_METHOD(1, RLY1)
SW_METHOD(2, RLY2)
SW_METHOD(3, RLY3)


void toggle(int rly){
    digitalWrite(rly, !digitalRead(rly));
}

void turnOn(int rly){
    digitalWrite(rly, HIGH);
}

void turnOff(int rly){
    digitalWrite(rly, LOW);
}

bool isOn(int rly){
    bool on = digitalRead(rly) == HIGH;
    return on;
}

void allOff(){
    turnOff(RLY0);
    turnOff(RLY1);
    turnOff(RLY2);
    turnOff(RLY3);
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
        turnOn(RLY0);
        turnOn(RLY1);
        turnOn(RLY2);
        turnOn(RLY3);
        webServer.send(200, "text/json", "[{\"on\":true, \"relay\":0}, {\"on\":true, \"relay\":1}, {\"on\":true, \"relay\":2}, {\"on\":true, \"relay\":3}]");
    });

    webServer.on("/on/0", []() {
        turnOn(RLY0);
        webServer.send(200, "text/json", "[{\"on\":true, \"relay\":0}]");
    });

    webServer.on("/on/1", []() {
        turnOn(RLY1);
        webServer.send(200, "text/json", "[{\"on\":true, \"relay\":1}]");
    });

    webServer.on("/on/2", []() {
        turnOn(RLY2);
        webServer.send(200, "text/json", "[{\"on\":true, \"relay\":2}]");
    });

    webServer.on("/on/3", []() {
        turnOn(RLY3);
        webServer.send(200, "text/json", "[{\"on\":true, \"relay\":3}]");
    });

    webServer.on("/off", []() {
        allOff();
        webServer.send(200, "text/json", "[{\"on\":false, \"relay\":0}, {\"on\":false, \"relay\":1}, {\"on\":false, \"relay\":2}, {\"on\":false, \"relay\":3}]");
    });

    webServer.on("/off/0", []() {
        turnOff(RLY0);
        webServer.send(200, "text/json", "[{\"on\":false, \"relay\":0}]");
    });

    webServer.on("/off/1", []() {
        turnOff(RLY1);
        webServer.send(200, "text/json", "[{\"on\":false, \"relay\":1}]");
    });

    webServer.on("/off/2", []() {
        turnOff(RLY2);
        webServer.send(200, "text/json", "[{\"on\":false, \"relay\":2}]");
    });

    webServer.on("/off/3", []() {
        turnOff(RLY3);
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
        if ( isOn(RLY3) ){
            webServer.send(200, "text/json", "[{\"on\":true, \"relay\":3}]");
        } else {
            webServer.send(200, "text/json", "[{\"on\":false, \"relay\":3}]");
        }
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