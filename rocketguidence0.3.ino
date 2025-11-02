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
float gx, gy, gz;
float roll = 0;
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
  display.println(F("✅ IMU Ready (Gyro X)"));
  display.display();

  delay(1000);
  lastTime = millis();
}

void loop() {
  // --- Read gyro data ---
  if (IMU.gyroscopeAvailable()) {
    IMU.readGyroscope(gx, gy, gz);
  }

  // --- Time delta ---
  unsigned long currentTime = millis();
  float dt = (currentTime - lastTime) / 1000.0;
  lastTime = currentTime;

  // --- Integrate gyro X to get roll angle ---
  roll += gx * dt; // gx in deg/s -> roll in degrees

  // --- Normalize roll angle ---
  if (roll > 180) roll -= 360;
  if (roll < -180) roll += 360;

  // --- Control logic ---
  if (roll > 25) {
    // Tilt right
    digitalWrite(BUZZER_MOSFET, HIGH);
    multiServo.write(135);
  } 
  else if (roll < -25) {
    // Tilt left
    digitalWrite(BUZZER_MOSFET, HIGH);
    multiServo.write(45);
  } 
  else {
    // Neutral
    digitalWrite(BUZZER_MOSFET, LOW);
    multiServo.write(90);
  }

  // --- OLED display ---
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(0, 0);
  display.print(F("Gyro X: "));
  display.println(gx, 1);

  display.setCursor(0, 16);
  display.print(F("Roll: "));
  display.print(roll, 1);
  display.println(" deg");

  display.setCursor(0, 32);
  display.print(F("Servo: "));
  display.print(roll > 25 ? "Right" : roll < -25 ? "Left" : "Center");

  display.display();

  // --- Serial monitor output (optional) ---
  Serial.print("Gyro X: "); Serial.print(gx, 2);
  Serial.print("  Roll: "); Serial.println(roll, 2);

  delay(100);
}
