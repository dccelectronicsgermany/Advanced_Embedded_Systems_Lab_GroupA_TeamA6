// ─────────────────────────────────────────────
// TEST 3: KY-016 RGB LED Module
// Board  : Arduino Uno WiFi Rev2
// Pins   : R → 9 | G → 10 | B → 11 | – → GND
// ─────────────────────────────────────────────

#define PIN_RED   9
#define PIN_GREEN 10
#define PIN_BLUE  11

void setLED(bool r, bool g, bool b) {
  digitalWrite(PIN_RED,   r);
  digitalWrite(PIN_GREEN, g);
  digitalWrite(PIN_BLUE,  b);
}

void setup() {
  Serial.begin(9600);
  pinMode(PIN_RED,   OUTPUT);
  pinMode(PIN_GREEN, OUTPUT);
  pinMode(PIN_BLUE,  OUTPUT);
  Serial.println("KY-016 RGB LED Test");
  Serial.println("Cycling: Red → Green → Blue → Yellow → Off");
}

void loop() {
  Serial.println("RED");
  setLED(1, 0, 0);
  delay(1000);

  Serial.println("GREEN");
  setLED(0, 1, 0);
  delay(1000);

  Serial.println("BLUE");
  setLED(0, 0, 1);
  delay(1000);

  Serial.println("YELLOW (Red + Green)");
  setLED(1, 1, 0);
  delay(1000);

  Serial.println("WHITE (all on)");
  setLED(1, 1, 1);
  delay(1000);

  Serial.println("OFF");
  setLED(0, 0, 0);
  delay(1000);
}
