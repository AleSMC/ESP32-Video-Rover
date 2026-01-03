/**
 * @file test_servo_calibration.cpp
 * @brief Steering Calibration Test (Step B).
 * @author Alejandro Moyano (@AleSMC)
 *
 * @details
 * Validation script for the steering servo (Ackermann).
 * Cycles the mechanism: Center -> Left -> Center -> Right.
 *
 * @note
 * Goal: Visually adjust STEERING_LEFT_MAX and STEERING_RIGHT_MAX limits
 * in 'config.h' to obtain maximum travel without forcing the mechanism.
 *
 * =================================================================================
 * @section execution Deployment Procedure (CLI)
 * =================================================================================
 *
 * 1. HARDWARE PREPARATION:
 * - Disconnect USB cable.
 * - Ensure battery power (Servo does not work on USB only).
 * - IMPORTANT: GPIO 2 shares the line with the Flash LED. Flashing is normal.
 * - Reconnect USB cable for uploading.
 *
 * 2. SOFTWARE PREPARATION:
 * - This code must be in 'firmware/src/main.cpp'.
 * - Configure conservative initial limits in 'config.h'.
 *
 * 3. TERMINAL COMMANDS (From project root):
 * $ cd firmware
 * $ pio run -t upload
 * $ pio device monitor -b 115200
 *
 * 4. VERIFICATION AND CALIBRATION:
 * - Observe wheel turning.
 * - If turning is insufficient: Increase/Decrease angle in 'config.h' (5 degrees at a time).
 * - If servo makes noise (buzzing) at limits: BACK OFF 5-10 degrees immediately.
 *
 * 5. PRESERVATION:
 * - Once calibrated, move this code to 'firmware/examples/test_servo_calibration.cpp'.
 * =================================================================================
 */

#include <Arduino.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "config.h"
#include "SteeringServo.h"

// Instantiate servo with limits defined in config.h
SteeringServo steering(PIN_SERVO, STEERING_CENTER, STEERING_LEFT_MAX, STEERING_RIGHT_MAX);

void setup()
{
    // 1. Power Management
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

    // 2. Serial
    Serial.begin(115200);
    Serial.println("\n[BOOT] Servo Test Started");

    // 3. Start Servo
    steering.begin();
    Serial.println("[INFO] Servo initialized and centered.");

    // Initial wait to observe center
    delay(2000);
}

void loop()
{
    // --- 1. CENTER ---
    Serial.printf("[TEST] Straight (Angle: %d)\n", STEERING_CENTER);
    steering.center();
    delay(2000);

    // --- 2. LEFT ---
    Serial.printf("[TEST] Left (Angle: %d)\n", STEERING_LEFT_MAX);
    steering.turnLeft();
    delay(2000);

    // --- 3. CENTER ---
    Serial.println("[TEST] Straight...");
    steering.center();
    delay(1000);

    // --- 4. RIGHT ---
    Serial.printf("[TEST] Right (Angle: %d)\n", STEERING_RIGHT_MAX);
    steering.turnRight();
    delay(2000);
}
