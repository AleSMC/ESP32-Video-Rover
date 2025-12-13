/**
 * @file secrets_example.h
 * @brief Plantilla de credenciales de red (Placeholder).
 * @details INSTRUCCIONES DE USO PARA DESARROLLADORES:
 * 1. Duplica este archivo y renómbralo a 'secrets.h'.
 * 2. Rellena 'secrets.h' con tus credenciales reales.
 * 3. El archivo 'secrets.h' será ignorado por Git para seguridad.
 * @author Alejandro Moyano (@AleSMC)
 */

#pragma once

// =============================================================================
// 1. MODO ESTACIÓN (STA) - CONEXIÓN PRINCIPAL
// =============================================================================

/** @brief Nombre de la red WiFi (SSID) a la que conectarse. */
#define WIFI_SSID "PON_TU_SSID_AQUI"

/** @brief Contraseña de la red WiFi. */
#define WIFI_PASSWORD "PON_TU_CONTRASEÑA_AQUI"

// =============================================================================
// 2. MODO PUNTO DE ACCESO (AP) - RESPALDO DE EMERGENCIA
// =============================================================================

/** @brief SSID del Hotspot del Rover. */
#define AP_SSID "Rover-Emergency"

/** @brief Contraseña WPA2 para acceder al Rover. */
#define AP_PASSWORD "rover1234"

/** @brief Canal WiFi de emisión (1-13). */
#define AP_CHANNEL 1

/** @brief Límite de conexiones simultáneas (Ahorro RAM). */
#define AP_MAX_CONN 2

// =============================================================================
// 3. IDENTIDAD DE RED (mDNS)
// =============================================================================

/** @brief Hostname base (ej: rover -> rover.local). */
#define DEVICE_HOSTNAME "rover"
