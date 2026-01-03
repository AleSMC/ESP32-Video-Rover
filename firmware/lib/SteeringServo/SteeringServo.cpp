/**
 * @file SteeringServo.cpp
 * @brief Safe Servo Control Implementation.
 * @author Alejandro Moyano (@AleSMC)
 */

#include "SteeringServo.h"

SteeringServo::SteeringServo(int pin, int center, int leftMax, int rightMax)
{
    // Calculate absolute limits regardless of whether Left < Right or vice versa.
    // This makes the code robust against different mechanical assemblies.
    _minLimit = min(leftMax, rightMax);
    _maxLimit = max(leftMax, rightMax);

    _pin = pin;
    _angleCenter = center;
    _angleLeft = leftMax;
    _angleRight = rightMax;
}

void SteeringServo::begin()
{
    // 1. Configure Period (Frequency)
    // Standard analog servos (SG90, MG996R) operate at 50Hz (20ms).
    // @warning Using higher frequencies (>60Hz) can overheat or burn analog servos.
    _servo.setPeriodHertz(50);

    // 2. Configure Pulse Widths
    // Define the mapping between electrical signal (microseconds) and physical rotation.
    // - RC Theoretical Standard: 1000us (0°) to 2000us (180°).
    // - Extended Range (Real Hardware): 500us (0°) to 2400us (180°).
    // We use this wide range to ensure budget servos reach their full travel.
    _servo.attach(_pin, 500, 2400);

    // 3. Initial Position
    center();
}

void SteeringServo::center()
{
    _servo.write(_angleCenter);
}

void SteeringServo::turnLeft()
{
    _servo.write(_angleLeft);
}

void SteeringServo::turnRight()
{
    _servo.write(_angleRight);
}

void SteeringServo::write(int angle)
{
    // --- SAFETY LAYER (HARDWARE PROTECTION) ---
    // Function constrain(val, min, max):
    // If val < min -> returns min.
    // If val > max -> returns max.
    // This physically prevents the servo from receiving a command that breaks the steering.
    int safeAngle = constrain(angle, _minLimit, _maxLimit);

    _servo.write(safeAngle);
}
