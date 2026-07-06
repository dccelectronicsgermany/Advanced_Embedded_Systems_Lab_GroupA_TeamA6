#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <WiFi.h>
#include <PubSubClient.h>

// ── WiFi / MQTT ───────────────────────────────
const char* ssid     = "Hello_World";
const char* password = "ReeW@z007";
const char* brokerIP = "raspberrypia.local";
const char* topic    = "sensors/node01";
const char* cmdTopic = "sensors/node01/cmd";

// ── BLE UUIDs ─────────────────────────────────
#define SERVICE_UUID        "19B10000-A8F2-537E-4F6C-D104768A1214"
#define CHARACTERISTIC_UUID "19B10001-A8F2-537E-4F6C-D104768A1214"

// ── Pins ──────────────────────────────────────
#define PIN_TEMP  34
#define PIN_KNOCK 32

// ── Thresholds (adjustable from dashboard) ────
float tempWarning  = 25.0;
float tempCritical = 30.0;

// ── Mute flag (set from dashboard) ───────────
bool buzzerMuted = false;

// ── Force alert (set from dashboard button) ───
uint8_t forceAlert = 0;        // 0=none 1=WARNING 2=CRITICAL
unsigned long forceUntil = 0;  // holds for 5 seconds

// ── Knock accumulation state ──────────────────
#define KNOCK_WARN_COUNT  2
#define KNOCK_CRIT_COUNT  5
#define KNOCK_WARN_WINDOW 2000
#define KNOCK_CRIT_WINDOW 5000
#define KNOCK_HOLD        5000

int           knockAccum     = 0;
unsigned long knockFirstMs   = 0;
uint8_t       knockStatus    = 0;
unsigned long knockHoldUntil = 0;

// ── Objects ───────────────────────────────────
WiFiClient         wifiClient;
PubSubClient       mqttClient(wifiClient);
BLECharacteristic* pCharacteristic = NULL;

// ── MQTT command callback ─────────────────────
void onMqttMessage(char* t, byte* payload, unsigned int len) {
  char buf[64];
  if (len >= sizeof(buf)) return;
  memcpy(buf, payload, len);
  buf[len] = '\0';

  // CMD:WARN:25.5
  if (strncmp(buf, "CMD:WARN:", 9) == 0) {
    float v = atof(buf + 9);
    if (v >= 15.0 && v < tempCritical) {
      tempWarning = v;
      Serial.print(F("[CMD] Warn threshold → ")); Serial.println(tempWarning);
    }
  }
  // CMD:CRIT:30.0
  else if (strncmp(buf, "CMD:CRIT:", 9) == 0) {
    float v = atof(buf + 9);
    if (v >= 15.0 && v <= 60.0) {
      tempCritical = v;
      if (tempWarning >= tempCritical) tempWarning = tempCritical - 1.0;
      Serial.print(F("[CMD] Crit threshold → ")); Serial.println(tempCritical);
    }
  }
  // CMD:MUTE:1 or CMD:MUTE:0
  else if (strncmp(buf, "CMD:MUTE:", 9) == 0) {
    buzzerMuted = (buf[9] == '1');
    Serial.print(F("[CMD] Buzzer mute → ")); Serial.println(buzzerMuted);
  }
  // CMD:ALERT:WARNING or CMD:ALERT:CRITICAL
  else if (strncmp(buf, "CMD:ALERT:", 10) == 0) {
    if (strcmp(buf + 10, "WARNING") == 0)  { forceAlert = 1; forceUntil = millis() + 5000; }
    if (strcmp(buf + 10, "CRITICAL") == 0) { forceAlert = 2; forceUntil = millis() + 5000; }
    Serial.print(F("[CMD] Force alert → ")); Serial.println(buf + 10);
  }
}

class ServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* s) {
    Serial.println(F("[BLE] Arduino connected"));
  }
  void onDisconnect(BLEServer* s) {
    Serial.println(F("[BLE] Disconnected. Re-advertising..."));
    BLEDevice::getAdvertising()->start();
  }
};

float readTemp() {
  int raw = analogRead(PIN_TEMP);
  if (raw <= 0) return -99.0;
  float resistance = (4095.0 / raw - 1.0) * 10000.0;
  float tempK = 1.0 / (log(resistance / 10000.0) / 3950.0 + 1.0 / 298.15);
  return tempK - 273.15;
}

