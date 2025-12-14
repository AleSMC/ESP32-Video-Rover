/**
 * @file RemoteControl.h
 * @brief Gestor de Comandos UDP Binarios (Protocolo de 2 Bytes).
 * @author Alejandro Moyano (@AleSMC)
 * @details
 * Clase encargada de escuchar el puerto UDP, decodificar los paquetes de control
 * y orquestar los actuadores físicos (Motores y Servo).
 * Implementa seguridad (Failsafe) y eficiencia (Caché de estado).
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
    WiFiUDP _udp;                  ///< Instancia del socket UDP
    uint8_t _packetBuffer[2];      ///< Buffer de recepción (Byte 0: Tracción, Byte 1: Dirección)
    unsigned long _lastPacketTime; ///< Marca de tiempo del último paquete válido (ms)
    bool _failsafeActive;          ///< Bandera: true si el robot está en parada de emergencia

    // --- CACHÉ DE ESTADO (OPTIMIZACIÓN) ---
    // Guardamos el último comando aplicado para no saturar el bus
    // enviando la misma instrucción PWM repetidamente.
    uint8_t _prevSpeed; ///< Último código de velocidad enviado al motor (0-255)
    uint8_t _prevAngle; ///< Último ángulo enviado al servo (0-180)

    // --- DEPENDENCIAS (Punteros a Hardware) ---
    SolidAxle *_motors;       ///< Driver de tracción
    SteeringServo *_steering; ///< Driver de dirección

public:
    /**
     * @brief Constructor con Inyección de Dependencias.
     * @param motors Puntero al objeto controlador de motores ya inicializado.
     * @param steering Puntero al objeto controlador de servo ya inicializado.
     */
    RemoteControl(SolidAxle *motors, SteeringServo *steering);

    /**
     * @brief Abre el puerto UDP e inicia la escucha.
     */
    void begin();

    /**
     * @brief Procesa la cola de paquetes UDP entrantes.
     * @details
     * Protocolo Binario:
     * - Byte[0]: 0=Coast, 1=Brake, 2-255=PWM Velocidad.
     * - Byte[1]: 0-180=Ángulo Servo.
     * @note Debe llamarse en cada iteración del loop().
     */
    void listen();

    /**
     * @brief Monitor de Seguridad (Watchdog).
     * @details Si no se reciben paquetes válidos en UDP_FAILSAFE_MS (config.h),
     * detiene los motores y centra la dirección para evitar accidentes.
     */
    void checkFailsafe();
};