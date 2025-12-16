/**
 * @file RemoteControl.cpp
 * @brief Implementación del Protocolo de Control UDP (Baja Latencia).
 * @author Alejandro Moyano (@AleSMC)
 * @details
 * Implementa un protocolo binario de 2 bytes para control de tracción y dirección.
 * Incluye mecanismos de seguridad (Failsafe) y optimización de CPU (Caché de estado)
 * para evitar escrituras redundantes en los drivers PWM.
 */

#include "RemoteControl.h"

RemoteControl::RemoteControl(SolidAxle *motors, SteeringServo *steering)
{
    // Inyección de dependencias hardware
    _motors = motors;
    _steering = steering;

    _lastPacketTime = 0;
    _failsafeActive = false;

    // Inicializamos la caché con valores fuera de rango (255) para
    // forzar la actualización física del hardware con el primer paquete recibido.
    _prevSpeed = 255;
    _prevAngle = 255;
}

void RemoteControl::begin()
{
    _udp.begin(UDP_PORT); // Puerto definido en config.h (9999)
    Serial.printf("[UDP] Escuchando protocolo binario en puerto %d\n", UDP_PORT);
}

void RemoteControl::listen()
{
    int packetSize = _udp.parsePacket();

    // Filtro básico: Solo procesamos si el paquete tiene el tamaño del protocolo (2 bytes)
    // Byte 0: Tracción | Byte 1: Dirección
    if (packetSize >= 2)
    {
        _udp.read(_packetBuffer, 2); // Leemos solo los 2 bytes relevantes

        // DEBUG: Mostrar datos recibidos
        // Serial.printf("[UDP] Motor: %d | Servo: %d\n", _packetBuffer[0], _packetBuffer[1]);

        // 1. WATCHDOG RESET
        // Hemos recibido señal de vida del controlador, reseteamos el temporizador.
        _lastPacketTime = millis();

        // 2. GESTIÓN DE RECUPERACIÓN (SALIDA DE FAILSAFE)
        // Si el robot estaba en modo emergencia y recibe señal, reactivamos el control.
        if (_failsafeActive)
        {
            _failsafeActive = false;
            // Invalidamos la caché (_prev) para obligar a que los nuevos valores
            // se apliquen inmediatamente al hardware, aunque coincidan con los viejos.
            _prevSpeed = 255;
            _prevAngle = 255;
            Serial.println("[UDP] Señal recuperada. Control reactivado.");
        }

        // --- BYTE 0: TRACCIÓN (Throttle) ---
        uint8_t speedCode = _packetBuffer[0];

        // OPTIMIZACIÓN DE CACHÉ: Solo escribir al motor si el valor ha cambiado.
        // Esto ahorra ciclos de CPU y llamadas innecesarias al bus PWM.
        if (speedCode != _prevSpeed)
        {
            _prevSpeed = speedCode; // Actualizar caché

            if (speedCode == 0)
            {
                _motors->coast(); // 0 = Inercia (Soltar acelerador)
            }
            else if (speedCode == 1)
            {
                _motors->brake(); // 1 = Freno Activo (Pisar freno)
            }
            else
            {
                // Valores 2-255 se mapean directamente a PWM.
                // SolidAxle gestiona internamente la dirección de los pines.
                _motors->drive((int)speedCode);
            }
        }

        // --- BYTE 1: DIRECCIÓN (Steering) ---
        uint8_t angle = _packetBuffer[1];

        // OPTIMIZACIÓN DE CACHÉ: Solo escribir al servo si el ángulo ha cambiado.
        if (angle != _prevAngle)
        {
            _prevAngle = angle; // Actualizar caché

            // Pasamos el ángulo crudo. La clase SteeringServo se encarga
            // internamente de aplicar 'constrain' y proteger los topes físicos.
            _steering->write((int)angle);
        }
    }
}

void RemoteControl::checkFailsafe()
{
    // Solo verificamos si el sistema NO está ya en estado de fallo.
    if (!_failsafeActive)
    {
        // Si ha pasado más tiempo del permitido sin recibir paquetes UDP...
        if (millis() - _lastPacketTime > UDP_FAILSAFE_MS)
        {
            // ...ACTIVAR PROTOCOLO DE PARADA DE EMERGENCIA.
            Serial.println("[FAILSAFE] Pérdida de señal (Timeout). PARADA DE EMERGENCIA.");

            // Acciones físicas inmediatas
            _motors->brake();    // Clavar frenos
            _steering->center(); // Centrar dirección

            // Marcar estado activo para evitar repetir estas llamadas
            // en cada ciclo del loop (ahorro de recursos).
            _failsafeActive = true;
        }
    }
}