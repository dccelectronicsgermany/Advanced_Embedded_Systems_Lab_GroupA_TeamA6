// ─────────────────────────────────────────────
// Controls KY-016 (RGB LED) + KY-012 (Buzzer)
// Reads status from Serial (for standalone test)
// Board: Arduino Uno WiFi Rev2
// ─────────────────────────────────────────────

#define PIN_RED   9
#define PIN_GREEN 10
#define PIN_BLUE  11
#define PIN_BUZZ  6

// Current state
uint8_t currentStatus = 0;  // 0=NORMAL, 1=WARNING, 2=CRITICAL
bool    buzzerMuted   = false;

void setLED(bool r, bool g, bool b) {
  digitalWrite(PIN_RED,   r);
  digitalWrite(PIN_GREEN, g);
  digitalWrite(PIN_BLUE,  b);
}

void applyStatus(uint8_t status) {
  if (status == 0) {
    setLED(0, 1, 0);
    noTone(PIN_BUZZ);
  } else if (status == 1) {
    setLED(1, 1, 0);
    if (!buzzerMuted) { tone(PIN_BUZZ, 1000, 300); }
  } else if (status == 2) {
    setLED(1, 0, 0);
    if (!buzzerMuted) { tone(PIN_BUZZ, 2000, 800); }
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(PIN_RED,   OUTPUT);
  pinMode(PIN_GREEN, OUTPUT);
  pinMode(PIN_BLUE,  OUTPUT);
  pinMode(PIN_BUZZ,  OUTPUT);

  setLED(0, 1, 0);
  Serial.println("Arduino Actuator Integration Test");
  Serial.println("Send: 0=NORMAL  1=WARNING  2=CRITICAL  M=toggle mute");
  Serial.println("Starting in NORMAL (Green LED, buzzer off)");
}

void loop() {
  // Accept single-char command over Serial for standalone test
  if (Serial.available()) {
    char c = Serial.read();
    if (c == '0') {
      currentStatus = 0;
      Serial.println("→ NORMAL (Green LED)");
    } else if (c == '1') {
      currentStatus = 1;
      Serial.println("→ WARNING (Yellow LED)");
    } else if (c == '2') {
      currentStatus = 2;
      Serial.println("→ CRITICAL (Red LED)");
    } else if (c == 'M' || c == 'm') {
      buzzerMuted = !buzzerMuted;
      Serial.print("→ Mute: "); Serial.println(buzzerMuted ? "ON" : "OFF");
    }
    applyStatus(currentStatus);
  }

  // Blink LED for WARNING to make it visually distinct
  static unsigned long lastBlink = 0;
  static bool blinkState = false;
  if (currentStatus == 1 && millis() - lastBlink > 500) {
    blinkState = !blinkState;
    if (blinkState) setLED(1, 1, 0);
    else            setLED(0, 0, 0);
    lastBlink = millis();
  }
}
