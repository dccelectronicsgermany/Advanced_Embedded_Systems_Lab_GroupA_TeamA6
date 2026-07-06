// ─────────────────────────────────────────────
// Tests the scan → connect → disconnect → rescan
// loop in isolation before adding actuator logic.
// Prints connection state transitions to Serial.
// Board  : Arduino Uno WiFi Rev2
// Library: ArduinoBLE
// ─────────────────────────────────────────────

#include <ArduinoBLE.h>

#define SERVICE_UUID        "19B10000-A8F2-537E-4F6C-D104768A1214"
#define CHARACTERISTIC_UUID "19B10001-A8F2-537E-4F6C-D104768A1214"
#define TARGET_NAME         "ESP32_SensorNode"

BLEDevice         peripheral;
BLECharacteristic sensorChar;
bool connected  = false;
int  attemptNum = 0;

bool scanAndConnect() {
  attemptNum++;
  Serial.printf("Scan attempt #%d — looking for '%s'...\n", attemptNum, TARGET_NAME);

  BLE.scanForUuid(SERVICE_UUID);
  unsigned long start = millis();

  while (millis() - start < 8000) {
    peripheral = BLE.available();
    if (peripheral && peripheral.localName() == TARGET_NAME) {
      BLE.stopScan();
      Serial.printf("Found %s at %s\n", TARGET_NAME, peripheral.address().c_str());

      if (!peripheral.connect())          { Serial.println("connect() failed");           return false; }
      if (!peripheral.discoverAttributes()){ Serial.println("discoverAttributes() failed");
                                             peripheral.disconnect();                      return false; }

      sensorChar = peripheral.characteristic(CHARACTERISTIC_UUID);
      if (!sensorChar) {
        Serial.println("Characteristic not found");
        peripheral.disconnect();
        return false;
      }

      if (sensorChar.canSubscribe()) {
        sensorChar.subscribe();
        Serial.println("Subscribed to notifications");
      } else {
        Serial.println("Notifications not supported — will poll");
      }

      Serial.println("Connected successfully");
      return true;
    }
  }
  BLE.stopScan();
  Serial.println("Device not found in scan window");
  return false;
}

void setup() {
  Serial.begin(9600);
  while (!Serial);
  if (!BLE.begin()) {
    Serial.println("BLE init failed — check ArduinoBLE library");
    while (true);
  }
  Serial.println("Arduino BLE Reconnect Test");
  Serial.println("scan → connect → read → detect disconnect → rescan");
}

void loop() {
  if (!connected) {
    connected = scanAndConnect();
    if (!connected) { Serial.println("Retrying in 3 seconds..."); delay(3000); }
    return;
  }

  if (!peripheral.connected()) {
    Serial.println("Connection lost — restarting scan");
    connected = false;
    return;
  }

  if (sensorChar.valueUpdated() || sensorChar.canRead()) {
    int len = sensorChar.valueLength();
    if (len > 0) {
      char buf[8] = {0};
      memcpy(buf, sensorChar.value(), min(len, 7));
      Serial.printf("BLE value: [%s] (len=%d)\n", buf, len);
    }
  }

  BLE.poll();
  delay(500);
}
