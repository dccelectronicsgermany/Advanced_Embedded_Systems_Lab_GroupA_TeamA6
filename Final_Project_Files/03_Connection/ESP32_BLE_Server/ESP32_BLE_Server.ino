// ─────────────────────────────────────────────
// Advertises a BLE characteristic that holds
// the current alert status as a single byte:
//   '0' = NORMAL   '1' = WARNING   '2' = CRITICAL
// Muted prefix: 'M0' / 'M1' / 'M2'
// Arduino (client) reads it and drives LED+buzzer
// Board: ESP32 DevKitC V4
// Library: ESP32 BLE Arduino (built-in)
// ─────────────────────────────────────────────

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define SERVICE_UUID        "19B10000-A8F2-537E-4F6C-D104768A1214"
#define CHARACTERISTIC_UUID "19B10001-A8F2-537E-4F6C-D104768A1214"

BLEServer*         pServer = nullptr;
BLECharacteristic* pChar   = nullptr;
bool               deviceConnected = false;

uint8_t fakeStatus  = 0;
bool    fakeMuted   = false;
unsigned long lastChange = 0;

class ServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* s)    { deviceConnected = true;  Serial.println("Client connected"); }
  void onDisconnect(BLEServer* s) { deviceConnected = false; Serial.println("Client disconnected");
                                    s->startAdvertising(); }
};

void setup() {
  Serial.begin(115200);
  BLEDevice::init("ESP32_SensorNode");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new ServerCallbacks());

  BLEService* pService = pServer->createService(SERVICE_UUID);
  pChar = pService->createCharacteristic(
    CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY
  );
  pChar->addDescriptor(new BLE2902());
  pService->start();

  BLEAdvertising* pAdv = BLEDevice::getAdvertising();
  pAdv->addServiceUUID(SERVICE_UUID);
  pAdv->setScanResponse(true);
  BLEDevice::startAdvertising();

  Serial.println("BLE Server advertising — waiting for Arduino client");
  Serial.println("Status cycles NORMAL → WARNING → CRITICAL every 5s");
  Serial.println("Send 'm' over Serial to toggle mute");
}

void updateCharacteristic() {
  char buf[4] = {0};
  if (fakeMuted) {
    buf[0] = 'M'; buf[1] = '0' + fakeStatus;
    pChar->setValue((uint8_t*)buf, 2);
  } else {
    buf[0] = '0' + fakeStatus;
    pChar->setValue((uint8_t*)buf, 1);
  }
  pChar->notify();
  const char* st = fakeStatus == 2 ? "CRITICAL" : fakeStatus == 1 ? "WARNING" : "NORMAL";
  Serial.printf("Published: status=%s muted=%s\n", st, fakeMuted ? "yes" : "no");
}

void loop() {
  if (millis() - lastChange > 5000) {
    fakeStatus = (fakeStatus + 1) % 3;
    lastChange = millis();
    if (deviceConnected) updateCharacteristic();
  }
  if (Serial.available()) {
    char c = Serial.read();
    if (c == 'm' || c == 'M') {
      fakeMuted = !fakeMuted;
      Serial.printf("Mute toggled: %s\n", fakeMuted ? "ON" : "OFF");
      if (deviceConnected) updateCharacteristic();
    }
  }
}
