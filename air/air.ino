#include <Wire.h>
#include <BleMouse.h>

BleMouse bleMouse;

// MPU6050 I2C address
#define MPU_ADDR 0x68

// Button pins
#define BTN_LEFT   D5
#define BTN_RIGHT  D6
#define BTN_UP     D7
#define BTN_DOWN   D8

// Tilt configuration
const int tiltDeadzone = 500;   // ignore tiny jitter
const float tiltScale = 0.0015;  // adjust cursor speed

// Moving average filter
const int filterSize = 5;
int16_t axBuffer[filterSize], ayBuffer[filterSize];
int filterIndex = 0;

// Gyro calibration offsets
int16_t axOffset = 0, ayOffset = 0;

// Button states for press/release logic
bool leftPressed = false;
bool rightPressed = false;
bool upPressed = false;
bool downPressed = false;

void setup() {
  Serial.begin(115200);
  while (!Serial);

  // Initialize buttons
  pinMode(BTN_LEFT, INPUT_PULLUP);
  pinMode(BTN_RIGHT, INPUT_PULLUP);
  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);

  // Initialize BLE mouse
  bleMouse.begin();
  Serial.println("BLE Mouse ready");

  // Initialize I2C
  Wire.begin(13, 12);
  Wire.setClock(400000);

  // --- Check MPU6050 connection ---
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x75); // WHO_AM_I register
  Wire.endTransmission(false);
  Wire.requestFrom((uint8_t)MPU_ADDR, (size_t)1, true); // ✅ ESP32-safe
  if (Wire.available()) {
    uint8_t whoAmI = Wire.read();
    if (whoAmI == 0x68) {
      Serial.println("MPU6050 detected successfully!");
    } else {
      Serial.print("MPU6050 WHO_AM_I mismatch: 0x");
      Serial.println(whoAmI, HEX);
    }
  } else {
    Serial.println("Error: MPU6050 not detected! Check wiring.");
  }

  // Wake up MPU6050
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B);
  Wire.write(0); 
  Wire.endTransmission(true);
  Serial.println("MPU6050 initialized");

  // Initialize moving average buffers
  for (int i = 0; i < filterSize; i++) {
    axBuffer[i] = 0;
    ayBuffer[i] = 0;
  }

  // Calibrate gyro
  calibrateGyro();
}

// --- Gyro calibration ---
void calibrateGyro() {
  Serial.println("Calibrating gyro... keep device still!");
  long axSum = 0, aySum = 0;
  const int samples = 100;
  for (int i = 0; i < samples; i++) {
    int16_t ax, ay, az;
    readMPU(ax, ay, az);
    axSum += ax;
    aySum += ay;
    delay(5);
  }
  axOffset = axSum / samples;
  ayOffset = aySum / samples;
  Serial.print("Calibration complete: axOffset=");
  Serial.print(axOffset);
  Serial.print(" ayOffset=");
  Serial.println(ayOffset);
}

// --- Read raw MPU6050 values ---
void readMPU(int16_t &ax, int16_t &ay, int16_t &az) {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B); // starting register for accel
  Wire.endTransmission(false);
  Wire.requestFrom((uint8_t)MPU_ADDR, (size_t)6, true); // ✅ ESP32-safe
  ax = Wire.read() << 8 | Wire.read();
  ay = Wire.read() << 8 | Wire.read();
  az = Wire.read() << 8 | Wire.read();
}

// --- Map tilt to mouse movement with deadzone --- (possibly might have to update this part so it stays still at one point)
int mapTilt(int16_t val) {
  if (abs(val) < tiltDeadzone) return 0;
  int scaled = (int)(val * tiltScale);
  if (scaled == 0) return (val > 0 ? 1 : -1); // ensure minimum movement
  return scaled;
}


// --- Moving average filter for smooth movement ---
int filterAxis(int16_t val, int16_t *buffer) {
  buffer[filterIndex] = val;
  long sum = 0;
  for (int i = 0; i < filterSize; i++) sum += buffer[i];
  return sum / filterSize;
}

// --- Handle button clicks and scrolling ---
void handleButtons() {
  // --- Left click ---
  if (digitalRead(BTN_LEFT) == LOW) {
    if (!leftPressed) {
      bleMouse.press(MOUSE_LEFT);
      leftPressed = true;
    }
  } else if (leftPressed) {
    bleMouse.release(MOUSE_LEFT);
    leftPressed = false;
  }

  // --- Right click ---
  if (digitalRead(BTN_RIGHT) == LOW) {
    if (!rightPressed) {
      bleMouse.press(MOUSE_RIGHT);
      rightPressed = true;
    }
  } else if (rightPressed) {
    bleMouse.release(MOUSE_RIGHT);
    rightPressed = false;
  }

  // --- Scroll Up ---
  if (digitalRead(BTN_UP) == LOW) {
    if (!upPressed) {
      bleMouse.move(0, 0, 1); // scroll wheel up
      upPressed = true;
    }
  } else upPressed = false;

  // --- Scroll Down ---
  if (digitalRead(BTN_DOWN) == LOW) {
    if (!downPressed) {
      bleMouse.move(0, 1, -1); // scroll wheel down
      downPressed = true;
    }
  } else downPressed = false;
}

// --- Optional MPU6050 runtime check ---
void checkMPU() {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x75);
  if (Wire.endTransmission(false) != 0) {
    Serial.println("Warning: MPU6050 not responding!");
  }
}

void loop() {
  // Only move mouse if BLE is connected
  if (!bleMouse.isConnected()) return;

  // Optional runtime MPU check every loop
  checkMPU();

  int16_t ax, ay, az;
  readMPU(ax, ay, az);

  // Apply calibration offsets
  ax -= axOffset;
  ay -= ayOffset;

  // Apply moving average filter
  int filtX = filterAxis(ay, ayBuffer); // left/right
  int filtY = filterAxis(ax, axBuffer); // forward/back
  filterIndex = (filterIndex + 1) % filterSize;

  // Map to cursor movement
  int moveX = mapTilt(filtX);
  int moveY = mapTilt(filtY);

  // Limit movement to avoid huge jumps
  moveX = constrain(moveX, -5, 5);
  moveY = constrain(moveY, -5, 5);

  // Move mouse if movement exists
  if (moveX != 0 || moveY != 0) {
    bleMouse.move(moveX, moveY);
    Serial.print("Move → X: "); Serial.print(moveX);
    Serial.print(" Y: "); Serial.println(moveY);
  }

  // Handle all buttons and scroll
  handleButtons();

  delay(20); // ~50Hz update for smooth motion
}
