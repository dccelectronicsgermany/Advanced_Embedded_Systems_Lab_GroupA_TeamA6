// ─────────────────────────────────────────────
// Tests WiFi connection with automatic reconnect
// before adding MQTT on top.
// Board: ESP32 DevKitC V4
// ─────────────────────────────────────────────

#include <WiFi.h>

const char* SSID     = "Hello_World";
const char* PASSWORD = "ReeW@z007";

unsigned long lastAttempt = 0;
const unsigned long RETRY_INTERVAL = 5000;

void connectWiFi() {
  if (WiFi.status() == WL_CONNECTED) return;
  Serial.printf("Connecting to %s", SSID);
  WiFi.begin(SSID, PASSWORD);
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 10000) {
    Serial.print(".");
    delay(500);
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConnected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.printf("Signal strength (RSSI): %d dBm\n", WiFi.RSSI());
  } else {
    Serial.println("\nFailed — will retry");
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  connectWiFi();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED && millis() - lastAttempt > RETRY_INTERVAL) {
    lastAttempt = millis();
    Serial.println("WiFi dropped — reconnecting...");
    connectWiFi();
  }

  static unsigned long lastPrint = 0;
  if (millis() - lastPrint > 5000) {
    lastPrint = millis();
    if (WiFi.status() == WL_CONNECTED) {
      Serial.printf("WiFi OK | IP: %s | RSSI: %d dBm\n",
        WiFi.localIP().toString().c_str(), WiFi.RSSI());
    } else {
      Serial.println("WiFi: DISCONNECTED");
    }
  }
}
