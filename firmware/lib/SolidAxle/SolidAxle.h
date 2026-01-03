/**
 * @file SolidAxle.h
 * @brief Unified Traction Controller (Solid Axle Topology).
 * @author Alejandro Moyano (@AleSMC)
 * @version 1.0.0
 * @details
 * Manages two DC motors connected in parallel (same PWM, same Direction).
 * Abstracts H-Bridge (L298N) logic and provides safety methods to
 * avoid inductive current spikes (Back-EMF).
 */

#pragma once
#include <Arduino.h>

class SolidAxle
{
private:
    // --- Hardware Pins (L298N Configuration) ---
    int _pinFwd; ///< Logic pin to activate H-Bridge in forward direction
    int _pinRev; ///< Logic pin to activate H-Bridge in reverse direction
    int _pinPWM; ///< Enable Pin for Pulse Width Modulation

    // --- Internal State ---
    int _velocidadActual; ///< Last commanded speed (-255 to 255)

    // --- PWM Configuration (ESP32 LEDC) ---
    const int _pwmFreq = 1000;    ///< 1kHz Frequency (Optimal for generic DC motors)
    const int _pwmChannel = 0;    ///< PWM Channel 0
    const int _pwmResolution = 8; ///< 8-bit Resolution (Range 0-255)

public:
    /**
     * @brief Driver Constructor.
     * @param pinFwd GPIO connected to IN1+IN3.
     * @param pinRev GPIO connected to IN2+IN4.
     * @param pinPWM GPIO connected to ENA+ENB.
     */
    SolidAxle(int pinFwd, int pinRev, int pinPWM);

    /**
     * @brief Initializes GPIO pins and LEDC peripheral (PWM).
     * @note Initial state: Brake activated.
     */
    void begin();

    /**
     * @brief Main movement command.
     * @param velocidad Signed value [-255 to 255].
     * - Positive: Forward.
     * - Negative: Reverse (Blocked by default in Phase A).
     * - 0: Coast (Inertia).
     * @warning Includes range protection. Values >255 are ignored.
     */
    void drive(int velocidad);

    /**
     * @brief Magnetic Brake (Short Brake).
     * @details Sets control pins to LOW and PWM to max.
     * This short-circuits the motor coils, generating a back-electromotive force
     * that stops the axle quickly.
     */
    void brake();

    /**
     * @brief Inertia Mode (Coasting).
     * @details Disables the H-Bridge (High Impedance).
     * The motor remains electrically disconnected and spins freely by inertia.
     */
    void coast();
};
