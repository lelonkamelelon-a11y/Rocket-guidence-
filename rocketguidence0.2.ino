#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino_LSM6DSOX.h>
#include <Servo.h>
#include <math.h>

// --- OLED setup ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// --- Pin definitions ---
#define SERVO_PIN 9       // multi servo
#define BUZZER_MOSFET 8   // buzer mosfet

// --- Servo object ---
Servo multiServo;

// --- IMU data ---
float ax, ay, az;
float roll, pitch, yaw = 0;
unsigned long lastTime = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);

  // --- Setup pins ---
  pinMode(BUZZER_MOSFET, OUTPUT);
  digitalWrite(BUZZER_MOSFET, LOW);

  // --- Attach servo ---
  multiServo.attach(SERVO_PIN);
  multiServo.write(90); // neutral

  // --- OLED init ---
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("❌ OLED not found!"));
    while (true);
  }

  // --- IMU init ---
  if (!IMU.begin()) {
    Serial.println(F("❌ IMU not detected!"));
    while (true);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(F("✅ IMU Ready"));
  display.display();

  delay(1000);
  lastTime = millis();
}

void loop() {
  // --- Read IMU acceleration ---
  if (IMU.accelerationAvailable()) {
    IMU.readAcceleration(ax, ay, az);
  }

  // --- Calculate roll angle (tilt) ---
  roll = atan2(ay, sqrt(ax * ax + az * az)) * 180.0 / PI;

  // --- Control logic ---
  if (roll > 25) {
    // Tilt to the right
    digitalWrite(BUZZER_MOSFET, HIGH);
    multiServo.write(135);
  } 
  else if (roll < -25) {
    // Tilt to the left
    digitalWrite(BUZZER_MOSFET, HIGH);
    multiServo.write(45);
  } 
  else {
    // Neutral position
    digitalWrite(BUZZER_MOSFET, LOW);
    multiServo.write(90);
  }

  // --- OLED display ---
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print(F("Roll: "));
  display.print(roll, 1);
  display.println(" deg");
  display.setCursor(0, 16);
  display.print(F("Servo: "));
  display.print(roll > 25 ? "Affirmative" : roll < -25 ? "Negative" : "Neutral");
  display.display();

  delay(200);
}
