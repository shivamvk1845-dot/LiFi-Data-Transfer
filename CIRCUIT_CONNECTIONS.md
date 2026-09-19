# Li-Fi / VLC Circuit Connections

This document describes the current hardware connections used by the Li-Fi / Visible Light Communication (VLC) project.

> The connections below are based on the current Arduino transmitter/receiver code and the hardware setup shown in the project photos. The current working version does **not** use the LM358.

---

## 1. Transmitter Circuit

### Components
- Arduino Uno
- Red LED
- Current-limiting resistor (use the resistor actually installed in the working circuit; a typical LED setup uses about 220 ohms)
- Breadboard
- Jumper wires
- USB cable

### Arduino Uno to LED

| Arduino Pin | Connection |
|---|---|
| D3 | LED signal/output through the LED current-limiting resistor |
| GND | LED return / cathode side |

The transmitter code defines:

```cpp
#define LED_PIN 3
```

Therefore, **digital pin D3** is the data output pin.

### Working principle

The Arduino rapidly switches the LED ON and OFF according to the transmitted bits.

```text
Arduino D3
   |
   v
 LED + resistor
   |
   v
Visible light
```

The LED is used as the optical transmitter.

---

## 2. Receiver Circuit

### Components
- Arduino Uno
- BPW34 photodiode
- 16x2 I2C LCD
- Breadboard
- Jumper wires
- Resistor(s) used in the photodiode sensing circuit
- USB cable

The receiver code defines:

```cpp
const int SENSOR_PIN = A0;
```

Therefore, the photodiode sensing voltage is read through **analog pin A0**.

---

## 3. BPW34 Photodiode to Arduino

### Main signal connection

| Receiver Arduino Pin | Connection |
|---|---|
| A0 | Photodiode sensor output / sensing node |
| 5V | Photodiode circuit supply, as used in the working setup |
| GND | Photodiode circuit ground |

The BPW34 detects the intensity changes produced by the transmitting LED.

The Arduino reads the changing sensor voltage using:

```cpp
analogRead(A0)
```

The program then compares the reading against a threshold to decide whether the received optical signal represents HIGH or LOW.

```cpp
bool lightDetected() {
  return analogRead(SENSOR_PIN) >= threshold;
}
```

### Photodiode orientation

The BPW34 is a polarity-sensitive photodiode. Its exact leg orientation must match the physical wiring used in the working circuit. When rebuilding the circuit, verify the BPW34 pinout from the component datasheet before powering the circuit.

---

## 4. I2C LCD to Receiver Arduino

The code uses:

```cpp
LiquidCrystal_I2C lcd(0x27, 16, 2);
```

So the LCD is an I2C 16x2 module with address **0x27** in the current setup.

For an Arduino Uno:

| LCD Pin | Arduino Uno |
|---|---|
| VCC | 5V |
| GND | GND |
| SDA | A4 |
| SCL | A5 |

If the LCD is blank, the I2C address may need to be checked. The code comment also notes **0x3F** as an alternative address.

---

## 5. Receiver Breadboard Layout

The receiver breadboard contains the photodiode sensing section and its supporting resistive connections.

The basic signal path is:

```text
Transmitting LED
      |
      |  Visible light
      v
 BPW34 Photodiode
      |
      |  Sensor voltage
      v
 Arduino A0
      |
      v
Digital data decoding
      |
      v
I2C LCD
```

The current version performs the signal processing directly with the Arduino's analog input. **LM358 signal conditioning is not part of this current version.**

---

## 6. Data Transmission Format

The transmitter does more than simply blink the LED.

### Timing values

```cpp
BIT_TIME   = 150 ms
START_TIME = 600 ms
GAP_TIME   = 300 ms
```

Each byte is transmitted as:

```text
START BIT
8 DATA BITS (LSB first)
STOP BIT
```

The transmitter first sends the message length, then the message characters, and finally a checksum.

### Checksum

The transmitter calculates:

```cpp
checksum ^= c;
```

The receiver calculates the checksum again and compares it with the received checksum.

If they match:

```text
RX OK [VERIFIED]
```

is shown on the LCD.

---

## 7. Complete System Connection

```text
TRANSMITTER

Arduino Uno
    |
    | D3
    v
LED + Resistor
    |
    |  Optical signal
    v
==========================

RECEIVER

BPW34 Photodiode
    |
    v
Arduino Uno A0
    |
    v
Data decoding
    |
    v
I2C LCD
A4 = SDA
A5 = SCL
```

---

## 8. Important Notes

1. The transmitter Arduino and receiver Arduino are separate boards.
2. The optical link between the LED and BPW34 is wireless; the two sides do not require a signal wire between them.
3. The receiver uses `analogReference(INTERNAL)` and calibrates a baseline at startup.
4. The current working version is limited to messages of **up to 16 characters**, matching the 16x2 LCD display.
5. Keep the LED and BPW34 aligned and avoid excessive ambient light for more reliable reception.
6. The current documented hardware setup does **not** include the LM358. LM358 can be added later as a signal-conditioning improvement.

---

## Pin Summary

### Transmitter Arduino Uno

| Pin | Function |
|---|---|
| D3 | LED data output |
| GND | LED return |
| USB | Power / Serial communication |

### Receiver Arduino Uno

| Pin | Function |
|---|---|
| A0 | BPW34 sensor input |
| A4 | LCD SDA |
| A5 | LCD SCL |
| 5V | LCD / sensor circuit supply |
| GND | Common ground |
| USB | Power / Serial communication |
