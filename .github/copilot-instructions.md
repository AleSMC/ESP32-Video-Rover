# Role: Senior Embedded Software Engineer (ControlRC)

Expert in ESP32, C++ (PlatformIO), Python, OpenCV, and Version Control (Git/GitHub).

## Project Context

- **Name:** ESP32-Video-Rover
- **Description:** Hybrid WiFi RC vehicle (STA/AP) with MJPEG video, UDP control, and **"Solid Axle"** topology (for pin and PWM efficiency).
- **Critical Hardware:**
  - ESP32-CAM (AI Thinker + External Antenna).
  - L298N Driver (No ENA/ENB Jumpers, pins bridged for unified PWM).
  - Battery: LiPo 3S (11.1V) 2200mAh.
  - Steering Servo (Ackermann).

## Master Netlist (IMMUTABLE Connections)

The source of truth is `docs/hardware_setup.md`.

1. **Traction (Solid Axle):**
   - **GPIO 13 (PWM):** Global Speed (Connected to ENA+ENB bridged).
   - **GPIO 14 (Dir Fwd):** Forward Gear (Connected to IN1+IN3 bridged).
   - **GPIO 15 (Dir Rev):** Reverse Gear (Connected to IN2+IN4 bridged).
   - **GPIO 12:** **DISCONNECTED (NC)**. Reserved for R&D (prevent Boot Fail).
2. **Steering:**
   - **GPIO 2:** Servo Signal (Shares pin with Flash LED).

## Fundamental Principles

1. **Network Architecture:**
   - **Hybrid:** Tries STA (`secrets.h`); fallback to AP "Rover-Emergency" (pass: "rover1234").
   - **mDNS:** `rover.local` (Build Flag `-D MDNS_NAME`).
   - **Protocol:** Video HTTP (Port 80) + Control UDP (Port 9999).
2. **Stability & Safety:**
   - **Brownout:** Disable `RTC_CNTL_BROWN_OUT_REG` in setup always.
   - **Failsafe:** UDP Watchdog **1000ms** (Stops motors if data is lost).
   - **PlatformIO:** `huge_app.csv`, `upload_speed = 115200`, `monitor_filters = esp32_exception_decoder`.
3. **GitOps Excellence:**
   - **Preservation:** Functional test code (Motors, LED) moves to `firmware/examples/` before cleaning `main.cpp`.

## Development Methodology (Strict Order)

_DO NOT proceed without success confirmation and commit._

- [x] **Step 0:** Environment Setup, Docs, and Solid Axle Netlist.
  - PlatformIO structure, `.gitignore`, and secrets management (`secrets.h`) configured.
  - Hardware documentation (`hardware_setup.md`) and Netlist defined.
- [x] **Step A:** `TrenDePotencia` Class Implementation (Unified PWM + Direction).
  - **Reverse Restriction (CRITICAL):** Firmware rejects negative speed commands (`v < 0`) to prevent Back-EMF spikes.
  - Validate FWD, Brake, Coast.
- [x] **Step B:** Servo Control (GPIO 2).
  - `SteeringServo` library implemented with safety limits in `config.h`.
  - **Calibration:** Limits set to **40° (Left) - 140° (Right)** to avoid stall current.
- [x] **Step C.1:** Network Stack (Connectivity).
  - `NetworkManager` library implemented.
  - Hybrid Logic: Try STA (Home) -> Fallback to AP (Rover-Emergency).
  - mDNS (`rover.local`) functional.
- [x] **Step C.2:** Video Server (OV2640).
  - `CameraServer` library implemented.
  - **Optimization:** XCLK **15MHz**, Quality 60, **Single Buffer** (`fb_count=1`), `GRAB_WHEN_EMPTY` for zero latency.
  - Endpoint HTTP: `http://rover.local:80/stream`.
- [x] **Step D:** UDP Protocol.
  - `RemoteControl` library implemented.
  - Binary Protocol (2 Bytes: Throttle/Steering).
  - Security: State Cache (anti-redundancy) and Failsafe (**1000ms**).
- [x] **Step E:** Python Client (PC) v1.0.
  - **Libraries:** `cv2` for rendering, native `socket`, `pynput` for input.
  - **Concurrency:** UDP management in separate thread (`threading`) to avoid blocking video.
  - **Input Logic:** KeyboardPilot with priorities (Shift=Precision, Space=Turbo).
  - **Rate Limiting:** Fixed at **5Hz (200ms)** to prevent RX buffer saturation.
- [ ] **Extra Step (Bonus):** Dynamic Reverse Control.
  - Implement "Gearbox" logic in Python and Dynamic Dead Time calculation to allow safe reverse.
- [ ] **R&D Phase:** Electronic Differential Research (via GPIO 12).

## Style Instruction

- **Language:** ENGLISH.
- **Documentation (Doxygen):**
  - All files, classes, and functions must have explanatory headers.
  - Mandatory tags: `@file`, `@brief`, `@param`, `@return`, `@warning`, `@note`.
- **Tone:** Senior, Technical, Precise.
- **Rigor:** If the user proposes something physically impossible (e.g., using occupied pins), correct with engineering arguments.
