# Instrucciones del Proyecto: ESP32-Video-Rover

Eres el asistente de IA para el proyecto "ESP32-Video-Rover".
Tu objetivo es ayudar a escribir firmware para ESP32-CAM (C++ PlatformIO) y cliente PC (Python OpenCV).

## Reglas Críticas de Hardware (INMUTABLES)

1.  **Modelo de Cámara:** AI THINKER. Debes asumir `build_flags = -DCAMERA_MODEL_AI_THINKER`.
2.  **Pines de Motores (L298N):**
    - IN1: GPIO 14
    - IN2: GPIO 15
    - IN3: GPIO 13
    - IN4: GPIO 12 (Advertencia: Strapping Pin. Si falla boot, avisar desconexión).
3.  **Pin de Servo:** GPIO 2 (Comparte LED flash/status).
4.  **Restricción SD:** NO se usa tarjeta microSD. Los pines se reutilizan para motores.
5.  **Brownout:** El código de setup SIEMPRE debe desactivar el detector de Brownout: `WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);`.

## Arquitectura de Red

- **Modo Híbrido:** El código debe intentar conectar a STA (WiFi hogar) y si falla, crear AP ("Rover-Emergency").
- **mDNS:** Usar siempre `rover.local` (definido por macro `-D MDNS_NAME`).
- **Protocolo:** Video via HTTP (MJPEG) y Control via UDP.
- **Seguridad (Failsafe):** Si no llegan paquetes UDP en 500ms, los motores deben parar (PWM=0).

## Estándares de Código y GitOps

1.  **Documentación (Doxygen):** - Todos los archivos, funciones y clases deben tener comentarios estilo Doxygen (`/** ... */`).
    - Usar etiquetas: `@file`, `@brief`, `@param`, `@return`, `@note`.
    - Idioma: **ESPAÑOL**.
2.  **Estructura Modular:** Credenciales en `include/secrets.h` y constantes en `include/config.h`.
3.  **Preservación de Ejemplos:** - Al crear un test de hardware funcional (ej: test de servo), NUNCA borrarlo.
    - Mover el código funcional a la carpeta `firmware/examples/` antes de continuar con la siguiente feature en `src/main.cpp`.

## Python (Cliente)

- Usar `cv2` para mostrar imagen.
- Usar `socket` para UDP en hilos separados (threading).
