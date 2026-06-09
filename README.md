# Line Following Robot — ESP32

A 3 sensor infrared line following robot (car) built with an **ESP32** and **L298N motor driver**.  
Built as a school project.

---

## What it does

The robot uses 3 IR sensors to detect a black line on the floor and follow it automatically.  
It calibrates itself on startup by reading both the floor color and the line color, then calculates thresholds on its own.

---

## Hardware

| Component        | Details                     |
|-----------------|-----------------------------|
| Microcontroller | ESP32 (CP2102)              |
| Motor Driver    | L298N H-Bridge              |
| Sensors         | 3x IR sensors (digital out) |
| Motors          | 2x DC motors with wheels    |
| Power           | Li-Po battery (7.4V)        |
| Chassis         | Cardboard (prototype)       |

---

## Wiring

### ESP32 → L298N (Motor Driver)

| ESP32 Pin | Connected To     |
|-----------|------------------|
| GPIO 25   | L298N ENA        |
| GPIO 26   | L298N IN1        |
| GPIO 27   | L298N IN2        |
| GPIO 13   | L298N ENB        |
| GPIO 14   | L298N IN3        |
| GPIO 12   | L298N IN4        |

### ESP32 → IR Sensors

| Sensor    | ESP32 Pin |
|-----------|-----------|
| LEFT      | GPIO 33   |
| CENTER    | GPIO 35   |
| RIGHT     | GPIO 34   |

- **VCC** of all sensors → **3.3V** (ESP32)
- **GND** of all sensors → **GND** (ESP32)

### Power (Battery → L298N → ESP32)
Battery (+) → L298N 12V
Battery (−) → L298N GND
L298N 5V → ESP32 VIN
L298N GND → ESP32 GND


> Use **two Li-Po batteries in series** (7.4V total) for proper motor power.

---

## How to use

1. Open `src/main.ino` in **Arduino IDE 2.x**
2. Install **esp32 by Espressif Systems** from Boards Manager
3. Select **ESP32 Dev Module** as your board
4. Upload the code via USB
5. Disconnect USB and connect the battery
6. On startup the robot will calibrate automatically:
   - **First 3 seconds:** place it on the plain floor
   - **Next 3 seconds:** place it on the black line
7. After calibration it will start following the line on its own

---

## Calibration tips

- Keep sensors **2 to 3 cm** from the ground
- Make sure all 3 sensors point **straight down**, not at an angle
- Do calibration in the **same lighting** you'll use when running
- If the robot keeps losing the line, press **RESET** (or **EN** in ESP32) and recalibrate

---

## Problems we ran into and how we fixed them

### Motors turning the wrong way  
The right motor was spinning backwards.  
→ We swapped the two motor wires physically on the L298N output.

### Robot going straight instead of turning  
Turning speed was too low.  
→ We increased turning speed and changed the logic so one motor goes **forward** while the other goes **backward** for tight turns.

### Robot randomly stopping and losing the line  
Sensors were mounted too high (3–4 cm from the ground).  
→ We lowered them to about **0.5 cm** and recalibrated.

### Sensors not detecting the line properly  
We opened the Serial Monitor and saw the difference between black and white was too small.  
→ We adjusted sensor height and recalibrated until the readings were clearly separated.

### Robot not reacting to curves  
Original logic stopped one motor completely.  
→ We changed it so that in a sharp turn, one motor goes **forward** and the other goes **backward** at the same time.

### Motors tested fine but robot wouldn't move on the floor  
USB powers the ESP32 but not the motors.  
→ We connected the **battery to the L298N** and turned the switch ON.

### One motor weaker than the other (robot pulled to one side)  
The right motor was more powerful.  
→ We increased the left motor’s speed slightly in the code (`VITESSE_GAUCHE > VITESSE_NORMALE`).

### Robot got stuck after losing the line  
The robot would just stop.  
→ We added a **line search function** that makes it turn left and right until it finds the line again.

### ESP32 wouldn’t upload (error: “No serial data received”)  
The board wasn’t entering download mode.  
→ We pressed and held **BOOT**, pressed **EN**, released **BOOT**, then uploaded.

### L298N didn’t turn on (no LED)  
The L298N was dead.  
→ We replaced it with a new one.

### Switch blocked the current  
The switch was faulty or wired incorrectly.  
→ We removed the switch and connected the battery directly to the L298N.

---

## What we learned

- Always test motors before worrying about sensors  
- Sensor height matters a lot too far from the ground and it doesn’t work  
- Use Serial Monitor to debug, it shows you exactly what the robot is seeing  
- Sharp turns need **one motor forward and one backward**, not just one motor stopped  
- Calibrate every time you change surface or lighting  
- Battery voltage is critical — 3.7V is too weak for the L298N, 7.4V works perfectly  

---

## Built with

- Arduino IDE 2.3.8  
- esp32 by Espressif Systems 3.3.8  

---
