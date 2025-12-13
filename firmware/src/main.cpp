/**
 * @file main.cpp
 * @brief Punto de entrada del Firmware - Fase C.2 (Streaming de Video).
 * @author Alejandro Moyano (@AleSMC)
 * @version 1.4.0 (Video Layer)
 * * @details
 * Orquesta la inicialización del sistema completo.
 * 1. Inicializa la Cámara (Prioridad Hardware).
 * 2. Conecta a la Red (WiFi/AP).
 * 3. Inicia el Servidor Web.
 * 4. Mantiene el sistema vivo.
 * * @note
 * Si la cámara falla al inicio, el sistema entra en bucle de error para proteger el hardware.
 */

#include <Arduino.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "config.h"
#include "NetworkManager.h"
#include "CameraServer.h"

// =============================================================================
// INSTANCIAS GLOBALES
// =============================================================================
NetworkManager network; // Gestor de Conectividad
CameraServer camera;    // Gestor de Video

// =============================================================================
// SETUP
// =============================================================================
void setup()
{
    // 1. GESTIÓN DE ENERGÍA (CRÍTICO)
    // Desactivar Brownout para soportar picos de consumo de WiFi + Cámara
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

    // 2. INICIO SERIAL
    Serial.begin(115200);
    delay(1000);

    // 3. INICIALIZAR CÁMARA (OV2640)
    // Se inicia ANTES del WiFi para reservar los buffers de memoria (DMA)
    // sin fragmentación.
    Serial.println("\n[BOOT] Inicializando Hardware de Video...");
    if (camera.init())
    {
        Serial.println("[BOOT] Cámara OV2640 lista y buffers asignados.");
    }
    else
    {
        Serial.println("[ERROR] No se detecta la cámara. REVISA CABLE FLEX.");
        // Bucle de muerte: Detenemos todo si no hay ojos.
        while (true)
        {
            delay(1000);
        }
    }

    // 4. INICIAR STACK DE RED
    // Conecta a WiFi Hogar o crea AP de Emergencia
    network.begin();

    // 5. ARRANCAR SERVIDOR WEB
    // Habilita la ruta /stream en el puerto 80
    camera.startServer();

    // 6. REPORTE FINAL
    Serial.println("\n[BOOT] Sistema ONLINE - Video Ready.");
    Serial.printf("[INFO] Ver Video (mDNS): http://%s.local/stream\n", MDNS_NAME);
    Serial.printf("[INFO] Ver Video (IP):   http://%s/stream\n", network.getIP().c_str());
}

// =============================================================================
// LOOP
// =============================================================================
void loop()
{
    // Mantenimiento de servicios de red
    network.update();

    // Telemetría periódica (Heartbeat) cada 5 segundos
    static unsigned long lastTime = 0;
    if (millis() - lastTime > 5000)
    {
        lastTime = millis();
        Serial.printf("[ALIVE] Modo: %s | IP: %s | Uptime: %lu s\n",
                      network.getMode().c_str(),
                      network.getIP().c_str(),
                      millis() / 1000);
    }
}