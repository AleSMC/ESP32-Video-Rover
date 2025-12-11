/**
 * @file SteeringServo.h
 * @brief Controlador de dirección para servo (Ackermann).
 * @details Encapsula la librería ESP32Servo y gestiona los límites físicos
 * definidos en la configuración para evitar daños mecánicos.
 * @author Alejandro Moyano (@AleSMC)
 */

#pragma once
#include <Arduino.h>
#include <ESP32Servo.h>

class SteeringServo
{
private:
    Servo _servo;     ///< Objeto servo de bajo nivel
    int _pin;         ///< Pin GPIO
    int _angleCenter; ///< Ángulo central calibrado
    int _angleLeft;   ///< Límite izquierdo calibrado
    int _angleRight;  ///< Límite derecho calibrado

public:
    /**
     * @brief Constructor.
     * @param pin Pin GPIO del servo.
     * @param center Ángulo para ir recto (aprox 90).
     * @param leftMax Ángulo máximo izquierda.
     * @param rightMax Ángulo máximo derecha.
     */
    SteeringServo(int pin, int center, int leftMax, int rightMax);

    /**
     * @brief Inicializa el servo y lo coloca en el centro.
     */
    void begin();

    /**
     * @brief Gira las ruedas al centro (Recto).
     */
    void center();

    /**
     * @brief Gira todo lo posible a la izquierda (según límite).
     */
    void turnLeft();

    /**
     * @brief Gira todo lo posible a la derecha (según límite).
     */
    void turnRight();

    /**
     * @brief Control fino (opcional).
     * @param angle Ángulo absoluto (0-180). Se recorta a los límites seguros.
     */
    void write(int angle);
};