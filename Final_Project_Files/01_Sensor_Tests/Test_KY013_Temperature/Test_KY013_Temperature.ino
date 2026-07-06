// ─────────────────────────────────────────────
// TEST 1: KY-013 NTC Thermistor — Temperature Sensor
// Board  : ESP32 DevKitC V4
// Pin    : GPIO34 (VP)
// Wiring : S → GPIO34 | + → 3.3V | – → GND
// ─────────────────────────────────────────────

#define PIN_TEMP 34

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);  // ESP32 12-bit ADC (0–4095)
  pinMode(PIN_TEMP, INPUT);
  Serial.println("KY-013 Temperature Sensor Test");
  Serial.println("Expected: room temperature ~20–26°C");
}

void loop() {
  int raw = analogRead(PIN_TEMP);

  // Steinhart-Hart equation for NTC thermistor
  float resistance = (4095.0 / raw - 1.0) * 10000.0;
  float tempK = 1.0 / (log(resistance / 10000.0) / 3950.0 + 1.0 / 298.15);
  float tempC = tempK - 273.15;

  Serial.print("Raw ADC : "); Serial.println(raw);
  Serial.print("Temp    : "); Serial.print(tempC, 1); Serial.println(" °C");

  if (tempC < 15.0 || tempC > 50.0) {
    Serial.println("WARNING: Reading out of expected range — check wiring");
  } else {
    Serial.println("Status  : OK");
  }

  Serial.println("---");
  delay(1000);
}
