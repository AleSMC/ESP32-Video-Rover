# 🚜 ESP32-Video-Rover

> **Hybrid WiFi RC vehicle with MJPEG video transmission and UDP control.**

This project implements a remote-controlled rover using an **ESP32-CAM** (AI Thinker). The system operates under a **"Solid Axle"** topology (Unified Rear Traction) to ensure startup stability and resource efficiency, featuring Ackermann steering via a servo motor.

## 📂 Project Structure

    ESP32-Video-Rover/
    ├── firmware/               # C++ Source Code (PlatformIO)
    │   ├── src/                # Main Logic (.cpp)
    │   ├── include/            # Headers (.h) and Configuration
    │   ├── lib/                # Modular Libraries
    │   │   ├── SolidAxle/      # Traction Driver (Solid Axle Topology)
    │   │   ├── SteeringServo/  # Steering Driver (Ackermann Servo)
    │   │   ├── NetworkManager/ # Connectivity Manager (WiFi STA/AP + mDNS)
    │   │   ├── CameraServer/   # Video Driver (OV2640 + MJPEG Web Server)
    │   │   └── RemoteControl/  # UDP Protocol & Failsafe Logic
    │   ├── examples/           # Preserved Unit Tests (Motors, Servo, LED)
    │   └── platformio.ini      # Build Environment Configuration
    ├── software/               # PC Client (Python + OpenCV + UDP)
    │   ├── modules/            # Decoupled Logic Modules
    │   │   ├── __init__.py     # Python Package Initializer
    │   │   ├── KeyboardPilot.py # Keyboard Driver (pynput + Priorities)
    │   │   └── VideoStream.py   # Asynchronous Video Decoder (Threading)
    │   ├── main.py             # Main Executable (Control Loop)
    │   └── requirements.txt    # Dependencies (opencv, pynput, numpy)
    ├── docs/                   # Technical Documentation, Diagrams, and Notes
    └── README.md               # This file

## 🛠 Hardware & Connections

**Platform:** ESP32-CAM (AI Thinker Model) with modified external antenna.
**Traction Topology:** Parallel (Solid Axle). Both rear motors receive the same PWM signal.

> ℹ️ **Full Details:** See the setup guide, netlist, and warnings in [docs/hardware_setup.md](docs/hardware_setup.md).

| Logic Signal       | ESP32 Pin | L298N Connection | Technical Notes                     |
| :----------------- | :-------- | :--------------- | :---------------------------------- |
| **PWM (Speed)**    | GPIO 13   | ENA + ENB        | Bridged. Power Control (0-100%).    |
| **Dir Fwd**        | GPIO 14   | IN1 + IN3        | Bridged. Forward Gear.              |
| **Dir Rev**        | GPIO 15   | IN2 + IN4        | Bridged. Reverse Gear.              |
| **Steering Servo** | GPIO 2    | PWM Signal       | Shares line with Flash LED.         |
| **Reserved (R&D)** | GPIO 12   | **NC**           | _Not Connected_ to avoid Boot Fail. |

> **Note:** The _Brownout Detector_ has been disabled in software to prevent resets caused by motor current spikes.

## 🚀 Quick Start (Firmware)

### Prerequisites

- VSCode with **PlatformIO** extension.
- CH340 Driver (if using MB base) or FTDI adapter.

### Installation

1. Clone the repository.
2. Open the root folder in VSCode.
3. Create the credentials file:
   Copy `firmware/include/secrets_example.h` to `firmware/include/secrets.h` and fill in your WiFi details.

### Build & Upload

    # From PlatformIO Terminal
    cd firmware
    pio run -t upload

_If upload fails:_ Hold the `IO0` button (or connect GPIO0 to GND) and press Reset before uploading.

### Monitoring

To see debug logs (Assigned IP, Motor State):

    pio device monitor -b 115200

## 📡 Network Architecture

- **Hybrid Mode:** Tries to connect to STA (Home WiFi). If it fails after 10s, it deploys the AP "Rover-Emergency".
- **Discovery:** mDNS enabled at `rover.local`.
- **Protocols:**
  - **Video:** HTTP Server (MJPEG Stream).
  - **Control:** UDP (Default Port: `UDP_PORT` in config).
- **Safety (Failsafe):** 1000ms Watchdog. If no UDP packets are received, motors stop.

> **⚠️ SAFETY NOTE (REVERSE):**
> Reverse logic is **disabled in base firmware** (Phase A) to prevent Back-EMF current spikes. Safe reverse implementation (with Dynamic Dead Time) is handled via the Python Client in advanced stages.

## 🎮 Controls & Telemetry

The Rover relies on a custom low-latency communication protocol designed to prioritize responsiveness over reliability, a standard approach in FPV robotics.

### 1. Communication Protocol (UDP Layer)

Unlike the video stream which uses HTTP (TCP), the control link utilizes **UDP (User Datagram Protocol)** over port `9999`.

