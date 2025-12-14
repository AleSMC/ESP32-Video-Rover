/**
 * @file NetworkManager.cpp
 * @brief Implementación del Gestor de Red Híbrido (STA + AP).
 * @author Alejandro Moyano (@AleSMC)
 * @version 1.1.0
 */

#include "NetworkManager.h"

NetworkManager::NetworkManager()
{
    _isAP = false; // Estado inicial: Asumimos rol de Cliente (STA)
}

void NetworkManager::begin()
{
    // 1. CONFIGURACIÓN INICIAL
    // Forzamos modo estación para limpiar configuraciones previas
    WiFi.mode(WIFI_STA);

    Serial.println("\n[RED] Iniciando gestor de conectividad...");
    Serial.printf("[RED] Intentando conectar a SSID: %s\n", WIFI_SSID);

    // 2. INTENTO DE CONEXIÓN (STA)
    // Usa las credenciales definidas en 'secrets.h'
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    // 3. ESPERA ACTIVA CON TIMEOUT (10s)
    // Bloqueamos el arranque brevemente para intentar conectar.
    // Si falla, no bloqueamos el sistema eternamente; pasamos al plan B.
    unsigned long startAttempt = millis();
    bool connected = false;

    while (millis() - startAttempt < 10000)
    {
        if (WiFi.status() == WL_CONNECTED)
        {
            connected = true;
            break;
        }
        delay(500);
        Serial.print(".");
    }
    Serial.println();

    // 4. EVALUACIÓN DE RESULTADO Y FAILOVER
    if (connected)
    {
        // --- CASO A: ÉXITO (HOGAR) ---
        _isAP = false;
        Serial.println("[RED] ¡Conexión Exitosa!");
        Serial.printf("[RED] Modo: ESTACIÓN (Cliente)\n");
        Serial.printf("[RED] Signal (RSSI): %d dBm\n", WiFi.RSSI());
    }
    else
    {
        // --- CASO B: FALLO (CAMPO / ERROR) -> MODO EMERGENCIA ---
        Serial.println("[RED] Timeout. No se pudo conectar al Router.");
        Serial.println("[RED] ACTIVANDO PROTOCOLO DE EMERGENCIA (Hotspot)...");

        WiFi.disconnect();  // Limpiar config corrupta
        WiFi.mode(WIFI_AP); // Cambiar radio a modo Punto de Acceso

        // Desplegar red propia (Rover-Emergency)
        // Parámetros: SSID, Pass, Canal, Oculto(no), MaxConexiones
        bool apCreated = WiFi.softAP(AP_SSID, AP_PASSWORD, AP_CHANNEL, 0, AP_MAX_CONN);

        if (apCreated)
        {
            _isAP = true;
            Serial.printf("[RED] AP Creado con éxito.\n");
            Serial.printf("[RED] SSID: %s\n", AP_SSID);
            Serial.printf("[RED] Password: %s\n", AP_PASSWORD);
        }
        else
        {
            Serial.println("[ERROR] CRÍTICO: Fallo al crear AP.");
        }
    }

    // 5. INICIO SERVICIO mDNS
    // Permite resolución de nombres 'rover.local' en redes compatibles.
    // Nota: Android/iOS en modo Hotspot suelen bloquear mDNS.
    if (MDNS.begin(DEVICE_HOSTNAME))
    {
        Serial.printf("[RED] mDNS iniciado. Accede vía: http://%s.local\n", DEVICE_HOSTNAME);
    }
    else
    {
        Serial.println("[ERROR] No se pudo iniciar mDNS.");
    }

    // REPORTE FINAL DE IP
    Serial.println("------------------------------------------------");
    Serial.printf("[INFO] DIRECCIÓN IP: %s\n", getIP().c_str());
    Serial.println("------------------------------------------------");
}

void NetworkManager::update()
{
    /**
     * @note EXPLICACIÓN TÉCNICA (ESP32 FreeRTOS):
     * A diferencia de Arduino clásico, el ESP32 ejecuta el stack TCP/IP (LwIP)
     * en una tarea de FreeRTOS en segundo plano (Core 0 o IDLE).
     * * No es necesario llamar a una función 'keep_alive' aquí para mantener el WiFi.
     * La conexión se mantiene sola por hardware/OS.
     * * Esta función se deja vacía intencionalmente para futura implementación
     * de lógica de "Reconexión Automática" (Watchdog de Red) si se desea
     * que el Rover intente volver al WiFi de casa si se cae la señal.
     */
}

String NetworkManager::getIP()
{
    // Devuelve la IP correcta según el modo activo
    if (_isAP)
    {
        return WiFi.softAPIP().toString();
    }
    else
    {
        return WiFi.localIP().toString();
    }
}

String NetworkManager::getMode()
{
    return _isAP ? "AP (Hotspot)" : "STA (WiFi Hogar)";
}