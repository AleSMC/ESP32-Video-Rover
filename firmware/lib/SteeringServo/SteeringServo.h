/**
 * @file SteeringServo.h
 * @brief Ackermann Steering Controller (Servo).
 * @author Alejandro Moyano (@AleSMC)
 * @version 1.0.0
 * @details
 * Wrapper around the ESP32Servo library adding a safety layer (Hard Limits).
 * Translates logical commands (0-180 degrees) into physically protected PWM signals.
 */

#pragma once
#include <Arduino.h>
#include <ESP32Servo.h>

class SteeringServo
{
private:
    Servo _servo; ///< Low-level driver instance
    int _pin;     ///< PWM signal GPIO pin

    // --- Calibration Parameters ---
    int _angleCenter; ///< Calibrated value for going straight
    int _angleLeft;   ///< Physical left limit
    int _angleRight;  ///< Physical right limit

    // --- Safety Limits (Calculated) ---
    int _minLimit; ///< Lowest allowed numerical value (e.g., 70)
    int _maxLimit; ///< Highest allowed numerical value (e.g., 110)

public:
    /**
     * @brief Constructor with physical limits.
     * @param pin Servo GPIO.
     * @param center Center angle (ideally 90).
     * @param leftMax Max left angle.
     * @param rightMax Max right angle.
     */
    SteeringServo(int pin, int center, int leftMax, int rightMax);

    /**
     * @brief Initializes servo PWM at 50Hz.
     */
    void begin();

    /**
     * @brief Sets wheels to straight position.
     */
    void center();

    /**
     * @brief Turns to the allowed left stop.
     */
    void turnLeft();

    /**
     * @brief Turns to the allowed right stop.
     */
    void turnRight();

    /**
     * @brief Moves the servo to a specific angle safely.
     * @param angle Target angle (0-180).
     * @note If the angle exceeds configured limits, it is automatically clamped
     * to avoid forcing the mechanism.
     */
    void write(int angle);
};
