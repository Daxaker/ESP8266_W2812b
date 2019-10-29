#ifndef NETWORKING_h
#define NETWORKING_h

#include <Arduino.h>
#include <ESP8266WiFi.h>

#include <PubSubClient.h>
#include <JeVe_EasyOTA.h>

#ifdef EXTERNAL_CREDENTIALS
#include <credential.h>
#else
#ifndef CREDENTIALS_h
#define SSID "..."
#define PASSWD "..."
#define MQTT_SERVER "192.168.0.101"
#define PORT 1883
#endif
#endif
#define MQTT_MAX_PACKET_SIZE 1024

#define ESP_HOSTNAME "esp"

extern EasyOTA overTheAir;
extern WiFiClient espClient;
extern PubSubClient client;

void setup_wifi();
void setup_mqtt(MQTT_CALLBACK_SIGNATURE);
void setup_ota();
void reconnect_mqtt();

#endif