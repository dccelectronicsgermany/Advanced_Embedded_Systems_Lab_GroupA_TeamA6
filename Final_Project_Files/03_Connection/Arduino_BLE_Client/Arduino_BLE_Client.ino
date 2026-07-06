// ─────────────────────────────────────────────
// Connects to ESP32 BLE server, reads alert status
// and drives KY-016 RGB LED + KY-012 Buzzer.
// Run reconnect test (Step 3b) first.
// Board  : Arduino Uno WiFi Rev2
// Library: ArduinoBLE
// ─────────────────────────────────────────────

#include <ArduinoBLE.h>

#define PIN_RED   9
#define PIN_GREEN 10
#define PIN_BLUE  11
#define PIN_BUZZ  6

#define SERVICE_UUID        "19B10000-A8F2-537E-4F6C-D104768A1214"
#define CHARACTERISTIC_UUID "19B10001-A8F2-537E-4F6C-D104768A1214"

BLEDevice         peripheral;
BLECharacteristic sensorChar;
bool connected = false;

void setLED(bool r, bool g, bool b) {
  digitalWrite(PIN_RED, r); digitalWrite(PIN_GREEN, g); digitalWrite(PIN_BLUE, b);
}

void applyBLEValue(const uint8_t* data, int len) {
  if (len == 0) return;
  char buf[4] = {0};
  memcpy(buf, data, min(len, 3));
  bool muted  = (buf[0] == 'M');
  char status = muted ? buf[1] : buf[0];
  if (status == '0') {
    setLED(0, 1, 0); noTone(PIN_BUZZ);
    Serial.println("Status: NORMAL");
  } else if (status == '1') {
    setLED(1, 1, 0);
    if (!muted) tone(PIN_BUZZ, 1000, 300);
    Serial.println(muted ? "Status: WARNING (muted)" : "Status: WARNING");
  } else if (status == '2') {
    setLED(1, 0, 0);
    if (!muted) tone(PIN_BUZZ, 2000, 800);
    Serial.println(muted ? "Status: CRITICAL (muted)" : "Status: CRITICAL");
  }
}

bool connectToESP32() {
  BLE.scanForUuid(SERVICE_UUID);
  unsigned long start = millis();
  while (millis() - start < 10000) {
    peripheral = BLE.available();
    if (peripheral && peripheral.localName() == "ESP32_SensorNode") {
      BLE.stopScan();
      if (!peripheral.connect())           return false;
      if (!peripheral.discoverAttributes()){ peripheral.disconnect(); return false; }
      sensorChar = peripheral.characteristic(CHARACTERISTIC_UUID);
      if (!sensorChar || !sensorChar.canRead()) { peripheral.disconnect(); return false; }
      sensorChar.subscribe();
      Serial.println("Connected to ESP32");
      return true;
    }
  }
  BLE.stopScan();
  return false;
}

void setup() {
  Serial.begin(9600);
  pinMode(PIN_RED, OUTPUT); pinMode(PIN_GREEN, OUTPUT);
  pinMode(PIN_BLUE, OUTPUT); pinMode(PIN_BUZZ, OUTPUT);
  setLED(0, 0, 1);  // Blue = searching
  if (!BLE.begin()) { Serial.println("BLE init failed"); while (true); }
  Serial.println("Arduino BLE Client — scanning for ESP32_SensorNode...");
}

void loop() {
  if (!connected) {
    setLED(0, 0, 1);
    connected = connectToESP32();
    return;
  }
  if (!peripheral.connected()) { connected = false; return; }
  if (sensorChar.valueUpdated())
    applyBLEValue(sensorChar.value(), sensorChar.valueLength());
  BLE.poll();
}
