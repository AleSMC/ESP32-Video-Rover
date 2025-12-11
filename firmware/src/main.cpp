/**
 * @file main.cpp
 * @brief Archivo principal limpio y preparado para el Paso C (Stack de Red).
 * @details Este archivo actúa como punto de entrada limpio. Los tests de hardware
 * (Motores y Servo) se han preservado en la carpeta 'examples/'.
 * @author Alejandro Moyano (@AleSMC)
 */

#include <Arduino.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "config.h"

// Nota: Las instancias de 'SolidAxle' y 'SteeringServo' se añadirán
// e integrarán cuando tengamos el stack de control (UDP) listo.

void setup()
{
    // 1. Gestión de Energía: Deshabilitar Brownout Detector (CRÍTICO)
    // Evita reinicios inesperados cuando el WiFi y los motores arranquen juntos.
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

    // 2. Iniciar puerto serie
    Serial.begin(115200);

    // 3. Señal de vida
    Serial.println("\n[BOOT] Sistema Rover: Hardware validado.");
    Serial.println("[INFO] Esperando implementación del Stack de Red (Paso C)...");
}

void loop()
{
    // Bucle vacío de bajo consumo (Heartbeat lento)
    // Aquí irá la lógica de reconexión WiFi y manejo de clientes.
    delay(1000);
}