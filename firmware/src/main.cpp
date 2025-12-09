/**
 * @file main.cpp
 * @brief Archivo principal limpio para el siguiente paso (Paso B: Servo).
 * @author Alejandro Moyano (@AleSMC)
 */

#include <Arduino.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "config.h"

void setup()
{
    // 1. Gestión de Energía (Siempre necesario)
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

    // 2. Serial
    Serial.begin(115200);
    Serial.println("\n[BOOT] Sistema listo para Paso B (Servo)...");
}

void loop()
{
    // Nada por ahora
    delay(1000);
}