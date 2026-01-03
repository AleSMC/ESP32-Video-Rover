/**
 * @file test_motors_solid_axle.cpp
 * @brief Traction Unit Test (Step A) - Hardware Validation.
 * @author Alejandro Moyano (@AleSMC)
 *
 * @details
 * Validation script for the "Solid Axle" topology.
 * Executes a cyclic routine: Acceleration (Ramp) -> Inertia (Coast) -> Brake.
 *
 * @note
 * This test also verifies that the firmware correctly rejects reverse commands (safety feature).
 *
 * =================================================================================
 * @section execution Deployment Procedure (CLI)
 * =================================================================================
 *
 * 1. HARDWARE PREPARATION (CRITICAL):
 * - Disconnect the USB cable.
 * - Lift the chassis (wheels must NOT touch the ground).
 * - Connect the LiPo battery to the L298N driver.
 * - Reconnect the USB cable for uploading.
 *
 * 2. SOFTWARE PREPARATION:
 * - Copy the entire content of this file.
 * - Paste it into 'firmware/src/main.cpp' (overwriting current content).
 *
 * 3. TERMINAL COMMANDS (From project root):
 * $ cd firmware
 * $ pio run -t upload
 * $ pio device monitor -b 115200
 *
 * 4. VERIFICATION:
 * - Observe the physical behavior of the wheels.
 * - Verify that the logs in the monitor match the actions.
 * =================================================================================
 */

#include <Arduino.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "config.h"    // Pin definitions (Netlist)
#include "SolidAxle.h" // Abstraction library

// Global instance of the traction system
SolidAxle rover(PIN_MOTOR_FWD, PIN_MOTOR_REV, PIN_MOTOR_PWM);

void setup()
{
    // 1. Power Management: Disable Brownout Detector
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

    // 2. Start serial port
    Serial.begin(115200);
    Serial.println("\n[BOOT] Rover System Started (Mode: SolidAxle)");

    // 3. Start traction subsystem
    rover.begin();
}

void loop()
{
    Serial.println("[INFO] Starting Motor Test in 10 seconds...");
    Serial.println("[INFO] Ensure wheels are not touching the ground.");
    for (int i = 10; i > 0; i--)
    {
        Serial.printf("...%d\n", i);
        delay(1000);
    }

    // --- TEST 1: PROGRESSIVE ACCELERATION (Ramp) ---
    Serial.println("[TEST] 1. Accelerating gently (Forward)...");
    for (int i = 0; i <= 255; i += 5)
    {
        rover.drive(i);
        delay(20); // Smooth ramp
    }
    delay(1000); // Hold max speed

    // --- TEST 2: INERTIA (Coasting) ---
    Serial.println("[TEST] 2. Coast Mode (Free roll)...");
    rover.coast();
    delay(2000);

    // --- TEST 3: SUDDEN ACCELERATION (Safety Check) ---
    Serial.println("[TEST] 3. Acceleration Test...");
    rover.drive(150); // Half speed forward
    delay(3000);

    // --- TEST 4: FULL BRAKE ---
    Serial.println("[TEST] 4. Hard Brake...");
    rover.brake();
    delay(3000);

    // --- TEST 5: ERROR HANDLING TEST ---
    Serial.println("[TEST] 5. Error Test: Attempting to send invalid value (500).");
    rover.drive(500);
    delay(1000);

    // --- TEST 6: REVERSE ERROR TEST ---
    Serial.println("[TEST] 6. Error Test: Attempting to send negative value (-200).");
    rover.drive(-200);
    delay(1000);

    Serial.println("[TEST] 7. Cycle end.");
    delay(1000);
}