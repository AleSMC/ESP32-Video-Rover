/**
 * @file SteeringServo.cpp
 * @brief Implementación del control de servo seguro.
 * @author Alejandro Moyano (@AleSMC)
 */

#include "SteeringServo.h"

SteeringServo::SteeringServo(int pin, int center, int leftMax, int rightMax)
{
    // Calculamos los límites absolutos independientemente de si Izquierda < Derecha o viceversa.
    // Esto hace el código robusto ante diferentes montajes mecánicos.
    _minLimit = min(leftMax, rightMax);
    _maxLimit = max(leftMax, rightMax);

    _pin = pin;
    _angleCenter = center;
    _angleLeft = leftMax;
    _angleRight = rightMax;
}

void SteeringServo::begin()
{
    // 1. Configurar Periodo (Frecuencia)
    // Servos analógicos estándar (SG90, MG996R) funcionan a 50Hz (20ms).
    // @warning Usar frecuencias mayores (>60Hz) puede sobrecalentar o quemar servos analógicos.
    _servo.setPeriodHertz(50);

    // 2. Configurar Anchos de Pulso (Pulse Width)
    // Definimos la correspondencia entre señal eléctrica (microsegundos) y giro físico.
    // - Estándar Teórico RC: 1000us (0°) a 2000us (180°).
    // - Rango Extendido (Hardware Real): 500us (0°) a 2400us (180°).
    // Usamos este rango amplio para asegurar que servos económicos alcancen su recorrido completo.
    _servo.attach(_pin, 500, 2400);

    // 3. Posición Inicial
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
    // --- CAPA DE SEGURIDAD (HARDWARE PROTECTION) ---
    // Función constrain(val, min, max):
    // Si val < min -> devuelve min.
    // Si val > max -> devuelve max.
    // Esto impide físicamente que el servo reciba una orden que rompa la dirección.
    // Si fuera al revés, la lógica de constrain cambia.
    int safeAngle = constrain(angle, _minLimit, _maxLimit);

    _servo.write(safeAngle);
}