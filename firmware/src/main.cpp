/**
 * @file main.cpp
 * @brief Firmware Final v1.0 - ESP32 Video Rover.
 * @details Orquestador principal del sistema. Integra:
 * - Capa C.2: Video Streaming (MJPEG via HTTP)
 * - Capa C.1: Red Híbrida (WiFi STA/AP Failover)
 * - Capa D: Control Remoto UDP (Protocolo Binario + Seguridad)
 * @author Alejandro Moyano (@AleSMC)
 */

#include <Arduino.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

// --- LIBRERÍAS DEL PROYECTO ---
#include "config.h"
#include "NetworkManager.h"
#include "CameraServer.h"
#include "SolidAxle.h"
#include "SteeringServo.h"
#include "RemoteControl.h"

// =============================================================================
// INSTANCIAS GLOBALES (Arquitectura de Servicios)
// =============================================================================

// 1. Gestores de Alto Nivel (Red y Video)
NetworkManager network;
CameraServer camera;

// 2. Drivers de Hardware (Actuadores Físicos)
// Instanciamos los objetos con los pines definidos en 'config.h'
SolidAxle motors(PIN_MOTOR_FWD, PIN_MOTOR_REV, PIN_MOTOR_PWM);
SteeringServo steering(PIN_SERVO, STEERING_CENTER, STEERING_LEFT_MAX, STEERING_RIGHT_MAX);

// 3. Controlador Lógico (Inyección de Dependencias)
// Pasamos los punteros (&) de los drivers al controlador remoto.
// Esto permite que 'remote' manipule 'motors' y 'steering' sin poseerlos.
RemoteControl remote(&motors, &steering);

// =============================================================================
// SETUP (Inicialización del Sistema)
// =============================================================================
void setup()
{
    // 1. GESTIÓN DE ENERGÍA (CRÍTICO)
    // Desactivar el Brownout Detector. El arranque del WiFi y Motores genera
    // picos de corriente que podrían reiniciar el ESP32 si esto estuviera activo.
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

    // 2. INICIO PUERTO SERIE (Debug)
    Serial.begin(115200);
    delay(1000);

    // 3. INICIALIZAR ACTUADORES FÍSICOS
    // Es seguro iniciarlos antes que el WiFi.
    Serial.println("\n[BOOT] Inicializando Motores y Servo...");
    motors.begin();
    steering.begin();
    steering.center(); // Posición segura inicial (Ruedas rectas)

    // 4. INICIALIZAR CÁMARA
    // Prioridad alta: La cámara requiere reservar grandes bloques de memoria (DMA/PSRAM).
    // Hacemos esto antes de iniciar el stack WiFi para evitar fragmentación de RAM.
    Serial.println("[BOOT] Inicializando Hardware de Video...");
    if (camera.init())
    {
        Serial.println("[BOOT] Cámara OV2640 lista.");
    }
    else
    {
        Serial.println("[ERROR] No se detecta la cámara. REVISA CABLE FLEX.");
        // Bucle infinito de error para proteger el hardware
        while (true)
        {
            delay(1000);
        }
    }

    // 5. INICIAR STACK DE RED
    // Proceso bloqueante (~10s máx) que conecta al WiFi o crea el AP.
    network.begin();

    // 6. ARRANCAR SERVICIOS EN SEGUNDO PLANO
    camera.startServer(); // Servidor Web asíncrono (Puerto 80)
    remote.begin();       // Escucha UDP (Puerto 9999)

    // REPORTE DE ESTADO FINAL
    Serial.println("\n[BOOT] SISTEMA ONLINE - ROVER LISTO.");
    Serial.printf("[INFO] Video Stream: http://%s.local/stream\n", MDNS_NAME);
    Serial.printf("[INFO] Control UDP:  Puerto %d\n", UDP_PORT);
}

// =============================================================================
// LOOP (Bucle Principal No Bloqueante)
// =============================================================================
void loop()
{
    // 1. Mantenimiento de Red
    // (Actualmente pasivo gracias a FreeRTOS, reservado para lógica futura)
    network.update();

    // 2. Proceso de Control (Tiempo Real)
    // Lee buffer UDP, decodifica protocolo y actualiza motores/servo.
    remote.listen();

    // 3. Sistema de Seguridad (Watchdog)
    // Comprueba si se ha perdido la conexión con el piloto.
    remote.checkFailsafe();

    // 4. Telemetría (Heartbeat)
    // Imprime estado cada 5 segundos sin usar delay() para no bloquear el control.
    static unsigned long lastTime = 0;
    if (millis() - lastTime > 5000)
    {
        lastTime = millis();
        Serial.printf("[ALIVE] Modo: %s | IP: %s | Uptime: %lu s\n",
                      network.getMode().c_str(),
                      network.getIP().c_str(),
                      millis() / 1000);
    }
}