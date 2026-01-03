/**
 * @file SolidAxle.cpp
 * @brief L298N Motor Driver Implementation for ESP32.
 * @author Alejandro Moyano (@AleSMC)
 */

#include "SolidAxle.h"

SolidAxle::SolidAxle(int pinFwd, int pinRev, int pinPWM)
{
    _pinFwd = pinFwd;
    _pinRev = pinRev;
    _pinPWM = pinPWM;
    _velocidadActual = 0;
}

void SolidAxle::begin()
{
    // 1. Pin Configuration (Digital Output)
    pinMode(_pinFwd, OUTPUT);
    pinMode(_pinRev, OUTPUT);
    pinMode(_pinPWM, OUTPUT);

    // 2. PWM Peripheral Configuration (LEDC)
    // ESP32 uses LEDC hardware controller, not analogWrite().
    ledcSetup(_pwmChannel, _pwmFreq, _pwmResolution);
    ledcAttachPin(_pinPWM, _pwmChannel);

    // 3. Safe Initial State
    brake();
}

void SolidAxle::brake()
{
    // L298N Logic: IN1=LOW, IN2=LOW, ENA=HIGH -> Short Brake
    digitalWrite(_pinFwd, LOW);
    digitalWrite(_pinRev, LOW);
    ledcWrite(_pwmChannel, 255);
    _velocidadActual = 0;
}

void SolidAxle::coast()
{
    // L298N Logic: ENA=LOW -> Motor Disabled (Free Run)
    digitalWrite(_pinFwd, LOW);
    digitalWrite(_pinRev, LOW);
    ledcWrite(_pwmChannel, 0);
    _velocidadActual = 0;
}

void SolidAxle::drive(int velocidad)
{
    // --- 1. INTEGRITY VALIDATION ---
    if (velocidad > 255 || velocidad < -255)
    {
        Serial.printf("[ERROR] Motor: Speed %d out of range. Ignored.\n", velocidad);
        return;
    }

    // --- 2. HARDWARE PROTECTION (REVERSE LOCK) ---
    // @warning Sudden reverse generates Back-EMF currents that can burn
    // the L298N or reset the ESP32.
    // In this phase, reverse is blocked until "Dynamic Dead Time" is implemented in client.
    if (velocidad < 0)
    {
        Serial.printf("[WARN] Reverse requested. Blocked for safety.\n");
        brake();
        return;
    }

    // --- 3. DEADZONE ---
    // Cheap DC motors lack torque to move at very low PWM.
    // Cut signal to avoid electric humming without movement.
    if (abs(velocidad) < 15)
    {
        coast();
        return;
    }

    // --- 4. POWER APPLICATION ---
    if (velocidad > 0)
    {
        // Forward Config: IN1=HIGH, IN2=LOW
        digitalWrite(_pinFwd, HIGH);
        digitalWrite(_pinRev, LOW);
    }
    else if (velocidad < 0)
    {
        // Reverse Config: IN1=LOW, IN2=HIGH
        digitalWrite(_pinFwd, LOW);
        digitalWrite(_pinRev, HIGH);
    }

    // PWM is always positive (velocity vector magnitude)
    ledcWrite(_pwmChannel, abs(velocidad));
    _velocidadActual = velocidad;
}
