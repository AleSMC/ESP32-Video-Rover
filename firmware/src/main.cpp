/**
 * @file main.cpp
 * @brief Punto de entrada del Firmware - Fase C (Stack de Red).
 * @author Alejandro Moyano (@AleSMC)
 * @version 1.3.0 (Network Layer)
 * * @details
 * Este archivo orquesta la inicialización del sistema. En la Fase C, su responsabilidad es:
 * 1. Gestión de Energía (Desactivar Brownout Detector).
 * 2. Inicialización del Hardware Serial (Logs).
 * 3. Despliegue del Gestor de Red (NetworkManager) con lógica de Failover (STA -> AP).
 * 4. Bucle principal con mantenimiento de servicios y telemetría básica.
 * * @note
 * El código de control de motores y servo se integrará en fases posteriores.
 */

#include <Arduino.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "config.h"         // Configuración Maestra de Hardware
#include "NetworkManager.h" // Gestor de Conectividad (WiFi + mDNS)

// =============================================================================
// INSTANCIAS GLOBALES
// =============================================================================
/** * @brief Gestor de Red.
 * Encargado de mantener la conexión WiFi y el servicio mDNS.
 */
NetworkManager network;

// =============================================================================
// SETUP (CONFIGURACIÓN INICIAL)
// =============================================================================
void setup()
{
    // 1. GESTIÓN DE ENERGÍA (CRÍTICO)
    // El arranque del WiFi genera picos de corriente altos. Si la batería no es perfecta,
    // el voltaje puede caer momentáneamente (dip), disparando el "Brownout Detector"
    // y reiniciando el ESP32 en bucle. Lo desactivamos para ganar estabilidad.
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

    // 2. INICIO DE PUERTO SERIE
    // Velocidad: 115200 baudios (Estándar para ESP32).
    Serial.begin(115200);

    // Pequeña pausa para permitir que el chip USB-Serial se estabilice
    delay(1000);

    // 3. INICIO DEL STACK DE RED
    // Esta función es bloqueante (puede tardar ~10s).
    // Intenta conectar a WiFi Hogar (STA). Si falla, crea WiFi Propia (AP).
    network.begin();

    Serial.println("\n[BOOT] Sistema iniciado correctamente. Esperando clientes...");
}

// =============================================================================
// LOOP (BUCLE PRINCIPAL)
// =============================================================================
void loop()
{
    // 1. MANTENIMIENTO DE RED
    // Vital para gestionar reconexiones y atender peticiones mDNS entrantes.
    network.update();

    // 2. TELEMETRÍA (HEARTBEAT)
    // Imprime el estado cada 5 segundos sin bloquear el procesador.
    // Usamos el patrón "Millis Timer" en lugar de delay().
    static unsigned long lastTime = 0;
    const unsigned long interval = 5000;

    if (millis() - lastTime > interval)
    {
        lastTime = millis();

        // Reporte de estado en consola
        Serial.printf("[STATUS] Modo: %s | IP: %s | Uptime: %lu s\n",
                      network.getMode().c_str(),
                      network.getIP().c_str(),
                      millis() / 1000);
    }
}