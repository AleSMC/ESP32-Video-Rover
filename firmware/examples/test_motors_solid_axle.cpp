/**
 * @file test_motors_solid_axle.cpp
 * @brief Test Unitario de Tracción (Paso A) - Validación de Hardware.
 * @author Alejandro Moyano (@AleSMC)
 * * @details
 * Script de validación para la topología "Solid Axle" (Eje Sólido).
 * Ejecuta una rutina cíclica: Aceleración (Rampa) -> Inercia (Coast) -> Freno (Brake).
 * * @note
 * Este test verifica también que el firmware rechace comandos de reversa (seguridad).
 * * =================================================================================
 * @section execution Procedimiento de Despliegue (CLI)
 * =================================================================================
 * * 1. PREPARACIÓN DE HARDWARE (CRÍTICO):
 * - Desconectar el cable USB.
 * - Levantar el chasis (las ruedas NO deben tocar el suelo).
 * - Conectar la batería LiPo al driver L298N.
 * - Volver a conectar el cable USB para la carga.
 * * 2. PREPARACIÓN DE SOFTWARE:
 * - Copiar el contenido íntegro de este archivo.
 * - Pegarlo en 'firmware/src/main.cpp' (sobrescribiendo el contenido actual).
 * * 3. COMANDOS DE TERMINAL (Desde la raíz del proyecto):
 * $ cd firmware
 * $ pio run -t upload
 * $ pio device monitor -b 115200
 * * 4. VERIFICACIÓN:
 * - Observar el comportamiento físico de las ruedas.
 * - Verificar que los logs en el monitor coinciden con las acciones.
 * =================================================================================
 */

#include <Arduino.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "config.h"    // Definiciones de pines (Netlist)
#include "SolidAxle.h" // Librería de abstracción

// Instancia global del sistema de tracción
SolidAxle rover(PIN_MOTOR_FWD, PIN_MOTOR_REV, PIN_MOTOR_PWM);

void setup()
{
    // 1. Gestión de Energía: Deshabilitar Brownout Detector
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

    // 2. Iniciar puerto serie
    Serial.begin(115200);
    Serial.println("\n[BOOT] Sistema Rover Iniciado (Modo: SolidAxle)");

    // 3. Iniciar subsistema de tracción
    rover.begin();
}

void loop()
{
    Serial.println("[INFO] Inicio de Test de Motores en 10 segundos...");
    Serial.println("[INFO] Asegurese de que las ruedas no tocan el suelo.");
    for (int i = 10; i > 0; i--)
    {
        Serial.printf("...%d\n", i);
        delay(1000);
    }

    // --- TEST 1: ACELERACIÓN PROGRESIVA (Rampa) ---
    Serial.println("[TEST] 1. Acelerando suavemente (Adelante)...");
    for (int i = 0; i <= 255; i += 5)
    {
        rover.drive(i);
        delay(20); // Rampa suave
    }
    delay(1000); // Mantener velocidad máxima

    // --- TEST 2: INERCIA (Coasting) ---
    Serial.println("[TEST] 2. Modo Coast (Dejar rodar por inercia)...");
    rover.coast();
    delay(2000);

    // --- TEST 3: ACELERACIÓN BRUSCA (Prueba de Seguridad) ---
    Serial.println("[TEST] 3. Prueba de Aceleracion...");
    rover.drive(150); // Velocidad media adelante
    delay(3000);

    // --- TEST 4: FRENADO TOTAL ---
    Serial.println("[TEST] 4. Freno Seco (Brake)...");
    rover.brake();
    delay(3000);

    // --- TEST 5: PRUEBA DE ERROR HANDLING ---
    Serial.println("[TEST] 5. Prueba de Error: Intentando enviar valor invalido (500).");
    rover.drive(500);
    delay(1000);

    // --- TEST 6: PRUEBA DE ERROR REVERSA ---
    Serial.println("[TEST] 6. Prueba de Error: Intentando enviar valor negativo (-200).");
    rover.drive(-200);
    delay(1000);

    Serial.println("[TEST] 7. Final de ciclo.");
    delay(1000);
}