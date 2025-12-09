/**
 * @file SolidAxle.h
 * @brief Controlador de tracción unificada (Topología Eje Sólido) para ESP32.
 * @details Gestiona la velocidad (PWM global) y el sentido de giro unificado de los motores,
 * incluyendo lógica de seguridad "Dead Time" para proteger el driver contra picos de corriente
 * por inversión de marcha (Back-EMF).
 * @board ESP32-CAM (AI Thinker)
 * @author Alejandro Moyano (@AleSMC)
 */

#pragma once
#include <Arduino.h>

class SolidAxle
{
private:
    // --- Pines de Hardware ---
    int _pinFwd; ///< Pin para marcha adelante (IN1+IN3)
    int _pinRev; ///< Pin para marcha atrás (IN2+IN4)
    int _pinPWM; ///< Pin para velocidad global (ENA+ENB)

    // --- Estado Interno ---
    int _velocidadActual; ///< Estado actual (-255 a 255)

    // --- Configuración PWM (ESP32 LEDC) ---
    const int _pwmFreq = 1000;    ///< Frecuencia base 1kHz
    const int _pwmChannel = 0;    ///< Canal 0 del temporizador
    const int _pwmResolution = 8; ///< Resolución de 8 bits (0-255)

public:
    /**
     * @brief Constructor del sistema de tracción.
     * @param pinFwd Pin GPIO conectado a la dirección Adelante.
     * @param pinRev Pin GPIO conectado a la dirección Atrás.
     * @param pinPWM Pin GPIO conectado a la habilitación (Enable/PWM).
     */
    SolidAxle(int pinFwd, int pinRev, int pinPWM);

    /**
     * @brief Inicializa los periféricos y configura el PWM.
     * @note Debe llamarse dentro del setup() de Arduino.
     */
    void begin();

    /**
     * @brief Controla el movimiento del vehículo con seguridad.
     * @param velocidad Valor con signo:
     * - Positivo (1 a 255): Marcha Adelante.
     * - Negativo (-1 a -255): Marcha Atrás.
     * - Cero (0): Activa modo Coasting (Inercia).
     * @warning Si se envía un valor fuera de rango [-255, 255], se registra un ERROR en consola y el comando se ignora.
     */
    void drive(int velocidad);

    /**
     * @brief Freno Magnético (Short Brake).
     * @details Conecta ambos terminales del motor a GND, deteniendo el giro bruscamente.
     */
    void brake();

    /**
     * @brief Inercia (Coasting).
     * @details Desconecta el puente H (Alta impedancia). El motor gira libremente.
     */
    void coast();
};