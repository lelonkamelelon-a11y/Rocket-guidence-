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
#define MOSFET1 9
#define MOSFET2 10
#define SERVO1 7
#define SERVO2 8

// --- Servo objects ---
Servo servo1;
Servo servo2;

// --- IMU data ---
float ax, ay, az;
float roll, pitch, yaw = 0;
unsigned long lastTime = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);

  // --- Setup pins ---
  pinMode(MOSFET1, OUTPUT);
  pinMode(MOSFET2, OUTPUT);
  digitalWrite(MOSFET1, LOW);
  digitalWrite(MOSFET2, LOW);

  // --- Attach servos ---
  servo1.attach(SERVO1);
  servo2.attach(SERVO2);
  servo1.write(90);
  servo2.write(90);

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

  // --- Calculate roll angle ---
  roll = atan2(ay, sqrt(ax * ax + az * az)) * 180.0 / PI;

  // --- Control logic ---
    if (roll > 25) {
    // Tilt to the right
    digitalWrite(MOSFET1, HIGH);
    digitalWrite(MOSFET2, HIGH);
    servo1.write(135);  // right
    servo2.write(135);  // right
  } 
  else if (roll < -25) {
    // Tilt to the left
    digitalWrite(MOSFET1, HIGH);
    digitalWrite(MOSFET2, HIGH);
    servo1.write(45);   // left
    servo2.write(45);   // left
  } 
  else {
    // Neutral position
    digitalWrite(MOSFET1, LOW);
    digitalWrite(MOSFET2, LOW);
    servo1.write(90);
    servo2.write(90);
  }
}