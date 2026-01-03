/**
 * @file RemoteControl.cpp
 * @brief UDP Control Protocol Implementation (Low Latency).
 * @author Alejandro Moyano (@AleSMC)
 * @details
 * Implements a 2-byte binary protocol for traction and steering control.
 * Includes safety mechanisms (Failsafe) and CPU optimization (State Cache)
 * to avoid redundant writes to PWM drivers.
 */

#include "RemoteControl.h"

RemoteControl::RemoteControl(SolidAxle *motors, SteeringServo *steering)
{
    // Hardware dependency injection
    _motors = motors;
    _steering = steering;

    _lastPacketTime = 0;
    _failsafeActive = false;

    // Initialize cache with out-of-range values (255) to
    // force physical hardware update on the first received packet.
    _prevSpeed = 255;
    _prevAngle = 255;
}

void RemoteControl::begin()
{
    _udp.begin(UDP_PORT); // Port defined in config.h
    Serial.printf("[UDP] Listening for binary protocol on port %d\n", UDP_PORT);
}

void RemoteControl::listen()
{
    int packetSize = _udp.parsePacket();

    // Basic Filter: Process only if packet matches protocol size (2 bytes)
    // Byte 0: Traction | Byte 1: Steering
    if (packetSize >= 2)
    {
        _udp.read(_packetBuffer, 2); // Read only relevant 2 bytes

        // DEBUG: Show received data
        // Serial.printf("[UDP] Motor: %d | Servo: %d\n", _packetBuffer[0], _packetBuffer[1]);

        // 1. WATCHDOG RESET
        // Received heartbeat/signal from controller, reset timer.
        _lastPacketTime = millis();

        // 2. RECOVERY MANAGEMENT (EXIT FAILSAFE)
        // If rover was in emergency mode and receives signal, reactivate control.
        if (_failsafeActive)
        {
            _failsafeActive = false;
            // Invalidate cache (_prev) to force immediate hardware update
            // even if new values match old ones.
            _prevSpeed = 255;
            _prevAngle = 255;
            Serial.println("[UDP] Signal recovered. Control reactivated.");
        }

        // --- BYTE 0: TRACTION (Throttle) ---
        uint8_t speedCode = _packetBuffer[0];

        // CACHE OPTIMIZATION: Write to motor only if value changed.
        // Saves CPU cycles and unnecessary PWM bus calls.
        if (speedCode != _prevSpeed)
        {
            _prevSpeed = speedCode; // Update cache

            if (speedCode == 0)
            {
                _motors->coast(); // 0 = Inertia (Release throttle)
            }
            else if (speedCode == 1)
            {
                _motors->brake(); // 1 = Active Brake
            }
            else
            {
                // Values 2-255 map directly to PWM.
                // SolidAxle internally manages pin direction.
                _motors->drive((int)speedCode);
            }
        }

        // --- BYTE 1: STEERING ---
        uint8_t angle = _packetBuffer[1];

        // CACHE OPTIMIZATION: Write to servo only if angle changed.
        if (angle != _prevAngle)
        {
            _prevAngle = angle; // Update cache

            // Pass raw angle. SteeringServo class internally handles
            // 'constrain' and physical limits.
            _steering->write((int)angle);
        }
    }
}

void RemoteControl::checkFailsafe()
{
    // Check only if system is NOT already in failure state.
    if (!_failsafeActive)
    {
        // If more time than allowed has passed without UDP packets...
        if (millis() - _lastPacketTime > UDP_FAILSAFE_MS)
        {
            // ...ACTIVATE EMERGENCY STOP PROTOCOL.
            Serial.println("[FAILSAFE] Signal Lost (Timeout). EMERGENCY STOP.");

            // Immediate physical actions
            _motors->brake();    // Hard brake
            _steering->center(); // Center steering

            // Mark state active to avoid repeating these calls
            // in every loop cycle (resource saving).
            _failsafeActive = true;
        }
    }
}
