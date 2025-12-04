# ESP32-Video-Rover

> **Vehículo RC WiFi híbrido con transmisión de video MJPEG y control UDP.**

Este proyecto implementa un rover controlado remotamente utilizando un **ESP32-CAM** (AI Thinker). El sistema es capaz de funcionar en modo Estación (conectado al WiFi de casa) o Punto de Acceso (emergencia), transmitiendo video en baja latencia y recibiendo comandos de control vía UDP.

## 📂 Estructura del Proyecto

    ESP32-Video-Rover/
    ├── firmware/           # Código fuente C++ (PlatformIO)
    │   ├── src/            # Lógica principal (.cpp)
    │   ├── include/        # Cabeceras (.h) y Configuración
    │   ├── examples/       # Tests de hardware (Motores, Servo, LED)
    │   └── platformio.ini  # Configuración del entorno de compilación
    ├── software/           # Cliente PC (Python + OpenCV + UDP)
    ├── docs/               # Documentación técnica, diagramas y notas
    └── README.md           # Este archivo

## 🛠 Hardware y Conexiones

**Plataforma:** ESP32-CAM (Modelo AI Thinker) con antena externa modificada.

> ℹ️ **Detalles Completos:** Ver guía de montaje y diagramas en [docs/hardware_setup.md](docs/hardware_setup.md).

| Componente          | Pin ESP32 | Función | Notas Técnicas                                             |
| :------------------ | :-------- | :------ | :--------------------------------------------------------- |
| **Motor A (L298N)** | GPIO 14   | IN1     |                                                            |
| **Motor A (L298N)** | GPIO 15   | IN2     |                                                            |
| **Motor B (L298N)** | GPIO 13   | IN3     |                                                            |
| **Motor B (L298N)** | GPIO 12   | IN4     | **⚠️ Strapping Pin**: Debe estar flotante/LOW al arrancar. |
| **Servo Dirección** | GPIO 2    | PWM     | Comparte línea con LED Flash.                              |
| **Alimentación**    | 5V / GND  | VCC     | **CRÍTICO:** GND común entre L298N y ESP32.                |

> **Nota:** Se ha desactivado el _Brownout Detector_ por software para evitar reinicios debido a picos de consumo de los motores.

## 🚀 Inicio Rápido (Firmware)

### Prerrequisitos

- VSCode con extensión **PlatformIO**.
- Driver CH340 (si usas la base MB) o FTDI.

### Instalación

1. Clonar el repositorio.
2. Abrir la carpeta raíz en VSCode.
3. Crear el archivo de credenciales:
   Copiar `firmware/include/secrets_example.h` a `firmware/include/secrets.h` y rellenar con tu WiFi.

### Compilación y Carga

    # Desde la terminal de PlatformIO
    cd firmware
    pio run -t upload

_Si falla la subida:_ Mantén pulsado el botón `IO0` (o conecta GPIO0 a GND) y reinicia antes de subir.

### Monitorización

Para ver logs de depuración (IP asignada, estado de motores):

    pio device monitor -b 115200

## 📡 Arquitectura de Red

- **Modo Híbrido:** Intenta conectar a STA (WiFi Hogar). Si falla tras 10s, despliega AP `Rover-Emergency`.
- **Descubrimiento:** mDNS habilitado en `rover.local`.
- **Protocolos:**
  - **Video:** Servidor HTTP (Stream MJPEG).
  - **Control:** UDP (Puerto por defecto: `UDP_PORT` en config).
- **Seguridad (Failsafe):** Watchdog de 500ms. Si no se reciben paquetes UDP, los motores se detienen.

## ✅ Roadmap de Desarrollo

- [x] **Paso 0:** Configuración de Entorno y GitOps.
- [ ] **Paso A:** Implementación de Driver de Motores (L298N).
- [ ] **Paso B:** Control de Servo de Dirección.
- [ ] **Paso C:** Stack de Red (WiFi + mDNS + Video).
- [ ] **Paso D:** Protocolo de Control UDP.
- [ ] **Paso E:** Cliente Python (PC).

---

**Licencia:** MIT License. Ver archivo `LICENSE` para el texto completo.
