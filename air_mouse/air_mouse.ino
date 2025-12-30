#include <Wire.h>
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#include <BleMouse.h>

// ----- BLE Mouse -----
BleMouse bleMouse;

// ----- MPU6050 -----
MPU6050 mpu;
const int INTERRUPT_PIN = 2;  // MPU6050 INT pin
volatile bool mpuInterrupt = false; 

// DMP variables
bool dmpReady = false;
uint8_t devStatus;
uint16_t packetSize;
uint8_t fifoBuffer[64];

// Orientation
Quaternion q;
VectorFloat gravity;
float ypr[3];

// LED for feedback
bool ledState = false;

// ----- Interrupt handler -----
void dmpDataReady() { mpuInterrupt = true; }

void setup() {
  Serial.begin(115200);
  while (!Serial);

  // I2C setup (SDA=13, SCL=12 for Nano ESP32)
  Wire.begin(13, 12);
  Wire.setClock(400000);

  // MPU6050 setup
  Serial.println("Initializing MPU6050...");
  mpu.initialize();
  pinMode(INTERRUPT_PIN, INPUT);

  if (!mpu.testConnection()) {
    Serial.println("MPU6050 connection failed!");
    while (1);
  }
  Serial.println("MPU6050 connected.");

  // Initialize DMP
  devStatus = mpu.dmpInitialize();
  mpu.setXGyroOffset(0);
  mpu.setYGyroOffset(0);
  mpu.setZGyroOffset(0);
  mpu.setXAccelOffset(0);
  mpu.setYAccelOffset(0);
  mpu.setZAccelOffset(0);

  if (devStatus == 0) {
    mpu.CalibrateAccel(6);
    mpu.CalibrateGyro(6);
    mpu.setDMPEnabled(true);
    attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), dmpDataReady, RISING);
    packetSize = mpu.dmpGetFIFOPacketSize();
    dmpReady = true;
    Serial.println("DMP ready!");
  } else {
    Serial.print("DMP init failed (code ");
    Serial.print(devStatus);
    Serial.println(")");
    while(1);
  }

  // Initialize BLE Mouse
  bleMouse.begin();

  // LED for feedback
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  if (!dmpReady) return;

  // Only read if MPU interrupt occurred
  if (mpuInterrupt) {
    mpuInterrupt = false;

    if (mpu.dmpGetCurrentFIFOPacket(fifoBuffer)) {
      mpu.dmpGetQuaternion(&q, fifoBuffer);
      mpu.dmpGetGravity(&gravity, &q);
      mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);

      Serial.print("Yaw: "); Serial.print(ypr[0]*180/M_PI);
      Serial.print("\tPitch: "); Serial.print(ypr[1]*180/M_PI);
      Serial.print("\tRoll: "); Serial.println(ypr[2]*180/M_PI);

      // Move BLE mouse if connected
      if (bleMouse.isConnected()) {
        int xMove = map(ypr[2]*180/M_PI, -45, 45, -10, 10); // roll -> X
        int yMove = map(ypr[1]*180/M_PI, -45, 45, -10, 10); // pitch -> Y
        bleMouse.move(xMove, yMove);
      }

      // Blink LED to indicate activity
      ledState = !ledState;
      digitalWrite(LED_BUILTIN, ledState);
    }
  }
}
