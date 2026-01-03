/**
 * @file secrets_example.h
 * @brief Network Credentials Template (Placeholder).
 * @details USAGE INSTRUCTIONS FOR DEVELOPERS:
 * 1. Duplicate this file and rename it to 'secrets.h'.
 * 2. Fill 'secrets.h' with your real credentials.
 * 3. The 'secrets.h' file will be ignored by Git for security.
 * @author Alejandro Moyano (@AleSMC)
 */

#pragma once

// =============================================================================
// 1. STATION MODE (STA) - MAIN CONNECTION
// =============================================================================

/** @brief WiFi Network Name (SSID) to connect to. */
#define WIFI_SSID "YOUR_SSID_HERE"

/** @brief WiFi Password. */
#define WIFI_PASSWORD "YOUR_PASSWORD_HERE"

// =============================================================================
// 2. ACCESS POINT MODE (AP) - EMERGENCY BACKUP
// =============================================================================

/** @brief Rover Hotspot SSID. */
#define AP_SSID "Rover-Emergency"

/** @brief WPA2 Password to access the Rover. */
#define AP_PASSWORD "rover1234"

/** @brief WiFi Broadcast Channel (1-13). */
#define AP_CHANNEL 1

/** @brief Simultaneous connection limit (RAM saving). */
#define AP_MAX_CONN 2

// =============================================================================
// 3. NETWORK IDENTITY (mDNS)
// =============================================================================

/** @brief Base Hostname (e.g., rover -> rover.local). */
#define DEVICE_HOSTNAME "rover"
