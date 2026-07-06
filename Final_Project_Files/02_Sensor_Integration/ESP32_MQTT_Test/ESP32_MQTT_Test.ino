// ─────────────────────────────────────────────
// Connects to Mosquitto broker on Raspberry Pi,
// publishes a counter every 2 seconds,
// and echoes any received command messages.
// Run WiFi test (Step 2c) successfully first.
// Board  : ESP32 DevKitC V4
// Library: PubSubClient
// ─────────────────────────────────────────────

#include <WiFi.h>
#include <PubSubClient.h>

const char* SSID        = "YOUR_WIFI_SSID";
const char* PASSWORD    = "YOUR_WIFI_PASSWORD";
const char* MQTT_SERVER = "192.168.1.50";   // Raspberry Pi IP
const int   MQTT_PORT   = 1883;
const char* PUB_TOPIC   = "test/node01";
const char* SUB_TOPIC   = "test/node01/cmd";

WiFiClient   wifiClient;
PubSubClient mqtt(wifiClient);

int counter = 0;

void onMqttMessage(char* topic, byte* payload, unsigned int len) {
  char buf[64] = {0};
  if (len < sizeof(buf)) memcpy(buf, payload, len);
  Serial.printf("Received [%s]: %s\n", topic, buf);
}

void connectWiFi() {
  Serial.printf("Connecting to %s", SSID);
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) { Serial.print("."); delay(500); }
  Serial.printf("\nWiFi OK — IP: %s\n", WiFi.localIP().toString().c_str());
}

void connectMQTT() {
  while (!mqtt.connected()) {
    Serial.print("MQTT connecting...");
    if (mqtt.connect("ESP32_MQTTTest")) {
      Serial.println(" connected");
      mqtt.subscribe(SUB_TOPIC);
      Serial.printf("Subscribed to: %s\n", SUB_TOPIC);
    } else {
      Serial.printf(" failed (rc=%d) — retry in 5s\n", mqtt.state());
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  connectWiFi();
  mqtt.setServer(MQTT_SERVER, MQTT_PORT);
  mqtt.setCallback(onMqttMessage);
  connectMQTT();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) connectWiFi();
  if (!mqtt.connected())             connectMQTT();
  mqtt.loop();

  static unsigned long lastPub = 0;
  if (millis() - lastPub > 2000) {
    lastPub = millis();
    char msg[32];
    snprintf(msg, sizeof(msg), "COUNT:%d", counter++);
    mqtt.publish(PUB_TOPIC, msg);
    Serial.printf("Published: %s\n", msg);
  }
}
