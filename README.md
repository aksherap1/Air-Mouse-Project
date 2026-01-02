# Air Mouse Project

<img width="532" height="562" alt="Screenshot 2026-01-02 at 2 03 32 PM" src="https://github.com/user-attachments/assets/8e3393e4-f817-40f2-b9a1-1a8abe832844" />

### About  
The Air Mouse is a wireless, motion-controlled computer mouse. Using an ESP32 microcontroller and an MPU-6050 IMU sensor, it detects hand movement and translates it into cursor movement on your computer. A push button allows for left-click functionality.  

This project demonstrates:

- Bluetooth HID communication
- Sensor integration
- Embedded programming w/Arduino IDE
- PCB Building
- Gyroscope + accelerometer processing
- Motion-controlled input devices
- Button input handling
- Real-time data processing
- Adjustable sensitivity & deadzone implementation

---

### Materials Used  

- ESP32 Nano Dev Board
- MPU-6050
- 4 Push Buttons
- Breadboard
- Jumper Wires
- USB Cable
- PCB

---

### Process

**1. Set up development environment**
- Install Arduino IDE
- Add ESP32 board support
- Install ESP32-BLE-Mouse library by T-vK

**2. Connect hardware (Breadboard)**
- Wire MPU-6050 to ESP32 via I2C (SDA -> ESP32 Nano D13, SCL -> ESP32 Nano D12, VCC -> 3.3V, GND -> GND)
- Connect push button to a digital input pin (e.g., D5, D6, D7, D8) & connect the other terminal to GND
- Use a breadboard for easy prototyping

**3. Test BLE Mouse**
- Open: File -> Examples ->ESP32 BLE Mouse -> BLE_Mouse in Arduino IDE
- Upload code via USB
- Check Bluetooth settings - ESP32 should appear as a mouse

**4. Integrate motion control**
- Read MPU-6050 values
- Map tilt and acceleration data to mouse movement commands
- Adjust sensitivity and filtering for smooth cursor movement

**5. Test clicking**
- Configure the push buttons to send left-click commands via BLE
- Verify clicking works with cursor movement

**6. Design a PCB**
- After a working breadboard prototype:
  - Draw schematic of ESP32, MPU-6050, and button
  - Designed PCB layout using KiCad
  - Order PCB
  - Solder components (ESP32 Nano module, sensor, button) onto the PCB
  - Upload same Arduino code to ESP32 module

7. Finalize device
- Add battery for wireless use
- Place all components in an enclosure for portability
- Ensure smooth movement and reliable clicking

---

### Analysis

- The MPU-6050 gyroscope and accelerometer detect the device's orientation and motion in 3D space. The accelerometer measures linear acceleration (tilt and movement), while the gyroscope measures rotational velocity. Together, they allow the ESP32 to move the mouse cursor based on how you tilt or rotate the device.
- Built-in gyro calibration establishes a baseline “neutral” position when the device is stationary. This means the system knows what counts as “no movement,” preventing the cursor from drifting due to small biases in the sensor readings.
- The moving average filter is a mathematical method to smooth out rapid fluctuations in sensor data. It averages several consecutive readings so that sudden spikes or jitters (like tiny hand tremors) do not cause erratic mouse movement.
- Adjustable deadzone defines a small range of tilt values that are ignored. Any motion inside this range is treated as zero, which prevents the cursor from moving unintentionally when the device is nearly still.
- Sensitivity determines how strongly tilt translates into cursor movement. Higher sensitivity makes the cursor move faster for a small tilt, while lower sensitivity requires more tilt for the same cursor motion.
- By combining calibration, filtering, deadzone, and adjustable sensitivity, the system achieves smooth, stable, and intuitive motion control, accurately translating intended device movements into cursor actions while ignoring unintended micro-movements.

---

### Citations

- https://github.com/udit1567/Circuit_forge/tree/main/Air%20Mouse
- https://github.com/T-vK/ESP32-BLE-Mouse 
