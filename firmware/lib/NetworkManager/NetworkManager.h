/**
 * @file NetworkManager.h
 * @brief Contrato de la interfaz de conectividad WiFi (STA + AP).
 * @author Alejandro Moyano (@AleSMC)
 * @version 1.1.0
 * @details
 * Expone métodos para gestionar la conexión sin bloquear el hilo principal
 * indefinidamente y provee getters para telemetría.
 */

#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include "secrets.h" // Dependencia crítica: WIFI_SSID, AP_SSID, etc.

class NetworkManager
{
private:
    /**
     * @brief Estado actual de operación.
     * - true: Modo Emergencia (Hotspot propio).
     * - false: Modo Normal (Conectado a Router).
     */
    bool _isAP;

public:
    /**
     * @brief Constructor. Inicializa el estado por defecto a Cliente (STA).
     */
    NetworkManager();

    /**
     * @brief Inicia la máquina de estados de red.
     * @details
     * 1. Intenta conectar a WIFI_SSID (timeout 10s).
     * 2. Si falla, levanta AP_SSID (Red de emergencia).
     * 3. Inicia mDNS para resolución de nombres.
     * @note Esta función es bloqueante durante el intento de conexión.
     */
    void begin();

    /**
     * @brief Ciclo de mantenimiento (Tick).
     * @note Actualmente pasivo gracias a la gestión interna de FreeRTOS en ESP32,
     * pero reservado para futura lógica de reconexión automática (Watchdog de Red).
     */
    void update();

    /**
     * @brief Devuelve la IP asignada.
     * @return String con formato "XXX.XXX.XXX.XXX". Depende del modo (STA vs AP).
     */
    String getIP();

    /**
     * @brief Devuelve una descripción legible del modo actual.
     * @return "STA (WiFi Hogar)" o "AP (Hotspot)".
     */
    String getMode();
};