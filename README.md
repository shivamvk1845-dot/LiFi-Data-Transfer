# Li-Fi Data Transfer using Arduino

An Arduino-based **Visible Light Communication (VLC)** project for transmitting digital data using visible light.

## How It Works

The system transmits data by rapidly switching an LED/Laser ON and OFF.

**Transmitter:**
Arduino → LED/Laser → Visible Light

**Receiver:**
Visible Light → BPW34 Photodiode → Arduino → LCD

The transmitted message is detected by the BPW34 photodiode and displayed on a 16×2 LCD.

## Components Used

- Arduino Uno × 2
- LED/Laser
- BPW34 Photodiode
- 16×2 I2C LCD
- Resistors
- Jumper wires
- Breadboard

## Features

- Visible Light Communication (VLC)
- Digital data transmission through LED light
- BPW34 photodiode based reception
- LCD display of received message
- Framed data transmission
- Checksum-based error verification

## Current Status

Working prototype with successful data transmission and reception.

## Project Structure

```text
LiFi-Data-Transfer/
│
├── transmitter/
│   └── transmitter.ino
│
├── receiver/
│   └── receiver.ino
│
└── README.md
