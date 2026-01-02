#include <Wire.h>
#include <BleMouse.h>

BleMouse bleMouse;

#define MPU_ADDR 0x68

#define BTN_LEFT   D5
#define BTN_RIGHT  D6
#define BTN_UP     D7
#define BTN_DOWN   D8

// Tilt settings
const int tiltDeadzone = 1000;  // ignore small jitter
const float tiltScale = 0.01;   // scale raw accel to pixels

// Button scroll
const int buttonScroll = 5;

void setup() {
  Serial.begin(115200);
  while (!Serial);

  pinMode(BTN_LEFT, INPUT_PULLUP);
  pinMode(BTN_RIGHT, INPUT_PULLUP);
  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);

  bleMouse.begin();
  Serial.println("BLE Mouse ready");

  Wire.begin(13, 12); // SDA, SCL
  Wire.setClock(400000);

  // Wake MPU6050
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);

  Serial.println("MPU6050 initialized (RAW mode)");
}

int mapTilt(int16_t val) {
  if (abs(val) < tiltDeadzone) return 0;
  // scale to 0–20 pixels roughly
  return (int)(val * tiltScale);
}

void loop() {
  if (!bleMouse.isConnected()) return;

  int16_t ax, ay, az;
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom((uint8_t)MPU_ADDR, (uint8_t)6, (uint8_t)true);
  ax = Wire.read() << 8 | Wire.read();
  ay = Wire.read() << 8 | Wire.read();
  az = Wire.read() << 8 | Wire.read();

  int scrollX = 0;
  int scrollY = 0;

  // Buttons
  if (digitalRead(BTN_LEFT)  == LOW) scrollX -= buttonScroll;
  if (digitalRead(BTN_RIGHT) == LOW) scrollX += buttonScroll;
  if (digitalRead(BTN_UP)    == LOW) scrollY -= buttonScroll;
  if (digitalRead(BTN_DOWN)  == LOW) scrollY += buttonScroll;

  // Tilt (swap axes if needed)
  scrollX += mapTilt(ay); // left/right
  scrollY += mapTilt(ax); // forward/back

  // Apply movement
  if (scrollX != 0 || scrollY != 0) {
    bleMouse.move(scrollX, scrollY);
    Serial.print("Tilt → X: "); Serial.print(scrollX);
    Serial.print(" Y: "); Serial.println(scrollY);
  }

  delay(15);
}
