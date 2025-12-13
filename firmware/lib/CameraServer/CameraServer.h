/**
 * @file CameraServer.h
 * @brief Driver de hardware de cámara (OV2640) y Servidor Web de Streaming MJPEG.
 * @author Alejandro Moyano (@AleSMC)
 * @version 1.0.0
 * * @details
 * Esta clase encapsula la complejidad de la librería 'esp_camera' y 'esp_http_server'.
 * Proporciona una interfaz simple para inicializar el sensor y levantar un servidor web
 * que emite video en formato "Motion JPEG" (secuencia de fotos).
 */

#pragma once
#include <Arduino.h>
#include "esp_camera.h"
#include "esp_http_server.h"
#include "config.h"

class CameraServer
{
private:
    httpd_handle_t _httpServer; // Puntero al manejador del servidor web (esp_http_server)

public:
    /**
     * @brief Constructor por defecto.
     * Inicializa los punteros internos a NULL.
     */
    CameraServer();

    /**
     * @brief Inicializa el hardware de la cámara (OV2640).
     * @details Configura los 16 pines necesarios, el reloj XCLK a 20MHz y
     * asigna los buffers de memoria (DMA/PSRAM) para el video.
     * @return true si la cámara arranca y detecta el sensor, false si falla.
     */
    bool init();

    /**
     * @brief Levanta el servidor web en el puerto 80.
     * @details Registra la ruta '/stream' que servirá el contenido MJPEG.
     */
    void startServer();

    /**
     * @brief Manejador de la petición HTTP GET /stream.
     * @warning Debe ser 'static' porque la librería C 'esp_http_server' requiere
     * un puntero a función simple, no puede invocar métodos de instancia C++.
     * @param req Estructura de la petición HTTP entrante.
     * @return esp_err_t Estado de la operación (ESP_OK o Error).
     */
    static esp_err_t streamHandler(httpd_req_t *req);
};