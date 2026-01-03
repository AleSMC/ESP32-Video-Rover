/**
 * @file NetworkManager.cpp
 * @brief Hybrid Network Manager Implementation (STA + AP).
 * @author Alejandro Moyano (@AleSMC)
 * @version 1.1.0
 */

#include "NetworkManager.h"

NetworkManager::NetworkManager()
{
    _isAP = false; // Initial state: Assume Client role (STA)
}

void NetworkManager::begin()
{
    // 1. INITIAL CONFIGURATION
    // Force Station mode to clean previous configurations
    WiFi.mode(WIFI_STA);

    Serial.println("\n[NET] Starting connectivity manager...");
    Serial.printf("[NET] Attempting to connect to SSID: %s\n", WIFI_SSID);

    // 2. CONNECTION ATTEMPT (STA)
    // Uses credentials defined in 'secrets.h'
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    // 3. ACTIVE WAIT WITH TIMEOUT (10s)
    // We block boot briefly to attempt connection.
    // If it fails, we don't block the system eternally; we switch to Plan B.
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

    // 4. RESULT EVALUATION AND FAILOVER
    if (connected)
    {
        // --- CASE A: SUCCESS (HOME) ---
        _isAP = false;
        Serial.println("[NET] Connection Successful!");
        Serial.printf("[NET] Mode: STATION (Client)\n");
        Serial.printf("[NET] Signal (RSSI): %d dBm\n", WiFi.RSSI());
    }
    else
    {
        // --- CASE B: FAILURE (FIELD/ERROR) -> EMERGENCY MODE ---
        Serial.println("[NET] Timeout. Could not connect to Router.");
        Serial.println("[NET] ACTIVATING EMERGENCY PROTOCOL (Hotspot)...");

        WiFi.disconnect();  // Clean corrupt config
        WiFi.mode(WIFI_AP); // Switch radio to Access Point mode

        // Deploy own network (Rover-Emergency)
        // Parameters: SSID, Pass, Channel, Hidden(no), MaxConn
        bool apCreated = WiFi.softAP(AP_SSID, AP_PASSWORD, AP_CHANNEL, 0, AP_MAX_CONN);

        if (apCreated)
        {
            _isAP = true;
            Serial.printf("[NET] AP Created Successfully.\n");
            Serial.printf("[NET] SSID: %s\n", AP_SSID);
            Serial.printf("[NET] Password: %s\n", AP_PASSWORD);
        }
        else
        {
            Serial.println("[ERROR] CRITICAL: Failed to create AP.");
        }
    }

    // 5. START mDNS SERVICE
    // Allows name resolution 'rover.local' on compatible networks.
    // Note: Android/iOS in Hotspot mode often block mDNS.
    if (MDNS.begin(DEVICE_HOSTNAME))
    {
        Serial.printf("[NET] mDNS started. Access via: http://%s.local\n", DEVICE_HOSTNAME);
    }
    else
    {
        Serial.println("[ERROR] Could not start mDNS.");
    }

    // FINAL IP REPORT
    Serial.println("------------------------------------------------");
    Serial.printf("[INFO] IP ADDRESS: %s\n", getIP().c_str());
    Serial.println("------------------------------------------------");
}

void NetworkManager::update()
{
    /**
     * @note TECHNICAL EXPLANATION (ESP32 FreeRTOS):
     * Unlike classic Arduino, the ESP32 runs the TCP/IP stack (LwIP)
     * in a background FreeRTOS task (Core 0 or IDLE).
     *
     * - It is not necessary to call a 'keep_alive' function here to maintain WiFi.
     * The connection is maintained by hardware/OS.
     * - This function is intentionally left empty for future implementation
     * of "Auto-Reconnect" logic (Network Watchdog) if it is desired
     * for the Rover to attempt to rejoin Home WiFi if the signal drops.
     */
}

String NetworkManager::getIP()
{
    // Returns the correct IP based on active mode
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
    return _isAP ? "AP (Hotspot)" : "STA (Home WiFi)";
}
