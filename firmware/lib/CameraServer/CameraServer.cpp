/**
 * @file CameraServer.cpp
 * @brief Implementación del Streaming de Video MJPEG de baja latencia.
 * @author Alejandro Moyano (@AleSMC)
 * @details
 * Utiliza el protocolo 'multipart/x-mixed-replace' para enviar una secuencia infinita
 * de imágenes JPEG sobre una única conexión HTTP persistente.
 */

#include "CameraServer.h"

// =============================================================================
// DEFINICIÓN DE PINES (MODELO AI THINKER ESP32-CAM)
// =============================================================================
// Mapeo físico estricto para la placa ESP32-CAM estándar.
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
// CONSTANTES DEL PROTOCOLO MJPEG
// =============================================================================
// El "Boundary" es una cadena de texto arbitraria que sirve de separador entre fotos.
#define PART_BOUNDARY "123456789000000000000987654321"

// Cabeceras HTTP precalculadas para eficiencia
static const char *_STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char *_STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char *_STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";

CameraServer::CameraServer()
{
    _httpServer = NULL;
}

bool CameraServer::init()
{
    // 1. Configuración Hardware del Sensor OV2640
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
    config.pin_sscb_sda = SIOD_GPIO_NUM;
    config.pin_sscb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.xclk_freq_hz = 20000000;       // 20MHz (Estable)
    config.pixel_format = PIXFORMAT_JPEG; // El hardware comprime a JPEG nativamente

    // --- 2. OPTIMIZACIÓN PARA STREAMING FLUIDO (RC) ---
    // Usamos QVGA (320x240) para garantizar >25 FPS y baja latencia (<100ms),
    // ofrece el mejor equilibrio calidad/velocidad para FPV.
    // Resoluciones mayores (SVGA/HD) introducen lag inaceptable para conducción.
    config.frame_size = FRAMESIZE_QVGA;
    config.jpeg_quality = 15; // Rango 0-63 (10-15 es ideal para streaming fluido)
    config.fb_count = 2;      // Doble Buffer: Clave para mantener FPS altos

    // 3. Verificación de Memoria Externa (PSRAM)
    // El video requiere mucha RAM. Si no hay PSRAM, bajamos la calidad para no crashear.
    // Chip ESP32 tiene poca RAM interna. La PSRAM (4MB) es vital para video.
    if (psramFound())
    {
        Serial.println("[CAM] PSRAM detectada. Modo Alto Rendimiento activado.");
    }
    else
    {
        Serial.println("[CAM] WARNING: No PSRAM. Reduciendo buffers.");
        config.frame_size = FRAMESIZE_QVGA;
        config.jpeg_quality = 18;
        config.fb_count = 1; // Sin PSRAM solo cabe 1 frame
    }

    // 4. Iniciar Driver
    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK)
    {
        Serial.printf("[ERROR] Fallo init cámara: 0x%x\n", err);
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

    // Cabecera inicial del stream
    res = httpd_resp_set_type(req, _STREAM_CONTENT_TYPE);
    if (res != ESP_OK)
        return res;

    // Bucle de transmisión infinita
    while (true)
    {
        // A. Capturar Frame (Bloqueante)
        fb = esp_camera_fb_get();
        if (!fb)
        {
            Serial.println("[ERROR] Frame corrupto o cámara desconectada");
            res = ESP_FAIL;
        }
        else
        {
            // B. Enviar Boundary y Headers del Frame
            if (res == ESP_OK)
            {
                res = httpd_resp_send_chunk(req, _STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY));
            }
            // C. Enviar Cabecera de la Imagen actual (Content-Type y Length)
            if (res == ESP_OK)
            {
                size_t hlen = snprintf((char *)part_buf, 64, _STREAM_PART, fb->len);
                res = httpd_resp_send_chunk(req, (const char *)part_buf, hlen);
            }
            // D. Enviar Payload (Imagen JPEG)
            if (res == ESP_OK)
            {
                res = httpd_resp_send_chunk(req, (const char *)fb->buf, fb->len);
            }

            // E. Liberar buffer para la siguiente captura
            esp_camera_fb_return(fb);
            fb = NULL;
        }

        // Si el cliente cierra la conexión, salimos del bucle
        if (res != ESP_OK)
            break;
    }
    return res;
}

void CameraServer::startServer()
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.server_port = HTTP_PORT; // Puerto 80 (definido en config.h)

    // Definición de la ruta URI
    httpd_uri_t stream_uri = {
        .uri = "/stream", // URL: http://ip/stream
        .method = HTTP_GET,
        .handler = streamHandler, // Función estática que gestiona el bucle
        .user_ctx = NULL};

    Serial.printf("[CAM] Servidor HTTP escuchando en puerto %d\n", config.server_port);

    // Iniciar servidor httpd ligero de Espressif
    if (httpd_start(&_httpServer, &config) == ESP_OK)
    {
        httpd_register_uri_handler(_httpServer, &stream_uri);
        Serial.println("[CAM] Endpoint registrado: /stream");
    }
    else
    {
        Serial.println("[ERROR] Fallo al iniciar servidor HTTP");
    }
}