int readKnocks() {
  int count = 0;
  unsigned long start = millis();
  while (millis() - start < 500) {
    mqttClient.loop();
    if (digitalRead(PIN_KNOCK) == LOW) {
      count++;
      delay(50);
    }
  }
  return count;
}

void updateKnockStatus(int newKnocks) {
  unsigned long now = millis();
  if (newKnocks > 0) {
    if (knockAccum == 0) knockFirstMs = now;
    knockAccum += newKnocks;
    unsigned long elapsed = now - knockFirstMs;
    if (knockAccum >= KNOCK_CRIT_COUNT && elapsed <= KNOCK_CRIT_WINDOW) {
      knockStatus    = 2;
      knockHoldUntil = now + KNOCK_HOLD;
    } else if (knockAccum >= KNOCK_WARN_COUNT && elapsed <= KNOCK_WARN_WINDOW && knockStatus < 2) {
      knockStatus    = 1;
      knockHoldUntil = now + KNOCK_HOLD;
    }
    if (elapsed > KNOCK_CRIT_WINDOW) {
      knockAccum   = newKnocks;
      knockFirstMs = now;
    }
  }
  if (knockStatus > 0 && now > knockHoldUntil) {
    knockStatus = 0;
    knockAccum  = 0;
  }
}

void connectWiFi() {
  Serial.print(F("[WiFi] Connecting to Hello_World"));
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.print(F("\n[WiFi] Connected. IP: "));
  Serial.println(WiFi.localIP());
}

void connectMQTT() {
  mqttClient.setServer(brokerIP, 1883);
  mqttClient.setCallback(onMqttMessage);
  Serial.print(F("[MQTT] Connecting to Pi"));
  int attempts = 0;
  while (!mqttClient.connect("ESP32_GroupA6") && attempts < 5) {
    delay(1000);
    Serial.print(".");
    attempts++;
  }
  if (mqttClient.connected()) {
    mqttClient.subscribe(cmdTopic);
    Serial.println(F("\n[MQTT] Connected. Subscribed to cmd topic."));
  } else {
    Serial.println(F("\n[MQTT] Unavailable — continuing without MQTT."));
  }
}

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);
  pinMode(PIN_TEMP,  INPUT);
  pinMode(PIN_KNOCK, INPUT_PULLUP);

  connectWiFi();
  connectMQTT();

  BLEDevice::init("Group_A6");
  BLEServer* pServer = BLEDevice::createServer();
  pServer->setCallbacks(new ServerCallbacks());

  BLEService* pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY
  );
  pCharacteristic->addDescriptor(new BLE2902());
  pService->start();

  BLEDevice::getAdvertising()->addServiceUUID(SERVICE_UUID);
  BLEDevice::getAdvertising()->start();

  Serial.println(F("[BLE] Advertising as Group_A6"));
  Serial.println(F("[SYSTEM] Ready."));
}

void loop() {
  if (!mqttClient.connected()) connectMQTT();
  mqttClient.loop();

  float temp      = readTemp();
  int   newKnocks = readKnocks();

  updateKnockStatus(newKnocks);

  // Expire force alert
  if (forceAlert > 0 && millis() > forceUntil) forceAlert = 0;

  // Combine all sources — highest severity wins
  uint8_t tempStatus = 0;
  if (temp >= tempCritical) tempStatus = 2;
  else if (temp >= tempWarning) tempStatus = 1;

  uint8_t status = max((uint8_t)max(tempStatus, knockStatus), forceAlert);

  const char* statusStr = (status == 2) ? "CRITICAL" :
                          (status == 1) ? "WARNING"  : "NORMAL";

  char packet[80];
  snprintf(packet, sizeof(packet),
    "TEMP:%.1f;KNOCK:%d;STATUS:%s;MUTE:%d;WARN:%.1f;CRIT:%.1f",
    temp, knockAccum, statusStr, (int)buzzerMuted, tempWarning, tempCritical);
  Serial.println(packet);

  // BLE — send "M0"/"M1"/"M2" when muted, "0"/"1"/"2" when not
  // Arduino reads first char: 'M'=muted, digit=normal
  char ble[3];
  if (buzzerMuted) {
    ble[0] = 'M'; ble[1] = '0' + status; ble[2] = 0;
    pCharacteristic->setValue((uint8_t*)ble, 2);
  } else {
    ble[0] = '0' + status; ble[1] = 0;
    pCharacteristic->setValue((uint8_t*)ble, 1);
  }
  pCharacteristic->notify();

  mqttClient.publish(topic, packet);
}
