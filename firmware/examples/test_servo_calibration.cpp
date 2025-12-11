/**
 * @file test_servo_calibration.cpp
 * @brief Test de Calibración de Dirección (Paso B).
 * @author Alejandro Moyano (@AleSMC)
 * * @details
 * Script de validación para el servo de dirección (Ackermann).
 * Mueve el mecanismo cíclicamente: Centro -> Izquierda -> Centro -> Derecha.
 * * @note
 * Objetivo: Ajustar visualmente los límites STEERING_LEFT_MAX y STEERING_RIGHT_MAX
 * en 'config.h' para obtener el máximo giro sin forzar el mecanismo.
 * * =================================================================================
 * @section execution Procedimiento de Despliegue (CLI)
 * =================================================================================
 * * 1. PREPARACIÓN DE HARDWARE:
 * - Desconectar el cable USB.
 * - Asegurar alimentación de batería (El servo no funciona solo con USB).
 * - IMPORTANTE: El GPIO 2 comparte línea con el LED Flash. Es normal que parpadee.
 * - Volver a conectar el cable USB para la carga.
 * * 2. PREPARACIÓN DE SOFTWARE:
 * - Este código debe estar en 'firmware/src/main.cpp'.
 * - Configurar límites iniciales conservadores en 'config.h'.
 * * 3. COMANDOS DE TERMINAL (Desde la raíz del proyecto):
 * $ cd firmware
 * $ pio run -t upload
 * $ pio device monitor -b 115200
 * * 4. VERIFICACIÓN Y CALIBRACIÓN:
 * - Observar el giro de las ruedas.
 * - Si falta giro: Aumentar/Disminuir ángulo en 'config.h' (5 grados cada vez).
 * - Si el servo hace ruido (zumbido) en los topes: RETROCEDER 5-10 grados inmediatamente.
 * * 5. PRESERVACIÓN:
 * - Una vez calibrado, mover este código a 'firmware/examples/test_servo_calibration.cpp'.
 * =================================================================================
 */

#include <Arduino.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "config.h"
#include "SteeringServo.h"

// Instanciamos el servo con los límites definidos en config.h
SteeringServo steering(PIN_SERVO, STEERING_CENTER, STEERING_LEFT_MAX, STEERING_RIGHT_MAX);

void setup()
{
    // 1. Gestión de Energía
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

    // 2. Serial
    Serial.begin(115200);
    Serial.println("\n[BOOT] Test de Servo Iniciado");

    // 3. Inicio del Servo
    steering.begin();
    Serial.println("[INFO] Servo inicializado y centrado.");

    // Espera inicial para observar el centro
    delay(2000);
}

void loop()
{
    // --- 1. CENTRO ---
    Serial.printf("[TEST] Recto (Angulo: %d)\n", STEERING_CENTER);
    steering.center();
    delay(2000);

    // --- 2. IZQUIERDA ---
    Serial.printf("[TEST] Izquierda (Angulo: %d)\n", STEERING_LEFT_MAX);
    steering.turnLeft();
    delay(2000);

    // --- 3. CENTRO ---
    Serial.println("[TEST] Recto...");
    steering.center();
    delay(1000);

    // --- 4. DERECHA ---
    Serial.printf("[TEST] Derecha (Angulo: %d)\n", STEERING_RIGHT_MAX);
    steering.turnRight();
    delay(2000);
}