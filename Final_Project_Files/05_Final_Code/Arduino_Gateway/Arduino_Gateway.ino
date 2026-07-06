#include <ArduinoBLE.h>

#define SERVICE_UUID        "19B10000-A8F2-537E-4F6C-D104768A1214"
#define CHARACTERISTIC_UUID "19B10001-A8F2-537E-4F6C-D104768A1214"

#define PIN_RED   9
#define PIN_GREEN 10
#define PIN_BLUE  11
#define PIN_BUZZ  6

void setLED(bool r, bool g, bool b) {
  digitalWrite(PIN_RED,   r);
  digitalWrite(PIN_GREEN, g);
  digitalWrite(PIN_BLUE,  b);
}

bool doConnect(BLEDevice p) {
  if (!p.connect())            return false;
  if (!p.discoverAttributes()) { p.disconnect(); return false; }
  BLECharacteristic c = p.characteristic(CHARACTERISTIC_UUID);
  if (!c || !c.canSubscribe()) { p.disconnect(); return false; }
  if (!c.subscribe())          { p.disconnect(); return false; }
  return true;
}

BLEDevice     sensorNode;
BLECharacteristic sensorChar;
bool bleConnected = false;

void setup() {
  pinMode(PIN_RED,   OUTPUT);
  pinMode(PIN_GREEN, OUTPUT);
  pinMode(PIN_BLUE,  OUTPUT);
  pinMode(PIN_BUZZ,  OUTPUT);
  setLED(0, 0, 1);  // Blue = booting

  BLE.begin();
  BLE.scanForUuid(SERVICE_UUID);
}

void loop() {
  if (!bleConnected) {
    BLEDevice p = BLE.available();
    if (p) {
      BLE.stopScan();
      if (p.connect() && p.discoverAttributes()) {
        BLECharacteristic c = p.characteristic(CHARACTERISTIC_UUID);
        if (c && c.canSubscribe() && c.subscribe()) {
          sensorNode = p;
          sensorChar = c;
          bleConnected = true;
          setLED(0, 1, 0);  // Green = connected
        } else {
          p.disconnect();
          BLE.scanForUuid(SERVICE_UUID);
        }
      } else {
        BLE.scanForUuid(SERVICE_UUID);
      }
    }
    return;
  }

  if (!sensorNode.connected()) {
    bleConnected = false;
    setLED(0, 0, 0);
    BLE.scanForUuid(SERVICE_UUID);
    return;
  }

  if (sensorChar.valueUpdated()) {
    char buf[4] = {0};
    int len = min((int)sensorChar.valueLength(), 3);
    memcpy(buf, sensorChar.value(), len);

    // 'M' prefix means buzzer muted — LED still changes, buzzer silent
    bool muted = (buf[0] == 'M');
    char status = muted ? buf[1] : buf[0];

    if (status == '0') {
      setLED(0, 1, 0);
      noTone(PIN_BUZZ);
    } else if (status == '1') {
      setLED(1, 1, 0);
      if (!muted) tone(PIN_BUZZ, 1000, 300);
    } else if (status == '2') {
      setLED(1, 0, 0);
      if (!muted) tone(PIN_BUZZ, 2000, 800);
    }
  }
}
