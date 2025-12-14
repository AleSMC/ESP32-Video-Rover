/**
 * @file SolidAxle.cpp
 * @brief Implementación del driver de motores L298N para ESP32.
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
    // 1. Configuración de Pines (Salida Digital)
    pinMode(_pinFwd, OUTPUT);
    pinMode(_pinRev, OUTPUT);
    pinMode(_pinPWM, OUTPUT);

    // 2. Configuración del Periférico PWM (LEDC)
    // El ESP32 no usa analogWrite(), usa el controlador LEDC hardware.
    ledcSetup(_pwmChannel, _pwmFreq, _pwmResolution);
    ledcAttachPin(_pinPWM, _pwmChannel);

    // 3. Estado Inicial Seguro
    brake();
}

void SolidAxle::brake()
{
    // Lógica L298N: IN1=LOW, IN2=LOW, ENA=HIGH -> Freno Corto (Short Brake)
    digitalWrite(_pinFwd, LOW);
    digitalWrite(_pinRev, LOW);
    ledcWrite(_pwmChannel, 255);
    _velocidadActual = 0;
}

void SolidAxle::coast()
{
    // Lógica L298N: ENA=LOW -> Motor Deshabilitado (Free Run)
    digitalWrite(_pinFwd, LOW);
    digitalWrite(_pinRev, LOW);
    ledcWrite(_pwmChannel, 0);
    _velocidadActual = 0;
}

void SolidAxle::drive(int velocidad)
{
    // --- 1. VALIDACIÓN DE INTEGRIDAD ---
    if (velocidad > 255 || velocidad < -255)
    {
        Serial.printf("[ERROR] Motor: Velocidad %d fuera de rango. Ignorado.\n", velocidad);
        return;
    }

    // --- 2. PROTECCIÓN DE HARDWARE (BLOQUEO DE REVERSA) ---
    // @warning La inversión brusca de marcha genera corrientes de retorno (Back-EMF)
    // que pueden quemar el driver L298N o reiniciar el ESP32.
    // En esta fase, bloqueamos la reversa hasta implementar "Dynamic Dead Time" en el cliente.
    if (velocidad < 0)
    {
        // Comentar if() solo bajo tu propia responsabilidad y con lógica de parada previa.
        Serial.printf("[WARN] Reversa solicitada. Bloqueada por seguridad.\n");
        brake();
        return;
    }

    // --- 3. ZONA MUERTA (Deadzone) ---
    // Los motores DC baratos no tienen torque suficiente para moverse con PWM muy bajo.
    // Cortamos la señal para evitar zumbidos eléctricos sin movimiento.
    if (abs(velocidad) < 15)
    {
        coast();
        return;
    }

    // --- 4. APLICACIÓN DE POTENCIA ---
    if (velocidad > 0)
    {
        // Configuración Avance: IN1=HIGH, IN2=LOW
        digitalWrite(_pinFwd, HIGH);
        digitalWrite(_pinRev, LOW);
    }
    else if (velocidad < 0)
    {
        // Configuración Retroceso: IN1=LOW, IN2=HIGH
        digitalWrite(_pinFwd, LOW);
        digitalWrite(_pinRev, HIGH);
    }

    // El PWM siempre es positivo (magnitud del vector velocidad)
    ledcWrite(_pwmChannel, abs(velocidad));
    _velocidadActual = velocidad;
}