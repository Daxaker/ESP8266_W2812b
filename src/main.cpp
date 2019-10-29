#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <FastLED.h>
#include <networking.h>
#include <ledMatrix.h>
#include <routine.h>

bool fx = false;
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  // for (int i = 0; i < length; i++) {
  //   Serial.print((char)payload[i]);
  // }
  Serial.println();

  if(strcmp(topic, "ledMatrix/xy")==0){
    if(length < 5){
      Serial.println("Incorrect payload!");
      return;
    }
    auto x = (uint8_t)payload[0];
    Serial.print("x:");
    Serial.println(x);
    auto y = (uint8_t)payload[1];
    Serial.print("y:");
    Serial.println(y);
    auto r = (uint8_t)payload[2];
    Serial.print("r:");
    Serial.println(r);
    auto g = (uint8_t)payload[3];
    Serial.print("g:");
    Serial.println(g);
    auto b = (uint8_t)payload[4];
    Serial.print("b:");
    Serial.println(b);

    drawPixelXY(x, y, CRGB(r,g,b));
    FastLED.show();
  }
  else if(strcmp("ledMatrix/blob", topic) == 0) {
    for(uint32_t index = 0; index < length; index+=3){
        uint32_t x = (index / 3) / 16;
        uint32_t y = (index / 3) % 16;
        Serial.print("x:");
        Serial.print(x);
        Serial.print(" y:");
        Serial.println(y);
        //RGB color 24-bit
        auto r = (uint8_t)payload[index];
        Serial.print("r:");
        Serial.print(r);
        auto g = (uint8_t)payload[index+1];
        Serial.print(" g:");
        Serial.print(g);
        auto b = (uint8_t)payload[index+2];
        Serial.print(" b:");
        Serial.println(b);
        drawPixelXY(x,y, CRGB(r,g,b));
    }
    FastLED.show();
  }
  else if(strcmp("ledMatrix/toggleFX", topic) == 0) { 
    fx = !fx;
  }
  else if(strcmp("ledMatrix/changeFX", topic) == 0) {
    fx = true;
    nextRoutine();
  }
  else if(strcmp("ledMatrix/setFX", topic) == 0){
    uint8_t routineId = payload[0];
    setRoutine(routineId);
  }
  else if(strcmp("inTopic", topic) == 0) {
    // Switch on the LED if an 1 was received as first character
    if ((char)payload[0] == '1') {
      digitalWrite(D4, LOW);   // Turn the LED on (Note that LOW is the voltage level
      // but actually the LED is on; this is because
      // it is active low on the ESP-01)
    } else {
      digitalWrite(D4, HIGH);  // Turn the LED off by making the voltage HIGH
    }
  }
}

void setup() {
  pinMode(D4, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  setup_wifi();
  setup_mqtt(callback);
  setup_ota();  
  init_ledMatrix();
  initRoutines();
}

void loop() {
    if(fx) {
       loopRoutine();
    }

    if (!client.connected()) {
        reconnect_mqtt();
    }
    
    overTheAir.loop();
    client.loop();
}