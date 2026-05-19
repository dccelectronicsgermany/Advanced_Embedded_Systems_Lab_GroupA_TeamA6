#include <WiFi.h>
#include <PubSubClient.h>

// WiFi credentials
const char* ssid     = "Pegasus";
const char* password = "Nepal@007";

// MQTT Broker
const char* broker = "broker.hivemq.com";
const int   port   = 1883;

WiFiClient   espClient;
PubSubClient client(espClient);

// This function is called automatically when a message arrives
void callback(char* topic, byte* payload, unsigned int length) {
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.print("[");
  Serial.print(topic);
  Serial.print("] → ");
  Serial.println(message);
}

void connectWiFi() {
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected! IP: " + WiFi.localIP().toString());
}

void connectMQTT() {
  client.setServer(broker, port);
  client.setCallback(callback);

  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");
    String clientId = "ESP32-" + String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("connected!");
      // Subscribe to all 3 Arduino topics
      client.subscribe("teamA6/device1/temperature");
      client.subscribe("teamA6/device2/temperature");
      client.subscribe("teamA6/device3/temperature");
      Serial.println("Subscribed to all topics!");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" retrying in 3s...");
      delay(3000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  connectWiFi();
  connectMQTT();
}

void loop() {
  if (!client.connected()) {
    connectMQTT(); // reconnect if dropped
  }
  client.loop(); // listens for incoming messages
}
