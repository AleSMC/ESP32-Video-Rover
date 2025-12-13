/**
 * @file NetworkManager.h
 * @brief Gestor de conectividad WiFi (Híbrido STA/AP) y mDNS.
 * @details Gestiona automáticamente la conexión a la red doméstica o el despliegue
 * de un punto de acceso de emergencia.
 * @author Alejandro Moyano (@AleSMC)
 */

#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include "secrets.h" // Importamos las credenciales (WIFI_SSID, AP_SSID, etc.)

class NetworkManager
{
private:
    bool _isAP; // Estado interno: true = Modo Punto de Acceso, false = Modo Estación

public:
    /**
     * @brief Constructor por defecto.
     */
    NetworkManager();

    /**
     * @brief Inicia la lógica de conexión.
     * 1. Intenta conectar a la red WiFi configurada (STA) durante 10s.
     * 2. Si falla, levanta un Punto de Acceso (AP) propio.
     * 3. Inicia el servicio mDNS (rover.local).
     */
    void begin();

    /**
     * @brief Mantenimiento de red.
     * Debe llamarse periódicamente en el loop() para gestionar reconexiones o servicios.
     */
    void update();

    /**
     * @brief Obtiene la dirección IP actual del dispositivo.
     * @return String con la IP (ej: "192.168.1.50" o "192.168.4.1").
     */
    String getIP();

    /**
     * @brief Obtiene el modo de operación actual en texto.
     * @return "STA (WiFi Hogar)" o "AP (Hotspot)".
     */
    String getMode();
};
