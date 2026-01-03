# 🔧 Hardware Setup Guide

## 1. Connection Table (Master Netlist - Solid Axle Mode)

**This table is the SOURCE OF TRUTH.** If the visual diagram contradicts this table, **follow the table**.

For this configuration, remove Jumpers `ENA` and `ENB` from the L298N driver and create physical bridges between control pins to unify traction.

| Logic Function     | ESP32-CAM Pin | L298N Pins (Bridged) | Technical Description                           |
| :----------------- | :------------ | :------------------- | :---------------------------------------------- |
| **Speed (PWM)**    | **GPIO 13**   | **ENA** + **ENB**    | Global power control. Allows Coasting if PWM=0. |
| **Forward Dir**    | **GPIO 14**   | **IN1** + **IN3**    | Activates both motors forward.                  |
| **Reverse Dir**    | **GPIO 15**   | **IN2** + **IN4**    | Activates both motors backward.                 |
| **Steering Servo** | **GPIO 2**    | **PWM Signal**       | Orange/Yellow Servo Cable.                      |
| **Logic Power**    | **Pin 5V**    | **5V Out**           | ESP32 power from L298N regulator.               |
| **Common Ground**  | **Pin GND**   | **GND**              | **CRITICAL:** Common voltage reference.         |
| **Reserved**       | **GPIO 12**   | _Disconnected_       | See limitations section below.                  |

> ⚠️ **WARNING ABOUT GPIO 2 (SERVO):**
> GPIO 2 is internally connected to the ESP32-CAM **Flash LED**.
> When sending PWM signals to the servo, **the LED will flash or turn on**, which is normal but can generate heat. To avoid this, you must desolder the LED or cover it with tape.

## 1.1 Cable Unification Guide (The "Solid Axle Hack")

To control 4 inputs of the L298N with only 3 ESP32 pins, signal duplication is required.

### Option A: Mini-Breadboard (Recommended for Prototypes)

Non-destructive and clean.

1. Place ESP32-CAM on the breadboard.
2. Route **GPIO 14** to an empty line.
3. From that line, route two cables: one to **IN1** and another to **IN3**.
4. Repeat for **GPIO 15** (to IN2/IN4) and **GPIO 13** (to ENA/ENB).

### Option B: "Y" Dupont Cables (Splicing)

If not using a chassis breadboard:

1. Cut one end of two female cables.
2. Strip and join their copper tips with a male cable tip.
3. Solder and protect with heat shrink tubing.
   _Result:_ A "Y" shaped cable (1 Male to ESP32 -> 2 Females to Driver).

## 3. Troubleshooting, Technical Limitations & Pin Reservations

### A. ESP32 Resets when moving Servo (Brownout)

The L298N 5V regulator may not be sufficient for WiFi + Servo simultaneously.

- **Solution:** Use an external 5V UBEC/Buck Converter directly from the battery for the Servo, sharing only Ground (GND).

### B. Antenna Modification

To optimize video range with external antenna:

1. Locate IPEX connector near the metal shield.
2. Verify the 0-ohm resistor position. It must bridge the path to the IPEX connector, disabling the PCB antenna.

### C. GPIO 12 Restriction (Strapping Pin)

GPIO 12 determines internal flash voltage (VDD_SDIO) during Boot.

- **Risk:** If pulled HIGH during reset, ESP32 sets flash voltage to 1.8V (instead of 3.3V), causing "Flash voltage mismatch".
- **Design Decision:** Kept **DISCONNECTED** in this phase to ensure boot stability.

### D. mDNS Connectivity (rover.local) on Mobile Hotspots

- **Symptom:** Accessible via IP but **NOT** via name.
- **Cause:** Mobile hotspots often block Multicast traffic.
- **Solution:** Use direct IP when using mobile data.

### E. WiFi Interference Optimization (Channel 11)

- **Camera Config:** Reduced XCLK frequency to **10MHz** and increased JPEG quality to 60. This reduces bandwidth usage, making the system robust against physical interference.
