// ─────────────────────────────────────────────
// TEST 2: KY-031 Knock / Vibration Sensor
// Board  : ESP32 DevKitC V4
// Pin    : GPIO32
// Wiring : S → GPIO32 | + → 3.3V | – → GND
// Mode   : INPUT_PULLUP — signal goes LOW on knock
// ─────────────────────────────────────────────

#define PIN_KNOCK 32

int  knockCount = 0;
bool lastState  = HIGH;

void setup() {
  Serial.begin(115200);
  pinMode(PIN_KNOCK, INPUT_PULLUP);
  Serial.println("KY-031 Knock Sensor Test");
  Serial.println("Knock or tap the sensor — count appears below");
}

void loop() {
  bool state = digitalRead(PIN_KNOCK);

  // Detect falling edge (HIGH → LOW = knock)
  if (lastState == HIGH && state == LOW) {
    knockCount++;
    Serial.print("KNOCK detected! Total: ");
    Serial.println(knockCount);
    delay(50);  // debounce
  }

  lastState = state;

  // Print idle status every 3 seconds
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint > 3000) {
    Serial.print("Waiting... Total knocks so far: ");
    Serial.println(knockCount);
    lastPrint = millis();
  }
}
