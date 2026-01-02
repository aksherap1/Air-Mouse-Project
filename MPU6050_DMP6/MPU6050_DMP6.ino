#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu;

void setup() {
  Serial.begin(115200);
  while (!Serial);

  Serial.println("Starting MPU test...");

  Wire.begin(13, 12);   // SDA, SCL (ESP32-style)
  Wire.setClock(100000);

  mpu.initialize();

  if (mpu.testConnection()) {
    Serial.println("MPU6050 connected!");
  } else {
    Serial.println("MPU6050 NOT FOUND");
    while (1);
  }
}

void loop() {}
