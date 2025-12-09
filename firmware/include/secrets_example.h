/**
 * @file secrets_example.h
 * @brief Plantilla de credenciales de red y configuración sensible.
 * @details INSTRUCCIONES DE USO:
 * 1. Duplica este archivo y renómbralo a 'secrets.h'.
 * 2. Rellena 'secrets.h' con tus credenciales reales.
 * 3. El archivo 'secrets.h' será ignorado por Git (ver .gitignore) para seguridad.
 * @author Alejandro Moyano (@AleSMC)
 */

#ifndef SECRETS_H
#define SECRETS_H

// ==========================================
// 1. CONFIGURACIÓN MODO ESTACIÓN (Cliente)
// ==========================================
// El Rover intentará conectarse a esta red WiFi primero (ej: tu casa).
const char *WIFI_SSID = "TU_NOMBRE_DE_WIFI_AQUI";
const char *WIFI_PASS = "TU_CONTRASEÑA_WIFI_AQUI";

// ==========================================
// 2. CONFIGURACIÓN MODO PUNTO DE ACCESO (AP)
// ==========================================
// Si falla la conexión anterior, el Rover creará su propia red.
// SSID: Nombre visible del Rover en el móvil
const char *AP_SSID = "Rover-Emergency";
// PASS: Contraseña para conectarte al Rover (Mínimo 8 caracteres)
const char *AP_PASS = "rover1234";

// ==========================================
// 3. CONFIGURACIÓN DE RED
// ==========================================
// Puerto UDP donde el Rover escuchará comandos (WASD)
const int UDP_PORT = 9999;

#endif