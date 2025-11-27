#include <Arduino.h>
#include <Wire.h>
#include <TinyMPU6050.h>

// ====== MPU6050 Setup ======
MPU6050 mpu(Wire);

// ====== Motor Driver Pins (BTS7960) ======
// Motor 1 (Left)
#define L_RPWM 3
#define L_LPWM 9
// Motor 2 (Right)
#define R_RPWM 5
#define R_LPWM 6

// ====== Movement Settings ======
int motorSpeed = 120; // PWM value (0–255)
int threshold = 20;   // Tilt angle threshold in degrees
int deadZone = 5;     // Optional small dead zone to prevent jitter

// ====== Motor Control Functions ======
void stopMotors() {
  analogWrite(L_RPWM, 0);
  analogWrite(L_LPWM, 0);
  analogWrite(R_RPWM, 0);
  analogWrite(R_LPWM, 0);
}

void moveBackward() {
  Serial.println("Backward");

  analogWrite(L_RPWM, 0);
  analogWrite(L_LPWM, motorSpeed);
  analogWrite(R_RPWM, 0);
  analogWrite(R_LPWM, motorSpeed);
}

void moveForward() {
  Serial.println("Forward");
    analogWrite(L_RPWM, motorSpeed);
  analogWrite(L_LPWM, 0);
  analogWrite(R_RPWM, motorSpeed);
  analogWrite(R_LPWM, 0);

}

void turnLeft() {
  Serial.println("left");
  analogWrite(L_RPWM, 0);
  analogWrite(L_LPWM, motorSpeed);
  analogWrite(R_RPWM, motorSpeed);
  analogWrite(R_LPWM, 0);
}

void turnRight() {
  Serial.println("right");
  analogWrite(L_RPWM, motorSpeed);
  analogWrite(L_LPWM, 0);
  analogWrite(R_RPWM, 0);
  analogWrite(R_LPWM, motorSpeed);
}

// Optional: filter tiny tilts to prevent motor jitter
float applyDeadZone(float value, float dz) {
  if (fabs(value) > dz) return value;
  return 0;
}

// ====== Setup ======
void setup() {
  Serial.begin(115200);
  Wire.begin();

  pinMode(L_RPWM, OUTPUT);
  pinMode(L_LPWM, OUTPUT);
  pinMode(R_RPWM, OUTPUT);
  pinMode(R_LPWM, OUTPUT);

  Serial.println("Initializing MPU6050...");
  mpu.Initialize();
  Serial.println("Calibrating MPU6050...");
  mpu.Calibrate();
  Serial.println("Calibration complete!");
}

// ====== Main Loop ======
void loop() {
  mpu.Execute();  // Update angles

  float angleX = applyDeadZone(mpu.GetAngX(), deadZone);  // Forward/back tilt
  float angleY = applyDeadZone(mpu.GetAngY(), deadZone);  // Left/right tilt
  float angleZ = mpu.GetAngZ();                            // Optional

  Serial.print("AngleX: "); Serial.print(angleX);
  Serial.print("\tAngleY: "); Serial.print(angleY);
  Serial.print("\tAngleZ: "); Serial.println(angleZ);

  // Movement logic based on tilt angles
  if (angleX > threshold) {
    moveForward();
  }
  else if (angleX < -threshold) {
    moveBackward();
  }
  else if (angleY > threshold) {
    turnRight();
  }
  else if (angleY < -threshold) {
    turnLeft();
  }
  else {
    stopMotors();
  }

  delay(10);
}