/**
 * @file SteeringServo.cpp
 * @brief Implementación del controlador de dirección.
 * @author Alejandro Moyano (@AleSMC)
 */

#include "SteeringServo.h"

SteeringServo::SteeringServo(int pin, int center, int leftMax, int rightMax)
{
    _pin = pin;
    _angleCenter = center;
    _angleLeft = leftMax;
    _angleRight = rightMax;
}

void SteeringServo::begin()
{
    // 1. Configurar Frecuencia PWM (Periodo)
    // Los servos analógicos (SG90/MG90S) esperan un pulso cada 20ms.
    // Frecuencia = 1 / 0.020s = 50Hz.
    // @warning Usar frecuencias mayores (>60Hz) puede sobrecalentar o quemar servos analógicos.
    _servo.setPeriodHertz(50);

    // 2. Asignar Pin y Rango de Pulso (Pulse Width)
    // Definimos la correspondencia entre señal eléctrica (microsegundos) y giro físico.
    // - Estándar Teórico RC: 1000us (0°) a 2000us (180°).
    // - Rango Extendido (Hardware Real): 500us a 2400us.
    //   Se usa este rango ampliado para garantizar que alcanzamos los topes mecánicos
    //   reales (0° y 180°) de servos económicos que no son precisos con el estándar teórico.
    _servo.attach(_pin, 500, 2400);

    // 3. Posición inicial segura
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
    // Saturación de seguridad: Nunca exceder los límites configurados
    // Asumimos que Left < Right. Si fuera al revés, la lógica de constrain cambia.
    // Para simplificar, forzamos el valor entre el menor y el mayor de los límites.

    int minLimit = min(_angleLeft, _angleRight);
    int maxLimit = max(_angleLeft, _angleRight);

    int safeAngle = constrain(angle, minLimit, maxLimit);
    _servo.write(safeAngle);
}