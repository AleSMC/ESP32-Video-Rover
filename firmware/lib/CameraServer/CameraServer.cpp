/**
 * @file CameraServer.cpp
 * @brief Implementation of Low Latency MJPEG Video Streaming.
 * @author Alejandro Moyano (@AleSMC)
 * @details
 * Uses the 'multipart/x-mixed-replace' protocol to send an infinite sequence
 * of JPEG images over a single persistent HTTP connection.
 */

#include "CameraServer.h"

// =============================================================================
// PIN DEFINITIONS (AI THINKER ESP32-CAM MODEL)
// =============================================================================
// Strict physical mapping for standard ESP32-CAM board.
#define PWDN_GPIO_NUM 32
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM 0
#define SIOD_GPIO_NUM 26
#define SIOC_GPIO_NUM 27

#define Y9_GPIO_NUM 35
#define Y8_GPIO_NUM 34
#define Y7_GPIO_NUM 39
#define Y6_GPIO_NUM 36
#define Y5_GPIO_NUM 21
#define Y4_GPIO_NUM 19
#define Y3_GPIO_NUM 18
#define Y2_GPIO_NUM 5
#define VSYNC_GPIO_NUM 25
#define HREF_GPIO_NUM 23
#define PCLK_GPIO_NUM 22

// =============================================================================
// MJPEG PROTOCOL CONSTANTS
// =============================================================================
// The "Boundary" is an arbitrary string that serves as a separator between photos.
#define PART_BOUNDARY "123456789000000000000987654321"

// Precalculated HTTP headers for efficiency
static const char *_STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char *_STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char *_STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";

CameraServer::CameraServer()
{
    _httpServer = NULL;
}

bool CameraServer::init()
{
    // 1. OV2640 Sensor Hardware Configuration
    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_sccb_sda = SIOD_GPIO_NUM;
    config.pin_sccb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;

    // LOWER CLOCK SPEED (CRITICAL)
    // Default is 20000000 (20MHz).
    // Lowering it to 15MHz forces the camera to process slower physically.
    // This limits FPS to stable 10-15, freeing up CPU and WiFi bandwidth.
    config.xclk_freq_hz = 15000000;       // 15MHz
    config.pixel_format = PIXFORMAT_JPEG; // Hardware compresses to JPEG natively

    // --- 2. OPTIMIZATION FOR SMOOTH STREAMING (RC) ---
    // We use QVGA (320x240) to guarantee >25 FPS and low latency (<100ms),
    // offering the best balance of quality/speed for FPV.
    // Higher resolutions (SVGA/HD) introduce unacceptable lag for driving.
    config.frame_size = FRAMESIZE_QVGA;
    config.jpeg_quality = 60; // Range 0-63 (60 is very low quality -> high compression -> fast)

    // 3. External Memory Verification (PSRAM)
    // Video requires a lot of RAM. If no PSRAM, lower quality to prevent crashes.
    // ESP32 chip has little internal RAM. PSRAM (4MB) is vital for video.
    if (psramFound())
    {
        // Serial.println("[CAM] PSRAM detected. High Performance Mode activated.");
        // config.grab_mode = CAMERA_GRAB_LATEST; // Always the most recent frame
        // config.fb_count = 2;                   // Double Buffer: Key to maintain high FPS

        // SINGLE BUFFER MODE (ZERO LATENCY)
        config.fb_count = 1;                       // Only 1 frame buffer
        config.grab_mode = CAMERA_GRAB_WHEN_EMPTY; // Wait until buffer is free
    }
    else
    {
        Serial.println("[CAM] WARNING: No PSRAM. Reducing buffers.");
        config.fb_count = 1;                       // Without PSRAM only 1 frame fits
        config.grab_mode = CAMERA_GRAB_WHEN_EMPTY; // No memory to discard frames.
    }

    // 4. Initialize Driver
    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK)
    {
        Serial.printf("[ERROR] Camera Init Failed: 0x%x\n", err);
        return false;
    }

    return true;
}

esp_err_t CameraServer::streamHandler(httpd_req_t *req)
{
    camera_fb_t *fb = NULL;
    esp_err_t res = ESP_OK;
    size_t _jpg_buf_len = 0;
    uint8_t *_jpg_buf = NULL;
    char *part_buf[64];

    // Initial stream header
    res = httpd_resp_set_type(req, _STREAM_CONTENT_TYPE);
    if (res != ESP_OK)
        return res;

    // Infinite transmission loop
    while (true)
    {
        // A. Capture Frame (Blocking)
        fb = esp_camera_fb_get();
        if (!fb)
        {
            Serial.println("[ERROR] Corrupt frame or camera disconnected");
            res = ESP_FAIL;
        }
        else
        {
            // B. Send Boundary and Frame Headers
            if (res == ESP_OK)
            {
                res = httpd_resp_send_chunk(req, _STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY));
            }
            // C. Send Current Image Header (Content-Type and Length)
            if (res == ESP_OK)
            {
                size_t hlen = snprintf((char *)part_buf, 64, _STREAM_PART, fb->len);
                res = httpd_resp_send_chunk(req, (const char *)part_buf, hlen);
            }
            // D. Send Payload (JPEG Image)
            if (res == ESP_OK)
            {
                res = httpd_resp_send_chunk(req, (const char *)fb->buf, fb->len);
            }

            // E. Free buffer for next capture
            esp_camera_fb_return(fb);
            fb = NULL;
        }

        // If client closes connection, break loop
        if (res != ESP_OK)
            break;

        // --- STABILITY (THROTTLING) ---
        // Pause for 20ms to let WiFi breathe.
        // This allows UDP packets to enter without colliding with video.
        // Mathematically: Transmission + 20ms pause = approx 15-20 stable FPS.
        delay(20);
    }
    return res;
}

void CameraServer::startServer()
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.server_port = HTTP_PORT; // Port 80 (defined in config.h)

    // URI Route Definition
    httpd_uri_t stream_uri = {
        .uri = "/stream", // URL: http://ip/stream
        .method = HTTP_GET,
        .handler = streamHandler, // Static function managing the loop
        .user_ctx = NULL};

    Serial.printf("[CAM] HTTP Server listening on port %d\n", config.server_port);

    // Start Espressif lightweight httpd server
    if (httpd_start(&_httpServer, &config) == ESP_OK)
    {
        httpd_register_uri_handler(_httpServer, &stream_uri);
        Serial.println("[CAM] Endpoint registered: /stream");
    }
    else
    {
        Serial.println("[ERROR] Failed to start HTTP Server");
    }
}
