/**
 * @file SteeringServo.h
 * @brief Controlador de Dirección Ackermann (Servo).
 * @author Alejandro Moyano (@AleSMC)
 * @version 1.0.0
 * @details
 * Wrapper sobre la librería ESP32Servo que añade una capa de seguridad (Hard Limits).
 * Traduce comandos lógicos (0-180 grados) a señales PWM protegidas físicamente.
 */

#pragma once
#include <Arduino.h>
#include <ESP32Servo.h>

class SteeringServo
{
private:
    Servo _servo; ///< Instancia del driver de bajo nivel
    int _pin;     ///< Pin GPIO de señal PWM

    // --- Parámetros de Calibración ---
    int _angleCenter; ///< Valor calibrado para ir recto
    int _angleLeft;   ///< Límite físico izquierdo
    int _angleRight;  ///< Límite físico derecho

    // --- Límites de Seguridad (Calculados) ---
    int _minLimit; ///< El valor numérico más bajo permitido (ej: 70)
    int _maxLimit; ///< El valor numérico más alto permitido (ej: 110)

public:
    /**
     * @brief Constructor con límites físicos.
     * @param pin GPIO del servo.
     * @param center Ángulo central (idealmente 90).
     * @param leftMax Ángulo máximo hacia la izquierda.
     * @param rightMax Ángulo máximo hacia la derecha.
     */
    SteeringServo(int pin, int center, int leftMax, int rightMax);

    /**
     * @brief Inicializa el PWM del servo a 50Hz.
     */
    void begin();

    /**
     * @brief Coloca las ruedas rectas.
     */
    void center();

    /**
     * @brief Gira al tope izquierdo permitido.
     */
    void turnLeft();

    /**
     * @brief Gira al tope derecho permitido.
     */
    void turnRight();

    /**
     * @brief Mueve el servo a un ángulo específico con seguridad.
     * @param angle Ángulo objetivo (0-180).
     * @note Si el ángulo excede los límites configurados, se recorta (clamping)
     * automáticamente para no forzar el mecanismo.
     */
    void write(int angle);
};