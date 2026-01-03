/**
 * @file NetworkManager.h
 * @brief WiFi Connectivity Interface Contract (STA + AP).
 * @author Alejandro Moyano (@AleSMC)
 * @version 1.1.0
 * @details
 * Exposes methods to manage the connection without blocking the main thread
 * indefinitely and provides getters for telemetry.
 */

#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include "secrets.h" // Critical dependency: WIFI_SSID, AP_SSID, etc.

class NetworkManager
{
private:
    /**
     * @brief Current operation state.
     * - true: Emergency Mode (Own Hotspot).
     * - false: Normal Mode (Connected to Router).
     */
    bool _isAP;

public:
    /**
     * @brief Constructor. Initializes default state to Client (STA).
     */
    NetworkManager();

    /**
     * @brief Starts the network state machine.
     * @details
     * 1. Attempts to connect to WIFI_SSID (10s timeout).
     * 2. If it fails, raises AP_SSID (Emergency Network).
     * 3. Starts mDNS for name resolution.
     * @note This function is blocking during the connection attempt.
     */
    void begin();

    /**
     * @brief Maintenance cycle (Tick).
     * @note Currently passive thanks to internal FreeRTOS management on ESP32,
     * but reserved for future automatic reconnection logic (Network Watchdog).
     */
    void update();

    /**
     * @brief Returns the assigned IP.
     * @return String formatted "XXX.XXX.XXX.XXX". Depends on mode (STA vs AP).
     */
    String getIP();

    /**
     * @brief Returns a readable description of the current mode.
     * @return "STA (Home WiFi)" or "AP (Hotspot)".
     */
    String getMode();
};
