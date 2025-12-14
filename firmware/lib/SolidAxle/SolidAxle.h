/**
 * @file SolidAxle.h
 * @brief Controlador de Tracción Unificada (Topología Eje Sólido).
 * @author Alejandro Moyano (@AleSMC)
 * @version 1.0.0
 * @details
 * Gestiona dos motores DC conectados en paralelo (mismo PWM, misma Dirección).
 * Abstrae la lógica del puente H (L298N) y provee métodos de seguridad para
 * evitar picos de corriente inductiva (Back-EMF).
 */

#pragma once
#include <Arduino.h>

class SolidAxle
{
private:
    // --- Pines de Hardware (Configuración L298N) ---
    int _pinFwd; ///< Pin lógico para activar puente H en sentido horario
    int _pinRev; ///< Pin lógico para activar puente H en sentido anti-horario
    int _pinPWM; ///< Pin de Habilitación (Enable) para modulación de ancho de pulso

    // --- Estado Interno ---
    int _velocidadActual; ///< Última velocidad comandada (-255 a 255)

    // --- Configuración PWM (ESP32 LEDC) ---
    const int _pwmFreq = 1000;    ///< Frecuencia 1kHz (Óptima para motores DC genéricos)
    const int _pwmChannel = 0;    ///< Canal PWM 0
    const int _pwmResolution = 8; ///< Resolución 8 bits (Rango 0-255)

public:
    /**
     * @brief Constructor del driver.
     * @param pinFwd GPIO conectado a IN1+IN3.
     * @param pinRev GPIO conectado a IN2+IN4.
     * @param pinPWM GPIO conectado a ENA+ENB.
     */
    SolidAxle(int pinFwd, int pinRev, int pinPWM);

    /**
     * @brief Inicializa los pines GPIO y el periférico LEDC (PWM).
     * @note Estado inicial: Freno activado.
     */
    void begin();

    /**
     * @brief Comando principal de movimiento.
     * @param velocidad Valor con signo [-255 a 255].
     * - Positivo: Avance.
     * - Negativo: Retroceso (Bloqueado por defecto en Fase A).
     * - 0: Coast (Inercia).
     * @warning Incluye protección de rango. Valores >255 son ignorados.
     */
    void drive(int velocidad);

    /**
     * @brief Freno Magnético (Short Brake).
     * @details Pone los pines de control en LOW y el PWM al máximo.
     * Esto cortocircuita las bobinas del motor, generando una fuerza contraelectromotriz
     * que detiene el eje rápidamente.
     */
    void brake();

    /**
     * @brief Modo Inercia (Coasting).
     * @details Deshabilita el puente H (Alta Impedancia).
     * El motor queda desconectado eléctricamente y gira libremente por inercia.
     */
    void coast();
};