#include <Wire.h>
#include <BleMouse.h>

// ===============================
// MPU6050
// ===============================
#define MPU6050_ADDR 0x68

// ===============================
// ESP32 pins
// ===============================
#define LEFT_CLICK_BUTTON   25
#define RIGHT_CLICK_BUTTON  26
#define VIBRATION_PIN       27

// MPU6050 raw data
int16_t accelX, accelY, accelZ;
int16_t gyroX, gyroY, gyroZ;

// Gyroscope offsets
float gyroXOffset = 0;
float gyroYOffset = 0;
float gyroZOffset = 0;

BleMouse bleMouse("Gyroscopic Defence System");

// ===============================
// SETUP
// ===============================
void setup() {
  Serial.begin(115200);
  Wire.begin();

  pinMode(LEFT_CLICK_BUTTON, INPUT_PULLUP);
  pinMode(RIGHT_CLICK_BUTTON, INPUT_PULLUP);
  pinMode(VIBRATION_PIN, OUTPUT);

  digitalWrite(VIBRATION_PIN, LOW);

  // Wake up MPU6050
  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(0x6B);
  Wire.write(0x00);
  Wire.endTransmission();

  // Set gyro range to ±250°/s
  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(0x1B);
  Wire.write(0x00);
  Wire.endTransmission();

  delay(1000);

  Serial.println("GYRO SYSTEM STARTING...");
  Serial.println("Keep the system completely still.");

  calibrateGyro();

  Serial.println("CALIBRATION COMPLETE");
  Serial.println("WAITING FOR BLE CONNECTION...");

  bleMouse.begin();
}

// ===============================
// READ MPU6050
// ===============================
void readMPU6050() {
  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(0x3B);
  Wire.endTransmission(false);

  Wire.requestFrom(MPU6050_ADDR, 14, true);

  accelX = Wire.read() << 8 | Wire.read();
  accelY = Wire.read() << 8 | Wire.read();
  accelZ = Wire.read() << 8 | Wire.read();

  // Temperature - not used
  Wire.read();
  Wire.read();

  gyroX = Wire.read() << 8 | Wire.read();
  gyroY = Wire.read() << 8 | Wire.read();
  gyroZ = Wire.read() << 8 | Wire.read();
}

// ===============================
// GYRO CALIBRATION
// ===============================
void calibrateGyro() {
  long sumX = 0;
  long sumY = 0;
  long sumZ = 0;

  const int samples = 500;

  for (int i = 0; i < samples; i++) {
    readMPU6050();

    sumX += gyroX;
    sumY += gyroY;
    sumZ += gyroZ;

    delay(3);
  }

  gyroXOffset = (float)sumX / samples;
  gyroYOffset = (float)sumY / samples;
  gyroZOffset = (float)sumZ / samples;
}

// ===============================
// VIBRATION
// ===============================
void vibrate() {
  digitalWrite(VIBRATION_PIN, HIGH);
  delay(80);
  digitalWrite(VIBRATION_PIN, LOW);
}

// ===============================
// MAIN LOOP
// ===============================
void loop() {
  readMPU6050();

  // Remove gyro offset
  float correctedX = gyroX - gyroXOffset;
  float correctedY = gyroY - gyroYOffset;
  float correctedZ = gyroZ - gyroZOffset;

  // Convert raw gyro data to degrees/second
  float rotationX = correctedX / 131.0;
  float rotationY = correctedY / 131.0;
  float rotationZ = correctedZ / 131.0;

  // Small dead-zone to reduce sensor noise
  if (abs(rotationX) < 2) rotationX = 0;
  if (abs(rotationY) < 2) rotationY = 0;
  if (abs(rotationZ) < 2) rotationZ = 0;

  if (bleMouse.isConnected()) {

    // Map gyroscope movement to mouse movement.
    // Adjust sensitivity if required for your setup.
    int mouseX = (int)(rotationY * 0.15);
    int mouseY = (int)(-rotationX * 0.15);

    if (mouseX != 0 || mouseY != 0) {
      bleMouse.move(mouseX, mouseY);
    }

    // Left button
    bool leftPressed = digitalRead(LEFT_CLICK_BUTTON) == LOW;

    if (leftPressed) {
      if (!bleMouse.isPressed(MOUSE_LEFT)) {
        bleMouse.press(MOUSE_LEFT);
        vibrate();
      }
    } else {
      if (bleMouse.isPressed(MOUSE_LEFT)) {
        bleMouse.release(MOUSE_LEFT);
      }
    }

    // Right button
    bool rightPressed = digitalRead(RIGHT_CLICK_BUTTON) == LOW;

    if (rightPressed) {
      if (!bleMouse.isPressed(MOUSE_RIGHT)) {
        bleMouse.press(MOUSE_RIGHT);
        vibrate();
      }
    } else {
      if (bleMouse.isPressed(MOUSE_RIGHT)) {
        bleMouse.release(MOUSE_RIGHT);
      }
    }
  }

  delay(10);
}
