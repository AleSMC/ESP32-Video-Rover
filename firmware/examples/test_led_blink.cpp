/**
 * @file test_led_blink.cpp
 * @brief System Integrity Verification (Sanity Check).
 * @author Alejandro Moyano (@AleSMC)
 *
 * @details
 * Standalone test module to validate:
 * 1. Correct compilation and firmware upload (Toolchain).
 * 2. ESP32 SoC Boot sequence.
 * 3. Basic GPIO control (Status LED blinking).
 *
 * @note Hardware: Onboard LED (GPIO 33 on AI-Thinker).
 * @note Logic: Negative Logic (Active LOW).
 *
 * =================================================================================
 * @section execution Deployment Procedure (CLI)
 * =================================================================================
 *
 * 1. SOFTWARE PREPARATION:
 * - Copy the entire content of this file.
 * - Paste it into 'firmware/src/main.cpp' (overwriting current content).
 *
 * 2. TERMINAL COMMANDS (From project root):
 * $ cd firmware
 * $ pio run -t upload
 * $ pio device monitor -b 115200
 *
 * 3. VERIFICATION:
 * - The rear Red LED should blink (Brief flash every 2 seconds).
 * - The serial monitor should display "Heartbeat: System Active".
 * =================================================================================
 */

#include <Arduino.h>

// Onboard LED Pin Definition.
// GPIO 33 is the standard for the AI-Thinker ESP32-CAM board.
#define STATUS_LED_PIN 33

/**
 * @brief System Initialization.
 */
void setup()
{
    // Initialize serial bus for debug logs
    Serial.begin(115200);

    // Configure GPIO as digital output
    pinMode(STATUS_LED_PIN, OUTPUT);

    // Stabilization delay for the serial monitor to catch up
    delay(1000);

    // System Boot Logs
    Serial.println("\n--- SYSTEM DIAGNOSTIC: LED BLINK TEST ---");
    Serial.println("[INFO] Boot: SUCCESS");
    Serial.println("[INFO] Board: ESP32-CAM AI-Thinker");
    Serial.println("[INFO] Status: Ready");
}

/**
 * @brief Main Loop (Heartbeat).
 */
void loop()
{
    // Emit heartbeat signal to serial port
    Serial.println("[STATUS] Heartbeat: System Active");

    // Visual indication cycle (Active LOW)
    // ON State (Sink to Ground)
    digitalWrite(STATUS_LED_PIN, LOW);
    delay(100); // Pulse duration: 100ms

    // OFF State (High Impedance/VCC)
    digitalWrite(STATUS_LED_PIN, HIGH);
    delay(2000); // Cycle frequency: ~0.5Hz
}
