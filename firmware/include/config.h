/**
 * @file config.h
 * @brief Master Hardware Configuration and System Constants.
 * @details Single Source of Truth (SSOT) for GPIO mapping and physical robot parameters.
 *
 * --- HARDWARE SPECIFICATIONS ---
 * @board ESP32-CAM (AI Thinker Model).
 * @driver L298N Dual H-Bridge (Configured in Solid Axle Mode).
 * @actuator Standard Servo (SG90/MG90S) for Ackermann steering.
 *
 * --- FEATURES ---
 * - Pin Mapping (GPIO).
 * - Mechanical Calibration (Servo).
 * - Protocol Constants (Ports and Timings).
 *
 * @warning DO NOT include WiFi credentials here. Use 'secrets.h'.
 * @author Alejandro Moyano (@AleSMC)
 */

#pragma once
#include <Arduino.h>

// =============================================================================
// 1. TRACTION CONFIGURATION (SOLID AXLE TOPOLOGY)
// =============================================================================
// Rear motors (Left and Right) share electrical signals.

/** * @brief Global Speed Pin (PWM).
 * @details Connected to driver pins ENA and ENB (Physically bridged).
 * Controls power from 0% to 100%.
 * @note GPIO 13 is safe for PWM and does not interfere with ESP32 boot.
 */
#define PIN_MOTOR_PWM 13

/** * @brief Steering Pin: Forward Gear (FWD).
 * @details Connected to driver IN1 and IN3 (Physically bridged).
 * When activated, both wheels rotate forward.
 */
#define PIN_MOTOR_FWD 14

/** * @brief Steering Pin: Reverse Gear (REV).
 * @details Connected to driver IN2 and IN4 (Physically bridged).
 * When activated, both wheels rotate backward.
 * @warning Reverse must be used with caution (See Back-EMF documentation).
 */
#define PIN_MOTOR_REV 15

/** * @brief RESERVED Pin (Not Connected).
 * @warning CRITICAL: GPIO 12 is a 'Strapping Pin' (MTDI).
 * If the L298N driver holds this pin HIGH during Boot, the ESP32 will
 * incorrectly set the internal Flash voltage (to 1.8V) and fail to boot.
 * @note Design Solution: Physically left disconnected in this version.
 */
#define PIN_RESERVED_12 12

// =============================================================================
// 2. STEERING CONFIGURATION (ACKERMANN SERVO)
// =============================================================================

/** * @brief PWM Signal Pin for the servo.
 * @note GPIO 2 shares the physical line with the high-power Flash LED.
 * It is expected behavior to see LED flashes when moving the steering.
 */
#define PIN_SERVO 2

// --- ANGLE CALIBRATION (DEGREES 0-180) ---
// ADJUST THESE VALUES GRADUALLY TO AVOID FORCING THE MECHANISM

/** @brief Center Angle (Straight wheels). Ideal theoretical value: 90. */
#define STEERING_CENTER 90

/** * @brief Max Left Limit.
 * Start with a value near 90 (e.g., 75) and decrease slowly towards 0.
 * If you hear buzzing, you have hit the physical stop: back off 5 degrees immediately.
 */
#define STEERING_LEFT_MAX 40

/** * @brief Max Right Limit.
 * Start with a value near 90 (e.g., 105) and increase slowly towards 180.
 */
#define STEERING_RIGHT_MAX 140

// =============================================================================
// 3. PROTOCOL CONFIGURATION (SYSTEM CONSTANTS)
// =============================================================================

/** * @brief UDP Listening Port for Commands.
 * @details The Rover will listen for control packets (e.g., binary throttle/steering) on this port.
 * @note Must match the sending port in the Python client.
 */
const int UDP_PORT = 9999;

/** * @brief TCP Port for Web Server.
 * @details Standard HTTP port to serve the interface and MJPEG stream.
 */
const int HTTP_PORT = 80;

// --- Safety ---

/** * @brief Max time without receiving UDP packets before activating Failsafe.
 * @details If 1000ms pass without valid commands, the software Watchdog
 * will stop the motors to prevent the robot from running away if WiFi is lost.
 */
const int UDP_FAILSAFE_MS = 1000;
