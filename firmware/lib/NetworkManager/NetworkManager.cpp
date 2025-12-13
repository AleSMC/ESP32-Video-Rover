/**
 * @file NetworkManager.cpp
 * @brief Implementación del gestor de red robusto.
 * @author Alejandro Moyano (@AleSMC)
 */

#include "NetworkManager.h"

NetworkManager::NetworkManager()
{
    _isAP = false; // Asumimos por defecto que intentaremos ser clientes
}

void NetworkManager::begin()
{
    // 1. Configuración inicial
    WiFi.mode(WIFI_STA); // Modo Estación (Cliente)

    Serial.println("\n[RED] Iniciando gestor de conectividad...");
    Serial.printf("[RED] Intentando conectar a SSID: %s\n", WIFI_SSID);

    // 2. Intento de conexión STA
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    // 3. Espera activa con Timeout (10 segundos)
    unsigned long startAttempt = millis();
    bool connected = false;

    // Bucle de espera visual
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
    Serial.println(); // Nueva línea tras los puntos

    // 4. Evaluación de resultado
    if (connected)
    {
        // --- ÉXITO: CONECTADO A ROUTER ---
        _isAP = false;
        Serial.println("[RED] ¡Conexión Exitosa!");
        Serial.printf("[RED] Modo: ESTACIÓN (Cliente)\n");
        Serial.printf("[RED] Signal (RSSI): %d dBm\n", WiFi.RSSI());
    }
    else
    {
        // --- FALLO: MODO EMERGENCIA (AP) ---
        Serial.println("[RED] Tiempo agotado. No se pudo conectar al Router.");
        Serial.println("[RED] Desplegando Punto de Acceso de Emergencia...");

        WiFi.disconnect();  // Limpiar configuración fallida
        WiFi.mode(WIFI_AP); // Cambiar a modo Punto de Acceso

        // Configuración del AP con parámetros de secrets.h
        // softAP(ssid, password, channel, hidden, max_connection)
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
            Serial.println("[ERROR] Fallo crítico al crear AP.");
        }
    }

    // 5. Iniciar servicio de descubrimiento mDNS
    // Esto permite acceder como 'rover.local' en lugar de usar la IP
    if (MDNS.begin(DEVICE_HOSTNAME))
    {
        Serial.printf("[RED] mDNS iniciado. Accede vía: http://%s.local\n", DEVICE_HOSTNAME);
    }
    else
    {
        Serial.println("[ERROR] No se pudo iniciar el responder mDNS.");
    }

    // Mostrar IP Final
    Serial.println("------------------------------------------------");
    Serial.printf("[INFO] DIRECCIÓN IP: %s\n", getIP().c_str());
    Serial.println("------------------------------------------------");
}

void NetworkManager::update()
{
    // Por ahora no necesitamos lógica de reconexión compleja en el loop.
    // En el futuro, aquí vigilaremos si se cae el WiFi para reiniciar el proceso.
}

String NetworkManager::getIP()
{
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
