// ─────────────────────────────────────────────
// TEST 4: KY-012 Active Buzzer
// Board  : Arduino Uno WiFi Rev2
// Pin    : S → 6 | – → GND
// Note   : Active buzzer — HIGH = on, LOW = off
//          tone() sets frequency, noTone() stops
// ─────────────────────────────────────────────

#define PIN_BUZZ 6

void setup() {
  Serial.begin(9600);
  pinMode(PIN_BUZZ, OUTPUT);
  Serial.println("KY-012 Buzzer Test");
  Serial.println("Pattern: Normal beep → Warning beep → Critical beep → Silent");
}

void loop() {
  // NORMAL pattern — single short beep
  Serial.println("NORMAL beep (1kHz, 200ms)");
  tone(PIN_BUZZ, 1000, 200);
  delay(800);

  // WARNING pattern — two quick beeps
  Serial.println("WARNING beep (1kHz, 300ms x2)");
  tone(PIN_BUZZ, 1000, 300);
  delay(400);
  tone(PIN_BUZZ, 1000, 300);
  delay(700);

  // CRITICAL pattern — long high beep
  Serial.println("CRITICAL beep (2kHz, 800ms)");
  tone(PIN_BUZZ, 2000, 800);
  delay(1200);

  // Silent
  Serial.println("Silent (2 seconds)");
  noTone(PIN_BUZZ);
  delay(2000);
}
