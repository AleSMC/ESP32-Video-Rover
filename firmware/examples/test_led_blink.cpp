/**
 * @file test_led_blink.cpp
 * @brief Verificación de integridad del sistema y toolchain.
 * @details Módulo de prueba aislado para validar:
 * 1. Correcta compilación y carga del firmware (Toolchain).
 * 2. Arranque del SoC ESP32 (Boot sequence).
 * 3. Control GPIO básico (parpadeo de LED de estado).
 * * @note Hardware: LED integrado (Generalmente GPIO 33 o GPIO 4).
 * @note Lógica: Lógica negativa (Active LOW) común en módulos AI-Thinker.
 */

#include <Arduino.h>

// Definición del Pin del LED integrado.
// GPIO 33 es el estándar para la placa AI-Thinker ESP32-CAM.
#define STATUS_LED_PIN 33

/**
 * @brief Inicialización del sistema.
 */
void setup()
{
    // Inicialización del bus serie para salida de logs de depuración
    Serial.begin(115200);

    // Configuración del GPIO como salida digital
    pinMode(STATUS_LED_PIN, OUTPUT);

    // Retardo de estabilización para el monitor serie
    delay(1000);

    // Logs de inicio del sistema
    Serial.println("\n--- SYSTEM DIAGNOSTIC: LED BLINK TEST ---");
    Serial.println("[INFO] Boot: SUCCESS");
    Serial.println("[INFO] Board: ESP32-CAM AI-Thinker");
    Serial.println("[INFO] Status: Ready");
}

/**
 * @brief Bucle principal (Heartbeat).
 */
void loop()
{
    // Emisión de señal de vida (Heartbeat) al puerto serie
    Serial.println("[STATUS] Heartbeat: System Active");

    // Ciclo de indicación visual (Active LOW)
    // Estado ON (Cierre a tierra)
    digitalWrite(STATUS_LED_PIN, LOW);
    delay(100); // Duración del pulso: 100ms

    // Estado OFF (Alta impedancia/VCC)
    digitalWrite(STATUS_LED_PIN, HIGH);
    delay(2000); // Frecuencia de ciclo: ~0.5Hz
}