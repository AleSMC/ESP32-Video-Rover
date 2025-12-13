/**
 * @file config.h
 * @brief Archivo maestro de configuración de Hardware y Constantes del Sistema.
 * @details Fuente Única de Verdad (Single Source of Truth) para el mapeo de GPIOs
 * y parámetros físicos del robot.
 * * --- ESPECIFICACIONES DE HARDWARE ---
 * @board ESP32-CAM (Modelo AI Thinker).
 * @driver L298N Dual H-Bridge (Configurado en Modo Eje Sólido / Solid Axle).
 * @actuator Servomotor estándar (SG90/MG90S) para dirección Ackermann.
 * * --- FUNCIONALIDADES ---
 * - Mapeo de Pines (GPIO).
 * - Calibración Mecánica (Servo).
 * - Constantes de Protocolo (Puertos y Tiempos).
 * * @warning NO incluir credenciales WiFi aquí. Usar 'secrets.h'.
 * @author Alejandro Moyano (@AleSMC)
 */

#pragma once
#include <Arduino.h>

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
 * @warning La reversa debe usarse con precaución (Ver documentación sobre Back-EMF).
 */
#define PIN_MOTOR_REV 15

/** * @brief Pin RESERVADO (No Conectado).
 * @warning CRÍTICO: GPIO 12 es un 'Strapping Pin' (MTDI).  * Si el driver L298N mantiene este pin en HIGH durante el arranque (Boot),
 * el ESP32 configurará mal el voltaje interno de la Flash (1.8V) y no arrancará.
 * @note Solución de Diseño: Se deja desconectado físicamente en esta versión.
 */
#define PIN_RESERVED_12 12

// =============================================================================
// 2. CONFIGURACIÓN DE DIRECCIÓN (SERVO ACKERMANN)
// =============================================================================

/** * @brief Pin de señal PWM para el servo.
 * @note El GPIO 2 comparte línea física con el LED Flash de alta potencia.
 * Es comportamiento esperado ver destellos en el LED al mover la dirección.
 */
#define PIN_SERVO 2

// --- CALIBRACIÓN DE ÁNGULOS (GRADOS 0-180) ---
// AJUSTA ESTOS VALORES POCO A POCO PARA NO FORZAR EL MECANISMO

/** @brief Ángulo central (Ruedas rectas). Valor ideal teórico: 90. */
#define STEERING_CENTER 90

/** @brief Límite Máximo Izquierda.
 * Empieza con un valor cercano a 90 (ej: 75) y baja poco a poco hacia 0.
 * Si oyes zumbidos, has llegado al tope físico: retrocede 5 grados inmediatamente.
 */
#define STEERING_LEFT_MAX 70

/** @brief Límite Máximo Derecha.
 * Empieza con un valor cercano a 90 (ej: 105) y sube poco a poco hacia 180.
 */
#define STEERING_RIGHT_MAX 110

// =============================================================================
// 3. CONFIGURACIÓN DE PROTOCOLOS (CONSTANTES DE SISTEMA)
// =============================================================================

/** * @brief Puerto de escucha UDP para Comandos.
 * @details El Rover escuchará paquetes de control (ej: "W", "A", "S", "D") en este puerto.
 * @note Debe coincidir con el puerto de envío del cliente Python.
 */
const int UDP_PORT = 9999;

/** * @brief Puerto TCP para el Servidor Web.
 * @details Puerto estándar HTTP para servir la interfaz y el stream MJPEG.
 */
const int HTTP_PORT = 80;

// --- Seguridad ---

/** * @brief Tiempo máximo sin recibir paquetes UDP antes de activar el Failsafe.
 * @details Si pasan 500ms sin recibir comandos válidos, el Watchdog de software
 * detendrá los motores para evitar que el robot se escape si pierde WiFi.
 */
const int UDP_FAILSAFE_MS = 500;
