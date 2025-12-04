/**
 * @file config.h
 * @brief Configuración global del hardware, mapeo de pines y constantes de red.
 * @details Este archivo actúa como la "Single Source of Truth" (Fuente Única de Verdad)
 * para la configuración física y lógica del Rover.
 * * @board ESP32-CAM (AI Thinker Model)
 * @driver L298N Dual H-Bridge
 * @author ControlRC
 */

#pragma once

// =============================================================================
// 1. CONFIGURACIÓN DE ACTUADORES (L298N + SERVO)
// =============================================================================

// --- Motor A (Izquierda) ---
/** @brief Pin de dirección 1 para el Motor A (Puente H). */
#define PIN_MOTOR_IN1 14
/** @brief Pin de dirección 2 para el Motor A (Puente H). */
#define PIN_MOTOR_IN2 15

// --- Motor B (Derecha) ---
/** @brief Pin de dirección 1 para el Motor B (Puente H). */
#define PIN_MOTOR_IN3 13

/** * @brief Pin de dirección 2 para el Motor B (Puente H).
 * @warning CRÍTICO: GPIO 12 es un 'Strapping Pin' (MTDI).
 * Si el driver L298N mantiene este pin en HIGH durante el arranque (Boot),
 * el ESP32 configurará mal el voltaje de la Flash (1.8V) y no arrancará.
 * @note Solución de campo: Desconectar este cable si ocurre un Boot Loop.
 */
#define PIN_MOTOR_IN4 12

// --- Servo de Dirección ---
/** * @brief Pin de señal PWM para el servo.
 * @note Este pin (GPIO 4 o 2 según modelo) suele estar compartido con el LED Flash.
 * El movimiento del servo podría causar destellos leves en el LED.
 */
#define PIN_SERVO 2

// =============================================================================
// 2. CONFIGURACIÓN DE RED Y PROTOCOLOS
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
// 3. CREDENCIALES DE EMERGENCIA (Fallback AP)
// =============================================================================
// Estas credenciales se usan SOLO si falla la conexión al WiFi principal (secrets.h).
#define WIFI_AP_SSID "Rover-Emergency"
#define WIFI_AP_PASS "rover1234"
