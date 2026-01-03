/**
 * @file CameraServer.h
 * @brief Camera Hardware Driver (OV2640) and MJPEG Web Streaming Server.
 * @author Alejandro Moyano (@AleSMC)
 * @version 1.0.0
 * @details
 * Encapsulates low-level management of 'esp_camera' and 'esp_http_server'.
 * Implements buffering strategies for smooth streaming and automatically adapts
 * to the presence of PSRAM.
 */

#pragma once
#include <Arduino.h>
#include "esp_camera.h"
#include "esp_http_server.h"
#include "config.h"

class CameraServer
{
private:
    httpd_handle_t _httpServer; // Web server handler (C-Style pointer)

public:
    /**
     * @brief Default Constructor.
     * Initializes internal pointers to NULL.
     */
    CameraServer();

    /**
     * @brief Initializes the OV2640 sensor and I2S/DMA bus.
     * @details Configures the necessary 16 pins, the XCLK clock (set to 15MHz for stability),
     * and assigns memory buffers (DMA/PSRAM) for video.
     * @return true if the camera starts and detects the sensor, false if it fails.
     * @note In case of failure, check physical connections and pin configuration in config.h.
     */
    bool init();

    /**
     * @brief Starts the asynchronous HTTP server on port 80.
     * @details Registers the '/stream' route that serves the MJPEG content.
     */
    void startServer();

    /**
     * @brief Static callback to serve the video stream.
     * @details
     * Must be static because the ESP-IDF API (pure C) does not support
     * instance methods (C++).
     * @param req Incoming HTTP request structure.
     * @return esp_err_t Operation status (ESP_OK or Error).
     */
    static esp_err_t streamHandler(httpd_req_t *req);
};
