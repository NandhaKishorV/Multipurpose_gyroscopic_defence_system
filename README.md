# Multipurpose Gyroscopic Defence System

A motion-controlled interactive system built using an **ESP32, MPU6050 gyroscope, physical buttons, vibration feedback, and Bluetooth HID**.

## Overview

The system uses the MPU6050 to detect rotational movement. The ESP32 processes the gyroscope readings and uses Bluetooth HID to communicate with a computer as a wireless mouse.

Physical buttons provide additional input, while a vibration motor provides haptic feedback.

## System Architecture

```text
MPU6050
   |
   v
ESP32
   |
   +---- Physical Buttons
   |
   +---- Vibration Feedback
   |
   v
Bluetooth HID
   |
   v
PC / Game
```

## Hardware

- ESP32
- MPU6050
- Two push buttons
- Vibration motor
- Power supply
- Custom-built controller enclosure

## ESP32 Pin Configuration

| Component | ESP32 GPIO |
|---|---:|
| Left button | GPIO 25 |
| Right button | GPIO 26 |
| Vibration motor | GPIO 27 |
| MPU6050 SDA | ESP32 SDA |
| MPU6050 SCL | ESP32 SCL |
| MPU6050 address | 0x68 |

## How It Works

1. The ESP32 initializes the MPU6050.
2. The gyroscope is configured for a ±250°/s range.
3. The system collects 500 stationary samples for calibration.
4. X/Y/Z gyro offsets are calculated.
5. Raw gyro values are converted to degrees/second.
6. A small dead zone reduces sensor noise.
7. Gyroscope movement is mapped to mouse movement.
8. Physical buttons are mapped to mouse clicks.
9. The vibration motor provides feedback when a button is activated.
10. Bluetooth HID sends the controls wirelessly to the PC.

## Bluetooth HID

The ESP32 uses the `BleMouse` library to present itself to the computer as a Bluetooth mouse.

The computer therefore receives the controller's movement and button input wirelessly without requiring the earlier serial-to-Python control path.

## Software

- Arduino IDE
- Python
- ESP32 board support
- MPU6050
- `BleMouse` library
- C/C++ / Arduino

## Arduino Libraries

The sketch uses:

```cpp
#include <Wire.h>
#include <BleMouse.h>
```

Install the `BleMouse` library before compiling.

## Setup

### 1. Connect the hardware

Wire the MPU6050 and buttons according to the pin configuration above.

### 2. Open the ESP32 sketch

```text
ESP32/gyroscopic_defence_system.ino
```

### 3. Select your ESP32 board

In Arduino IDE, select the appropriate ESP32 board and the correct COM port.

### 4. Upload

Keep the controller still during the initial calibration.

### 5. Pair the ESP32

After uploading, power the ESP32 and pair the Bluetooth mouse device with the PC.

## Demo

Add your demonstration video here:

```text
[▶ Watch the Project Demo](YOUR_VIDEO_LINK)
```

## Project Images

Place your hardware and working-setup photographs inside the `Images/` folder.

Recommended images:

- Complete system
- ESP32 + MPU6050 wiring
- Internal hardware
- Working demonstration

## Future Improvements

- Advanced sensor filtering
- Adjustable motion sensitivity
- Improved calibration and drift compensation
- Additional Bluetooth control mappings
- Custom PCB
- Improved enclosure ergonomics
- Battery monitoring and power management

## Repository Structure

```text
multipurpose-gyroscopic-defence-system/
|
├── ESP32/
│   └── gyroscopic_defence_system.ino
|
├── Python/
│   ├── pc_controller.py
│   └── requirements.txt
|
├── Images/
|
├── README.md
└── .gitignore
```

## Note

The Python controller is retained as a reference for the earlier serial-control implementation. The current ESP32/Bluetooth HID implementation does not require Python for PC mouse control.
