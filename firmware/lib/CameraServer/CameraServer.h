/**
 * @file CameraServer.h
 * @brief Driver de hardware de cámara (OV2640) y Servidor Web de Streaming MJPEG.
 * @author Alejandro Moyano (@AleSMC)
 * @version 1.0.0
 * @details
 * Encapsula la gestión de bajo nivel de 'esp_camera' y 'esp_http_server'.
 * Implementa doble buffer para streaming fluido y se adapta automáticamente
 * a la presencia de PSRAM.
 */

#pragma once
#include <Arduino.h>
#include "esp_camera.h"
#include "esp_http_server.h"
#include "config.h"

class CameraServer
{
private:
    httpd_handle_t _httpServer; // Manejador del servidor web (C-Style pointer)

public:
    /**
     * @brief Constructor por defecto.
     * Inicializa los punteros internos a NULL.
     */
    CameraServer();

    /**
     * @brief Inicializa el sensor OV2640 y el bus I2S/DMA.
     * @details Configura los 16 pines necesarios, el reloj XCLK a 20MHz y
     * asigna los buffers de memoria (DMA/PSRAM) para el video.
     * @return true si la cámara arranca y detecta el sensor, false si falla.
     * @note En caso de fallo, revisar conexiones físicas y configuración de pines en config.h.
     */
    bool init();

    /**
     * @brief Inicia el servidor HTTP asíncrono en el puerto 80.
     * @details Registra la ruta '/stream' que servirá el contenido MJPEG.
     */
    void startServer();

    /**
     * @brief Callback estático para servir el flujo de video.
     * @details
     * Debe ser estático porque la API de ESP-IDF (C puro) no soporta
     * métodos de instancia (C++).
     * @param req Estructura de la petición HTTP entrante.
     * @return esp_err_t Estado de la operación (ESP_OK o Error).
     */
    static esp_err_t streamHandler(httpd_req_t *req);
};