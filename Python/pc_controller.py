# This PC-side Python controller is kept as a reference for the
# earlier serial-control version of the project.
#
# The current ESP32 implementation uses BLE HID directly through
# BleMouse, so this Python program is NOT required for the BLE version.

import serial
import time
import keyboard

PORT = "COM13"
BAUD = 115200

LEFT_KEY = "a"
RIGHT_KEY = "d"
SHOOT_KEY = "space"
TILT_THRESHOLD = 10.0

try:
    ser = serial.Serial(PORT, BAUD, timeout=1)
except serial.SerialException as e:
    print(f"Could not open {PORT}: {e}")
    raise SystemExit(1)

time.sleep(2)

current_state = "CENTER"
shooting = False

def release_movement_keys():
    keyboard.release(LEFT_KEY)
    keyboard.release(RIGHT_KEY)

def parse_line(line):
    values = {}
    for item in line.split(","):
        if ":" not in item:
            continue
        key, value = item.split(":", 1)
        try:
            values[key.strip()] = float(value.strip())
        except ValueError:
            return None

    if not {"X", "Y", "Z", "L", "R"}.issubset(values):
        return None

    return values

try:
    while True:
        line = ser.readline().decode(errors="ignore").strip()

        if not line:
            continue

        data = parse_line(line)
        if data is None:
            continue

        right_pressed = data["R"] == 1

        if right_pressed and not shooting:
            keyboard.press(SHOOT_KEY)
            shooting = True
        elif not right_pressed and shooting:
            keyboard.release(SHOOT_KEY)
            shooting = False

        angle = data["Y"]

        if angle < -TILT_THRESHOLD:
            if current_state != "LEFT":
                keyboard.release(RIGHT_KEY)
                keyboard.press(LEFT_KEY)
                current_state = "LEFT"

        elif angle > TILT_THRESHOLD:
            if current_state != "RIGHT":
                keyboard.release(LEFT_KEY)
                keyboard.press(RIGHT_KEY)
                current_state = "RIGHT"

        else:
            if current_state != "CENTER":
                release_movement_keys()
                current_state = "CENTER"

        print(
            f"X: {data['X']:6.1f} | "
            f"Y: {data['Y']:6.1f} | "
            f"Z: {data['Z']:6.1f} | "
            f"State: {current_state:6} | "
            f"Shooting: {shooting}"
        )

except KeyboardInterrupt:
    print("\nExiting...")

finally:
    release_movement_keys()
    keyboard.release(SHOOT_KEY)
    ser.close()
