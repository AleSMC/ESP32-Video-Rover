/**
 * @file SolidAxle.cpp
 * @brief Implementación de la lógica de control de tracción SolidAxle (Fwd-Only).
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
    // 1. Configurar pines digitales como salida
    pinMode(_pinFwd, OUTPUT);
    pinMode(_pinRev, OUTPUT);
    pinMode(_pinPWM, OUTPUT);

    // 2. Configurar periférico PWM (LEDC para ESP32)
    ledcSetup(_pwmChannel, _pwmFreq, _pwmResolution);
    ledcAttachPin(_pinPWM, _pwmChannel);

    // 3. Estado inicial seguro (Frenado activo)
    brake();
}

void SolidAxle::brake()
{
    digitalWrite(_pinFwd, LOW);
    digitalWrite(_pinRev, LOW);
    ledcWrite(_pwmChannel, 255); // Enable ON + Inputs LOW = Freno Magnético
    _velocidadActual = 0;
}

void SolidAxle::coast()
{
    digitalWrite(_pinFwd, LOW);
    digitalWrite(_pinRev, LOW);
    ledcWrite(_pwmChannel, 0); // Enable OFF = Alta Impedancia (Motor desconectado)
    _velocidadActual = 0;
}

void SolidAxle::drive(int velocidad)
{
    // --- 1. VALIDACIÓN DE RANGO ---
    if (velocidad > 255 || velocidad < -255)
    {
        Serial.printf("[ERROR] SolidAxle: Velocidad (%d) fuera de rango [-255, 255]. Comando ignorado.\n", velocidad);
        return;
    }

    // --- 2. RESTRICCIÓN DE SEGURIDAD (BLOQUEO DE REVERSA) ---
    // @warning INSTRUCCIONES PARA ACTIVAR REVERSA:
    // Para habilitar la marcha atrás, comenta el siguiente bloque 'if'.
    // ¡PELIGRO!: Al hacerlo, eliminas la protección del firmware. Debes garantizar
    // desde el software cliente (PC) que el coche se detiene completamente antes
    // de invertir el sentido, o dañarás el driver por picos de corriente (Back-EMF).
    if (velocidad < 0)
    {
        Serial.printf("[ERROR] SolidAxle: Velocidad (%d). Reversa bloqueada por seguridad. Comenta este bloque para activar.\n", velocidad);
        brake();
        return;
    }

    // --- 3. ZONA MUERTA / COAST ---
    if (abs(velocidad) < 15)
    {
        coast();
        return;
    }

    // --- 4. CONTROL DE DIRECCIÓN Y POTENCIA ---
    if (velocidad > 0)
    {
        // Marcha Adelante
        digitalWrite(_pinFwd, HIGH);
        digitalWrite(_pinRev, LOW);
    }
    else if (velocidad < 0) // Este código se activará solo si comentas el Bloque 2
    {
        // Marcha Atrás
        digitalWrite(_pinFwd, LOW);
        digitalWrite(_pinRev, HIGH);
    }

    // Aplicamos el valor absoluto al PWM
    ledcWrite(_pwmChannel, abs(velocidad));

    // Actualizamos el registro de estado
    _velocidadActual = velocidad;
}