- **Why UDP?** TCP introduces latency due to handshakes and ACKs. UDP allows "fire-and-forget" transmission, ensuring the rover always acts on the _latest_ command available.
- **Packet Structure:** The Python client samples the keyboard state at **5Hz** and encodes it into a **2-byte binary payload**:
  - `Byte[0]`: Traction State (0=Coast, 1=Brake, 2-255=PWM Speed).
  - `Byte[1]`: Steering Angle (0-180 degrees).

### 2. Input Mapping & Behavior

| Key / Combination  | Function            | Mechanical Action   | Technical Description                                                                                        |
| :----------------- | :------------------ | :------------------ | :----------------------------------------------------------------------------------------------------------- |
| **NONE (Release)** | **Inertia (Coast)** | Motors Disconnected | **High Impedance (Hi-Z).** H-Bridge disables output. Current drops to 0A, allowing the rover to roll freely. |
| **W**              | **Forward**         | Normal Speed        | Applies **standard PWM duty cycle** (e.g., ~70%) to the traction motors.                                     |
| **W + SPACE**      | **Turbo Boost**     | Max Speed           | Bypasses speed limiter, increasing **PWM to 100% (255)**. Ideal for straightaways.                           |
| **S**              | **Reverse / Brake** | Active Braking      | **Short Brake Mode.** Driver pulls motor terminals to Ground (LOW/LOW), using Back-EMF to stop rotation.     |
| **A / D**          | **Steering**        | Ackermann Turn      | Maps Servo to `STEERING_LEFT_MAX` or `RIGHT_MAX`. Includes software end-stops.                               |
| **ESC**            | **Emergency Stop**  | System Halt         | Sends a "Kill Signal" packet and terminates client connection.                                               |

### 3. Priority Hierarchy

To prevent conflicting commands, the firmware implements strict **Priority Logic**:

1.  **FAILSAFE (Highest):** If no UDP packets >1000ms -> **Stop**.
2.  **BRAKE (`S`):** Overrides acceleration. Safety first.
3.  **STEERING (`A` vs `D`):** If both pressed -> **Center (90°)**.
4.  **THROTTLE (`W`):** Active only if Brake is released.

## ✅ Development Roadmap

- [x] **Step 0:** Environment Setup & GitOps.
- [x] **Step A:** Motor Driver Implementation (Solid Axle with PWM). _Implemented FWD/Brake/Coast._
- [x] **Step B:** Steering Servo Control.
- [x] **Step C.1:** Network Stack (Hybrid WiFi + mDNS).
  - _Rover connects automatically or creates its own AP._
- [x] **Step C.2:** Video Streaming (OV2640).
  - _Asynchronous Web Server with Low Latency MJPEG streaming._
- [x] **Step D:** UDP Control Protocol.
  - **Architecture:** Fire-and-Forget unidirectional communication for low latency.
  - **Port:** `9999` (Configurable).
  - **Packet Structure (Binary - 2 Bytes):**
    - `Byte[0]` **Traction:** `0` (Coast), `1` (Brake), `2-255` (PWM Forward).
    - `Byte[1]` **Steering:** `0-180` (Servo Angle).
  - **Safety (Failsafe):** **1000ms** Timeout -> Emergency Brake.
- [x] **Step E:** Python Client (PC) v1.0.
  - **Input:** Migration to `pynput` (Hardware Input) supporting diagonals (W+A) and combos (Shift/Space).
  - **Video:** Asynchronous decoding in dedicated thread to eliminate rendering lag.
  - **Network:** Rate Limiting **(5Hz)** to prevent RX buffer saturation on ESP32.
- [ ] **Extra Step (Bonus):** Dynamic Reverse Control.
  - Implement safety logic in Python to calculate required braking time based on previous speed before sending reverse commands.
- [ ] **R&D Phase (Bonus):** Electronic Differential Research. Evaluate viability of safely using GPIO 12 (Strapping Pin).

## 💻 Software Architecture (PC Client)

The Python client (`software/main.py`) is designed following **Real-Time System** patterns to decouple vision from control.

### 1. Asynchronous Video Pipeline (`VideoStream.py`)

Unlike basic OpenCV examples that block the main loop, this system uses `threading`:

- **Background Thread:** Constantly downloads MJPEG frames and keeps only the latest one in memory (`buffer_size=1`). If processing is slow, it drops old frames to ensure we always see the "present".
- **Main Thread:** Only handles painting the already decoded image, ensuring 0ms blocking on control.

### 2. Hardware Interrupt Piloting (`KeyboardPilot.py`)

Uses the **`pynput`** library:

- **Advantage:** Reads physical key states (Press/Release events).
- **Capability:** Allows complex combinations like **Drift (W+A+Space)**, perfect diagonals, and variable speed control (Shift for precision) without "ghosting".
- **Priority Logic:**
  1. `S` (Brake) > `W` (Throttle).
  2. `Shift` (Precision) > `Space` (Turbo) > Normal.

### 3. UDP Traffic Management (Rate Limiting)

The ESP32 has a single antenna (Half-Duplex). To prevent collisions between Video Upload and Command Download:

- The client limits UDP packet transmission to **200ms (5Hz)**.
- This frees up the air spectrum 90% of the time, allowing video to flow without interruptions.

---

**License:** MIT License. See `LICENSE` file for full text.
