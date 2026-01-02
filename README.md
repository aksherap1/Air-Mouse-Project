# Air Mouse Project

### About  
The Air Mouse is a wireless, motion-controlled computer mouse. Using an ESP32 microcontroller and an MPU-6050 IMU sensor, it detects hand movement and translates it into cursor movement on your computer. A push button allows for left-click functionality.  

This project demonstrates:

- Bluetooth HID communication
- Sensor integration
- Embedded programming w/Arduino IDE
- PCB Building
- Gyro

---

### Materials Used  

- ESP32 Nano Dev Board
- MPU-6050
- Push Button
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
  - Solder components (ESP32 module, sensor, button) onto the PCB
  - Upload same Arduino code to ESP32 module

7. Finalize device
- Add battery for wireless use
- Place all components in an enclosure for portability
- Ensure smooth movement and reliable clicking

---

### Analysis

---

### Citations

- https://github.com/udit1567/Circuit_forge/tree/main/Air%20Mouse
- https://github.com/T-vK/ESP32-BLE-Mouse 
