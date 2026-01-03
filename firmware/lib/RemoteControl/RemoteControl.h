/**
 * @file RemoteControl.h
 * @brief Binary UDP Command Manager (2-Byte Protocol).
 * @author Alejandro Moyano (@AleSMC)
 * @details
 * Class responsible for listening to the UDP port, decoding control packets,
 * and orchestrating physical actuators (Motors and Servo).
 * Implements safety (Failsafe) and efficiency (State Cache).
 */

#pragma once
#include <Arduino.h>
#include <WiFiUdp.h>
#include "config.h"
#include "SolidAxle.h"
#include "SteeringServo.h"

class RemoteControl
{
private:
    WiFiUDP _udp;                  ///< UDP socket instance
    uint8_t _packetBuffer[2];      ///< Reception buffer (Byte 0: Traction, Byte 1: Steering)
    unsigned long _lastPacketTime; ///< Timestamp of the last valid packet (ms)
    bool _failsafeActive;          ///< Flag: true if the robot is in emergency stop

    // --- STATE CACHE (OPTIMIZATION) ---
    // We store the last applied command to avoid saturating the bus
    // by repeatedly sending the same PWM instruction.
    uint8_t _prevSpeed; ///< Last speed code sent to motor (0-255)
    uint8_t _prevAngle; ///< Last angle sent to servo (0-180)

    // --- DEPENDENCIES (Hardware Pointers) ---
    SolidAxle *_motors;       ///< Traction Driver
    SteeringServo *_steering; ///< Steering Driver

public:
    /**
     * @brief Constructor with Dependency Injection.
     * @param motors Pointer to the initialized motor controller object.
     * @param steering Pointer to the initialized servo controller object.
     */
    RemoteControl(SolidAxle *motors, SteeringServo *steering);

    /**
     * @brief Opens the UDP port and starts listening.
     */
    void begin();

    /**
     * @brief Processes the incoming UDP packet queue.
     * @details
     * Binary Protocol:
     * - Byte[0]: 0=Coast, 1=Brake, 2-255=PWM Speed.
     * - Byte[1]: 0-180=Servo Angle.
     * @note Must be called in every loop() iteration.
     */
    void listen();

    /**
     * @brief Safety Monitor (Watchdog).
     * @details If no valid packets are received within UDP_FAILSAFE_MS (config.h),
     * stops motors and centers steering to prevent accidents.
     */
    void checkFailsafe();
};
