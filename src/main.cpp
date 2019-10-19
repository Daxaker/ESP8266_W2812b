#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <credential.h>
#include <Adafruit_NeoPixel.h>

#define MQTT_MAX_PACKET_SIZE 1024

WiFiClient espClient;
PubSubClient client(espClient);

#define LED_PIN 4
#define LED_COUNT 256

Adafruit_NeoPixel matrix(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

#ifndef CREDENTIALS
const char* ssid = "....";
const char* passwd = "....";
const char* mqtt_server = "....";
const int port = 1883;
#endif

long lastMsg = 0;
char msg[50];
int value = 0;

void setXYColor(uint8_t x, uint8_t y, uint32_t c){
  uint8_t n = (16 * x) + (x % 2 == 0 ? y : (15 - y));
  matrix.setPixelColor(n, c);
}

void setXYColor(uint8_t x, uint8_t y, uint8_t r, uint8_t g, uint8_t b){
  uint8_t n = (16 * x) + (x % 2 == 0 ? y : (15 - y));
  matrix.setPixelColor(n, r, g, b);
}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  WiFi.begin(ssid, passwd);
  Serial.println();
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  randomSeed(micros());
  Serial.println();
  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());
}

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
    setXYColor(x, y, r, g, b);
    matrix.show();
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
        setXYColor(x,y, r,g,b);
    }
    matrix.show();
  }
  else {
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
  client.setServer(mqtt_server, port);
  client.setCallback(callback);
  matrix.begin();
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
      client.subscribe("ledMatrix/xy");
      client.subscribe("ledMatrix/blob");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop() {
 if (!client.connected()) {
    reconnect();
  }
  client.loop();
}