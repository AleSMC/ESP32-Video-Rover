/**
 * @file config.h
 * @brief Archivo maestro de configuración del sistema ESP32-Video-Rover.
 * @details Fuente Única de Verdad (Single Source of Truth) para todo el proyecto.
 * * --- ESPECIFICACIONES DE HARDWARE ---
 * @board ESP32-CAM (Modelo AI Thinker).
 * @driver L298N Dual H-Bridge (Configurado en Modo Eje Sólido / Solid Axle).
 * @actuator Servomotor estándar (SG90/MG90S) para dirección Ackermann.
 * * --- FUNCIONALIDADES ---
 * - Red Híbrida (STA + AP de Respaldo).
 * - Protocolos: mDNS, UDP (Control) y HTTP (Video MJPEG).
 * - Seguridad: Failsafe activo y gestión de Brownout.
 * * @author Alejandro Moyano (@AleSMC)
 */

#pragma once

// =============================================================================
// 1. CONFIGURACIÓN DE TRACCIÓN (TOPOLOGÍA EJE SÓLIDO)
// =============================================================================
// Los motores traseros (Izquierdo y Derecho) comparten señales eléctricas.

/** * @brief Pin de Velocidad Global (PWM).
 * @details Conectado a los pines ENA y ENB del driver (Puenteados físicamente).
 * Controla la potencia del 0% al 100%.
 * @note GPIO 13 es seguro para PWM y no interfiere con el arranque del ESP32.
 */
#define PIN_MOTOR_PWM 13

/** * @brief Pin de Dirección: Marcha Adelante (Forward / FWD).
 * @details Conectado a IN1 e IN3 del driver (Puenteados físicamente).
 * Al activarse, ambas ruedas giran hacia adelante.
 */
#define PIN_MOTOR_FWD 14

/** * @brief Pin de Dirección: Marcha Atrás (Reverse / REV).
 * @details Conectado a IN2 e IN4 del driver (Puenteados físicamente).
 * Al activarse, ambas ruedas giran hacia atrás.
 */
#define PIN_MOTOR_REV 15

/** * @brief Pin RESERVADO (No Conectado).
 * @warning CRÍTICO: GPIO 12 es un 'Strapping Pin' (MTDI).
 * Si el driver L298N mantiene este pin en HIGH durante el arranque (Boot),
 * el ESP32 configurará mal el voltaje interno de la Flash (1.8V) y no arrancará.
 * @note Solución de Diseño: Se deja desconectado físicamente en esta versión.
 */
#define PIN_RESERVED_12 12

// =============================================================================
// 2. CONFIGURACIÓN DE DIRECCIÓN (SERVO)
// =============================================================================

/** * @brief Pin de señal PWM para el servo de dirección.
 * @note Este pin (GPIO 4 o 2 según variante) suele estar compartido con el LED Flash.
 * El movimiento del servo podría causar destellos leves en el LED.
 */
#define PIN_SERVO 2

// =============================================================================
// 3. CONFIGURACIÓN DE RED Y PROTOCOLOS
// =============================================================================

// --- Identidad mDNS ---
/** @brief Hostname base. El dispositivo será accesible como 'rover.local'. */
#define MDNS_NAME "rover"
#define MDNS_DOMAIN "local"

// --- Puertos de Servicio ---
/** @brief Puerto de escucha para paquetes de control UDP (WASD). */
#define UDP_CONTROL_PORT 3333
/** @brief Puerto TCP para el servidor web y streaming MJPEG. */
#define MJPEG_HTTP_PORT 80

// --- Seguridad ---
/** * @brief Tiempo máximo sin recibir paquetes UDP antes de activar el Failsafe.
 * Si pasan 500ms sin comandos, los motores se detienen por seguridad.
 */
#define UDP_FAILSAFE_MS 500

// =============================================================================
// 4. CREDENCIALES DE EMERGENCIA (Fallback AP)
// =============================================================================
// Estas credenciales se usan SOLO si falla la conexión al WiFi principal (secrets.h).
#define WIFI_AP_SSID "Rover-Emergency"
#define WIFI_AP_PASS "rover1234"