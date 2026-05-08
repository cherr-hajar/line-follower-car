# Wiring Diagram

## Overview

Battery → L298N → ESP32 + Motors + IR Sensors

## Power flow

Li-Po (+) ──→ L298N 12V
Li-Po (−) ──→ L298N GND
L298N 5V  ──→ ESP32 VIN
L298N GND ──→ ESP32 GND

## Motors

L298N OUT1 ──→ Right motor (+)
L298N OUT2 ──→ Right motor (−)
L298N OUT3 ──→ Left motor (+)
L298N OUT4 ──→ Left motor (−)

## IR Sensors

All sensors share 3.3V and GND from ESP32.
Each sensor OUT pin goes to its own GPIO.

LEFT   → GPIO 33
CENTER → GPIO 35
RIGHT  → GPIO 34

## Notes

- If a motor spins the wrong way, swap its two output wires
- Sensors must be 0.5–1 cm from the ground
- All GND pins must be connected together