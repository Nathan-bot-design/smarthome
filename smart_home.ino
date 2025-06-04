#include <math.h>

// Pin Definitions
const int TEMP_PIN = A3;
const int LDR_PIN = A0;
const int POT1_PIN = A1;
const int POT2_PIN = A2;
const int BUTTON1_PIN = A4;
const int BUTTON2_PIN = A5;

const int LED_RED = 13;
const int LED_YELLOW = 12;
const int LED_GREEN = 11;
const int BUZZER_PIN = 9;

// Thermistor Constants
const float SERIES_RESISTOR = 10000.0;
const float NOMINAL_RESISTANCE = 10000.0;
const float NOMINAL_TEMPERATURE = 25.0;
const float BETA_COEFFICIENT = 3950.0;
const float ADC_MAX = 1023.0;
const float VCC = 5.0;

// Thresholds
const float TEMP_HIGH = 37.0;  // Celsius
const int LIGHT_LOW = 200;     // LDR threshold 
const int FIRE_LDR = 100;      // Fire-like intensity

unsigned long lastPrint = 0;

void setup() {
  Serial.begin(9600);

  pinMode(LED_RED, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  pinMode(BUTTON1_PIN, INPUT_PULLUP);
  pinMode(BUTTON2_PIN, INPUT_PULLUP);

  clearScreen();
  Serial.println("🌐 Smart Home System Booted...");
  delay(2000);
}

void loop() {
  float temperature = readTemperature();
  int ldrValue = analogRead(LDR_PIN);
  int pot1 = analogRead(POT1_PIN);
  int pot2 = analogRead(POT2_PIN);
  bool button1 = digitalRead(BUTTON1_PIN) == LOW;
  bool button2 = digitalRead(BUTTON2_PIN) == LOW;

  // LED Logic
  digitalWrite(LED_GREEN, temperature < 30);
  digitalWrite(LED_YELLOW, temperature >= 30 && temperature < TEMP_HIGH);
  digitalWrite(LED_RED, temperature >= TEMP_HIGH);

  // Buzzer logic
  if (temperature >= TEMP_HIGH) {
    tone(BUZZER_PIN, 1000);  // 1kHz tone
    delay(500);              // Play for 500ms
    noTone(BUZZER_PIN);
    delay(500);
    tone(BUZZER_PIN, 1000);  // 1kHz tone
    delay(500);              // Play for 500ms
    noTone(BUZZER_PIN);
    delay(500);
    tone(BUZZER_PIN, 1000);  // 1kHz tone
    delay(500);              // Play for 500ms
    noTone(BUZZER_PIN);
    delay(500);
  } else {
    noTone(BUZZER_PIN);
  }

  // Serial UI - Update every second
  if (millis() - lastPrint >= 1000) {
    clearScreen();
    printSmartUI(temperature, ldrValue, pot1, pot2, button1, button2);
    lastPrint = millis();
  }
}

float readTemperature() {
  int analogValue = analogRead(TEMP_PIN);
  float voltage = analogValue * VCC / ADC_MAX;
  float resistance = (voltage * SERIES_RESISTOR) / (VCC - voltage);

  float steinhart;
  steinhart = resistance / NOMINAL_RESISTANCE;
  steinhart = log(steinhart);
  steinhart /= BETA_COEFFICIENT;
  steinhart += 1.0 / (NOMINAL_TEMPERATURE + 273.15);
  steinhart = 1.0 / steinhart;
  steinhart -= 273.15;

  return steinhart;
}

void clearScreen() {
  Serial.write(27);     // ESC character
  Serial.print("[2J");  // Clear screen
  Serial.write(27);
  Serial.print("[H");  // Cursor to home
}

void printSmartUI(float temp, int light, int pot1, int pot2, bool b1, bool b2) {
  Serial.println("🏠 ========== SMART HOME DASHBOARD ==========");
  Serial.print("🌡️  Temp: ");
  Serial.print(temp, 1);
  Serial.println(" °C");

  Serial.print("💡 Light: ");
  Serial.print(light);
  if (light < FIRE_LDR) Serial.println(" 🔥 FIRE DETECTED!");
  else if (light < LIGHT_LOW) Serial.println(" 🌙 Night");
  else Serial.println(" ☀️ Day");

  Serial.print("🎚️ Pot1: ");
  Serial.println(pot1);

  Serial.print("🎛️ Pot2: ");
  Serial.println(pot2);

  Serial.print("🔘 Button 1: ");
  Serial.println(b1 ? "Pressed" : "Released");

  Serial.print("🔘 Button 2: ");
  Serial.println(b2 ? "Pressed" : "Released");

  Serial.print("🔊 Alarm: ");
  if (temp >= TEMP_HIGH || light <= FIRE_LDR) Serial.println("ACTIVE 🚨");
  else Serial.println("OFF");

  Serial.println("💡 LED Status:");
  Serial.print("   🟥 RED: ");
  Serial.println(digitalRead(LED_RED) ? "ON" : "OFF");
  Serial.print("   🟨 YELLOW: ");
  Serial.println(digitalRead(LED_YELLOW) ? "ON" : "OFF");
  Serial.print("   🟩 GREEN: ");
  Serial.println(digitalRead(LED_GREEN) ? "ON" : "OFF");

  Serial.println("==========================================");
}
