// ─────────────────────────────────────────────
// Reads KY-013 (temperature) + KY-031 (knock)
// Evaluates NORMAL / WARNING / CRITICAL status
// No BLE or WiFi — pure sensor reading test
// Board: ESP32 DevKitC V4
// ─────────────────────────────────────────────

#define PIN_TEMP  34   // KY-013 signal → GPIO34 (VP)
#define PIN_KNOCK 32   // KY-031 signal → GPIO32

// Thresholds
#define TEMP_WARNING  25.0
#define TEMP_CRITICAL 30.0
#define KNOCK_WARN    2     // knocks in window to trigger WARNING
#define KNOCK_CRIT    5     // knocks in window to trigger CRITICAL
#define KNOCK_HOLD    5000  // ms to hold knock status

int           knockAccum     = 0;
unsigned long knockFirstMs   = 0;
uint8_t       knockStatus    = 0;
unsigned long knockHoldUntil = 0;

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);
  pinMode(PIN_TEMP,  INPUT);
  pinMode(PIN_KNOCK, INPUT_PULLUP);
  Serial.println("ESP32 Sensor Integration Test");
  Serial.println("Format: TEMP:xx.x;KNOCK:n;STATUS:...");
}

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
    if (knockAccum >= KNOCK_CRIT && elapsed <= 5000) {
      knockStatus    = 2;
      knockHoldUntil = now + KNOCK_HOLD;
    } else if (knockAccum >= KNOCK_WARN && elapsed <= 2000 && knockStatus < 2) {
      knockStatus    = 1;
      knockHoldUntil = now + KNOCK_HOLD;
    }
    if (elapsed > 5000) { knockAccum = newKnocks; knockFirstMs = now; }
  }
  if (knockStatus > 0 && now > knockHoldUntil) {
    knockStatus = 0;
    knockAccum  = 0;
  }
}

void loop() {
  float temp      = readTemp();
  int   newKnocks = readKnocks();
  updateKnockStatus(newKnocks);

  uint8_t tempStatus = 0;
  if (temp >= TEMP_CRITICAL)      tempStatus = 2;
  else if (temp >= TEMP_WARNING)  tempStatus = 1;

  uint8_t status = max(tempStatus, knockStatus);
  const char* statusStr = (status == 2) ? "CRITICAL" :
                          (status == 1) ? "WARNING"  : "NORMAL";

  char packet[64];
  snprintf(packet, sizeof(packet),
    "TEMP:%.1f;KNOCK:%d;STATUS:%s", temp, knockAccum, statusStr);
  Serial.println(packet);
}
