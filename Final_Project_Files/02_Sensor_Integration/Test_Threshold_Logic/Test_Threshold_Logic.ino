// ─────────────────────────────────────────────
// Tests the combined NORMAL/WARNING/CRITICAL
// decision logic for temperature and knock events
// without needing any sensors connected.
// Enter test values over Serial Monitor.
// Board: Any (ESP32 or Arduino)
// ─────────────────────────────────────────────

float tempWarning  = 25.0;
float tempCritical = 30.0;

int           knockAccum     = 0;
unsigned long knockFirstMs   = 0;
uint8_t       knockStatus    = 0;
unsigned long knockHoldUntil = 0;
const unsigned long KNOCK_HOLD = 5000;

uint8_t evalTempStatus(float temp) {
  if (temp >= tempCritical) return 2;
  if (temp >= tempWarning)  return 1;
  return 0;
}

void recordKnock() {
  unsigned long now = millis();
  if (knockAccum == 0) knockFirstMs = now;
  knockAccum++;
  unsigned long elapsed = now - knockFirstMs;
  if (knockAccum >= 5 && elapsed <= 5000) {
    knockStatus    = 2;
    knockHoldUntil = now + KNOCK_HOLD;
  } else if (knockAccum >= 2 && elapsed <= 2000 && knockStatus < 2) {
    knockStatus    = 1;
    knockHoldUntil = now + KNOCK_HOLD;
  }
  if (elapsed > 5000) { knockAccum = 1; knockFirstMs = now; }
}

void decayKnockStatus() {
  if (knockStatus > 0 && millis() > knockHoldUntil) {
    knockStatus = 0;
    knockAccum  = 0;
    Serial.println("[knock status decayed → NORMAL]");
  }
}

void printStatus(float temp) {
  decayKnockStatus();
  uint8_t ts = evalTempStatus(temp);
  uint8_t combined = max(ts, knockStatus);
  const char* s = combined == 2 ? "CRITICAL" : combined == 1 ? "WARNING" : "NORMAL";
  Serial.printf("TEMP:%.1f  KNOCK_ACCUM:%d  COMBINED:%s  (warn=%.1f crit=%.1f)\n",
    temp, knockAccum, s, tempWarning, tempCritical);
}

float currentTemp = 22.0;

void printHelp() {
  Serial.println("Commands:");
  Serial.println("  t<value>  — set temperature,        e.g. t27.5");
  Serial.println("  k         — simulate a knock");
  Serial.println("  w<value>  — set warning threshold,  e.g. w26");
  Serial.println("  c<value>  — set critical threshold, e.g. c31");
  Serial.println("  s         — print current status");
}

void setup() {
  Serial.begin(115200);
  Serial.println("Threshold Logic Test");
  printHelp();
}

void loop() {
  decayKnockStatus();

  if (Serial.available()) {
    String line = Serial.readStringUntil('\n');
    line.trim();
    if (line.length() == 0) return;

    char  cmd = line.charAt(0);
    float val = line.substring(1).toFloat();

    if (cmd == 't') {
      currentTemp = val;
      Serial.printf("Temperature set to %.1f°C\n", currentTemp);
    } else if (cmd == 'k') {
      recordKnock();
      Serial.printf("Knock recorded (total in window: %d)\n", knockAccum);
    } else if (cmd == 'w') {
      if (val >= 15.0 && val < tempCritical) {
        tempWarning = val;
        Serial.printf("Warning threshold → %.1f°C\n", tempWarning);
      } else {
        Serial.println("Invalid: must be >= 15 and < critical threshold");
      }
    } else if (cmd == 'c') {
      if (val >= 15.0 && val <= 60.0) {
        tempCritical = val;
        if (tempWarning >= tempCritical) {
          tempWarning = tempCritical - 1.0;
          Serial.printf("Warning auto-adjusted to %.1f°C\n", tempWarning);
        }
        Serial.printf("Critical threshold → %.1f°C\n", tempCritical);
      } else {
        Serial.println("Invalid: must be between 15 and 60");
      }
    } else if (cmd != 's' && cmd != 'S') {
      printHelp();
      return;
    }
    printStatus(currentTemp);
  }

  static unsigned long lastPrint = 0;
  if (millis() - lastPrint > 3000) {
    lastPrint = millis();
    printStatus(currentTemp);
  }
}
