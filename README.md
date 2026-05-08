# Line Following Robot — ESP32

A 3-sensor infrared line following robot (car) built with an ESP32 
and L298N motor driver. Built as a school project.

---

## What it does

The robot uses 3 IR sensors to detect a black line on the floor 
and follow it automatically. It calibrates itself on startup by 
reading both the floor color and the line color, then calculates 
thresholds on its own.

---

## Hardware

| Component        | Details                        |
|------------------|--------------------------------|
| Microcontroller  | ESP32 (CP2102)                 |
| Motor Driver     | L298N H-Bridge                 |
| Sensors          | 3x IR sensors (digital output) |
| Motors           | 2x DC motors with wheels       |
| Power            | Li-Po battery                  |
| Chassis          | Cardboard (prototype)          |

---

## Wiring

| ESP32 Pin | Connected To      |
|-----------|-------------------|
| GPIO 25   | L298N ENA         |
| GPIO 26   | L298N IN1         |
| GPIO 27   | L298N IN2         |
| GPIO 13   | L298N ENB         |
| GPIO 14   | L298N IN3         |
| GPIO 12   | L298N IN4         |
| GPIO 33   | IR Sensor LEFT    |
| GPIO 35   | IR Sensor CENTER  |
| GPIO 34   | IR Sensor RIGHT   |
| 3.3V      | IR Sensors VCC    |
| GND       | IR Sensors GND    |

**Power:**
- Battery (+) → L298N 12V
- Battery (−) → L298N GND
- L298N 5V → ESP32 VIN
- L298N GND → ESP32 GND

---

## How to use

1. Open `src/main.ino` in Arduino IDE 2.x
2. Install **esp32 by Espressif Systems** from Boards Manager
3. Select **ESP32 Dev Module** as your board
4. Upload the code via USB
5. Disconnect USB and connect the battery
6. On startup the robot will calibrate automatically:
   - First 3 seconds: place it on the **plain floor**
   - Next 3 seconds: place it on the **black line**
7. After calibration it will start following the line on its own

---

## Calibration tips

- Keep sensors **0.5 to 1 cm** from the ground
- Make sure all 3 sensors point straight down, not at an angle
- Do calibration in the same lighting you'll use when running
- If the robot keeps losing the line, press RESET and recalibrate

---

## Problems we ran into and how we fixed them

**Motors turning the wrong way**
The right motor was spinning backwards. Instead of changing the 
code we just swapped the two motor wires physically on the L298N 
output. Simple fix, no code needed.

**Robot going straight instead of turning**
The turning speed was too low so the robot just pushed forward 
instead of actually turning. We increased the turning speed and 
changed the logic so one motor goes forward while the other goes 
backward for tight turns. That made a huge difference.

**Robot randomly stopping and losing the line**
The sensors were mounted too high (around 3–4 cm from the ground). 
We lowered them to about 0.5 cm and recalibrated. After that the 
detection became much more reliable.

**Sensors not detecting the line properly**
We opened the Serial Monitor and watched the raw sensor values. 
The difference between black and white wasn't big enough. We 
adjusted the sensor height and recalibrated until the gap between 
the two readings was clear.

**Robot not reacting to curves**
The original turning logic stopped one motor completely. For tight 
curves that wasn't enough. We changed it so that in a sharp turn 
one motor goes forward and the other goes backward at the same time. 
That made the robot spin faster and actually follow the curve.

**Motors tested fine but robot wouldn't move**
The USB cable powers the ESP32 but not the motors. The L298N needs 
the battery connected and the switch turned ON to actually drive 
the motors. Once we did that everything worked.

---

## What we learned

- Always test motors before worrying about sensors
- Sensor height matters a lot — too far from the ground and it 
  doesn't work
- Use Serial Monitor to debug, it shows you exactly what the 
  robot is seeing
- Sharp turns need one motor forward and one backward, not just 
  one motor stopped
- Calibrate every time you change surface or lighting

---

## Built with
- Arduino IDE 2.3.8
- esp32 by Espressif Systems 3.3.8