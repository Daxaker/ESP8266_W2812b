#include <networking.h>

EasyOTA overTheAir(ESP_HOSTNAME);
WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  WiFi.begin(SSID, PASSWD);
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

void OnOTAMsg(const String& message, int line) {
  Serial.println(message);
}

void setup_ota(){
  overTheAir.onMessage(OnOTAMsg);
  overTheAir.addAP(SSID, PASSWD);
}

void setup_mqtt(MQTT_CALLBACK_SIGNATURE){
  client.setServer(MQTT_SERVER, PORT);
  client.setCallback(callback);
}

void reconnect_mqtt() {
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
      client.subscribe("ledMatrix/toggleFX");
      client.subscribe("ledMatrix/changeFX");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}