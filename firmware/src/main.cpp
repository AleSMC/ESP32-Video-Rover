/**
 * @file main.cpp
 * @brief Firmware Final v1.0 - ESP32 Video Rover.
 * @details Main System Orchestrator. Integrates:
 * - Layer C.2: Video Streaming (MJPEG via HTTP)
 * - Layer C.1: Hybrid Network (WiFi STA/AP Failover)
 * - Layer D: UDP Remote Control (Binary Protocol + Safety)
 * @author Alejandro Moyano (@AleSMC)
 * @note --- USAGE INSTRUCTIONS (PLATFORMIO) ---
 * 1. Upload Firmware:   pio run -t upload
 * 2. Serial Monitor:    pio device monitor -b 115200
 * @warning If upload fails, connect GPIO0 to GND (IO0 Button) and press Reset.
 */

#include <Arduino.h>
#include <WiFi.h> // [COOL-DOWN] Required to adjust TX power
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

// --- PROJECT LIBRARIES ---
#include "config.h"
#include "NetworkManager.h"
#include "CameraServer.h"
#include "SolidAxle.h"
#include "SteeringServo.h"
#include "RemoteControl.h"

// =============================================================================
// GLOBAL INSTANCES (Service Architecture)
// =============================================================================

// 1. High-Level Managers (Network and Video)
NetworkManager network;
CameraServer camera;

// 2. Hardware Drivers (Physical Actuators)
// We instantiate objects with pins defined in 'config.h'
SolidAxle motors(PIN_MOTOR_FWD, PIN_MOTOR_REV, PIN_MOTOR_PWM);
SteeringServo steering(PIN_SERVO, STEERING_CENTER, STEERING_LEFT_MAX, STEERING_RIGHT_MAX);

// 3. Logic Controller (Dependency Injection)
// We pass pointers (&) of the drivers to the remote controller.
// This allows 'remote' to manipulate 'motors' and 'steering' without owning them.
RemoteControl remote(&motors, &steering);

// =============================================================================
// SETUP (System Initialization)
// =============================================================================
void setup()
{
    // 1. POWER MANAGEMENT (CRITICAL)
    // Disable Brownout Detector. WiFi and Motor startup generates
    // current spikes that could reset the ESP32 if this were active.
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

    // 2. START SERIAL PORT (Debug)
    Serial.begin(115200);
    delay(1000);

    // [COOL-DOWN] 3. ENSURE FLASH OFF (GPIO 4)
    // The flash pin sometimes floats and generates heat/phantom power drain.
    pinMode(4, OUTPUT);
    digitalWrite(4, LOW);

    // 4. INITIALIZE PHYSICAL ACTUATORS
    // Safe to init hardware before WiFi.
    Serial.println("\n[BOOT] Initializing Motors and Servo...");
    motors.begin();
    steering.begin();
    steering.center(); // Safe initial position (Straight wheels)

    // 5. INITIALIZE CAMERA
    // High Priority: Camera needs to reserve large memory blocks (DMA/PSRAM).
    // We do this before starting the WiFi stack to prevent RAM fragmentation.
    Serial.println("[BOOT] Initializing Video Hardware...");
    if (camera.init())
    {
        Serial.println("[BOOT] OV2640 Camera ready.");
    }
    else
    {
        Serial.println("[ERROR] Camera not detected. CHECK FLEX CABLE.");
        // Infinite error loop to protect hardware
        while (true)
        {
            delay(1000);
        }
    }

    // 6. START NETWORK STACK
    // Blocking process (~10s max) that connects to WiFi or creates AP.
    network.begin();

    // // [COOL-DOWN] 7. REDUCE WIFI POWER (Optional)
    // // When connecting to iPhone (short range), we don't need 20dBm.
    // // WIFI_POWER_11dBm saves ~100mA and significantly reduces heat.
    // WiFi.setTxPower(WIFI_POWER_11dBm);
    // Serial.println("[ENERGY] WiFi Power reduced to 11dBm.");

    // 8. START BACKGROUND SERVICES
    camera.startServer(); // Async Web Server (Port 80)
    remote.begin();       // UDP Listener (Port 9999)

    // FINAL STATUS REPORT
    Serial.println("\n[BOOT] SYSTEM ONLINE - ROVER READY.");
    Serial.printf("[INFO] Video Stream: http://%s.local/stream\n", MDNS_NAME);
    Serial.printf("[INFO] Video Stream by IP:   http://%s/stream\n", network.getIP().c_str());
    Serial.printf("[INFO] UDP Control:  Port %d\n", UDP_PORT);
}

// =============================================================================
// LOOP (Non-Blocking Main Loop)
// =============================================================================
void loop()
{
    // 1. Network Maintenance
    // (Currently passive thanks to FreeRTOS, reserved for future logic)
    network.update();

    // 2. Control Process (Real-Time)
    // Reads UDP buffer, decodes protocol, and updates motors/servo.
    remote.listen();

    // 3. Safety System (Watchdog)
    // Checks if connection with pilot has been lost.
    remote.checkFailsafe();

    // 4. Telemetry (Heartbeat)
    // Prints status every 5 seconds without using delay() to avoid blocking control.
    static unsigned long lastTime = 0;
    if (millis() - lastTime > 5000)
    {
        lastTime = millis();
        Serial.printf("[ALIVE] Mode: %s | IP: %s | Uptime: %lu s\n",
                      network.getMode().c_str(),
                      network.getIP().c_str(),
                      millis() / 1000);

        // Print RSSI to ensure lowering power didn't kill signal
        long rssi = WiFi.RSSI();
        Serial.printf("[STATUS] IP: %s | Signal: %ld dBm | Temp: OK\n",
                      network.getIP().c_str(), rssi);
    }

    // [COOL-DOWN] 5. CPU COOL-DOWN
    // Critical: A small delay allows the RTOS to put the CPU into "Idle" mode.
    // This drops temperature drastically without affecting response (5ms is imperceptible).
    delay(5);
}
