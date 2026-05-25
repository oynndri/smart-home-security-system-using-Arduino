#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

// LCD setup (0x27 is standard; change to 0x3F if text doesn't show)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Servo Setup
Servo safetyVentServo;
const int servoPin = 9; // Connected to Servo Signal Line

// Pin setup
const int smokePin = A0;
const int gasPin = A1;
const int buzzerPin = 7;

// Thresholds (Adjust these based on your room's environmental baseline)
const int smokeThreshold = 300;
const int gasThreshold = 400;

// Set to true if you are using a passive buzzer that requires a frequency tone
bool isPassiveBuzzer = false; 

void setup() {
  // Initialize the I2C LCD screen correctly
  lcd.init();
  lcd.backlight();

  // Attach Servo
  safetyVentServo.attach(servoPin);
  safetyVentServo.write(0); // Start with vent closed

  // Initialize outputs
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);

  // Boot screen greeting sequence
  lcd.setCursor(0, 0);
  lcd.print("  SMART SECURITY ");
  lcd.setCursor(0, 1);
  lcd.print("   INITIALIZING ");
  delay(2000);
  lcd.clear();
}

void loop() {
  int smokeValue = analogRead(smokePin);
  int gasValue = analogRead(gasPin);

  // Check if any hazard threshold is breached
  if (smokeValue > smokeThreshold || gasValue > gasThreshold) {
    
    // --- HAZARD STATE ACTIVED ---
    // 1. Sound the Buzzer Alarm
    if (isPassiveBuzzer) {
      tone(buzzerPin, 1000);  // Send a 1 kHz audio wave tone
    } else {
      digitalWrite(buzzerPin, HIGH);  // Direct power to active buzzer
    }

    // 2. Open the Safety Vent / Emergency Door
    safetyVentServo.write(90); 

    // 3. Update the Display with Alert Text
    lcd.setCursor(0, 0);
    lcd.print("!! HAZARD ALERT !!");
    lcd.setCursor(0, 1);
    lcd.print("S:"); 
    lcd.print(smokeValue);
    lcd.print("  G:");
    lcd.print(gasValue);
    lcd.print("     "); // Blank spaces clear old artifacts without flickering

  } else {
    
    // --- SAFE STATE ACTIVE ---
    // 1. Deactivate Alarms
    if (isPassiveBuzzer) {
      noTone(buzzerPin);
    } else {
      digitalWrite(buzzerPin, LOW);
    }

    // 2. Keep the Safety Vent closed 
    safetyVentServo.write(0); 

    // 3. Update the Display with normal telemetry
    lcd.setCursor(0, 0);
    lcd.print("SYSTEM: SECURE  ");
    lcd.setCursor(0, 1);
    lcd.print("S:"); 
    lcd.print(smokeValue);
    lcd.print("   G:");
    lcd.print(gasValue);
    lcd.print("     "); 
  }

  delay(500); // 500ms delay offers responsive alerting without over-polling
}
