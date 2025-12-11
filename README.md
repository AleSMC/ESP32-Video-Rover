# 🚜 ESP32-Video-Rover

> **Vehículo RC WiFi híbrido con transmisión de video MJPEG y control UDP.**

Este proyecto implementa un rover controlado remotamente utilizando un **ESP32-CAM** (AI Thinker). El sistema opera bajo una topología de **Eje Sólido Electrónico** (Tracción trasera unificada) para garantizar estabilidad de arranque y eficiencia de recursos, con dirección Ackermann mediante servomotor.

## 📂 Estructura del Proyecto

    ESP32-Video-Rover/
    ├── firmware/               # Código fuente C++ (PlatformIO)
    │   ├── src/                # Lógica principal (.cpp)
    │   ├── include/            # Cabeceras (.h) y Configuración
    │   ├── lib/                # Librerías de Hardware
    │   │   └── SolidAxle/      # Driver de tracción (Topología Eje Sólido)
    |   |   └── SteeringServo/  # Driver de dirección (Servo Ackermann)
    │   ├── examples/           # Tests unitarios preservados (Motores, Servo, LED)
    │   └── platformio.ini      # Configuración del entorno de compilación
    ├── software/               # Cliente PC (Python + OpenCV + UDP)
    ├── docs/                   # Documentación técnica, diagramas y notas
    └── README.md               # Este archivo

## 🛠 Hardware y Conexiones

**Plataforma:** ESP32-CAM (Modelo AI Thinker) con antena externa modificada.
**Topología de Tracción:** Paralelo (Solid Axle). Ambos motores traseros reciben la misma señal PWM y Dirección.

> ℹ️ **Detalles Completos:** Ver guía de montaje, netlist y advertencias en [docs/hardware_setup.md](docs/hardware_setup.md).

| Señal Lógica        | Pin ESP32 | Conexión L298N | Notas Técnicas                           |
| :------------------ | :-------- | :------------- | :--------------------------------------- |
| **PWM (Velocidad)** | GPIO 13   | ENA + ENB      | Puenteado. Control de Potencia (0-100%). |
| **Dirección Fwd**   | GPIO 14   | IN1 + IN3      | Puenteado. Marcha Adelante.              |
| **Dirección Rev**   | GPIO 15   | IN2 + IN4      | Puenteado. Marcha Atrás.                 |
| **Servo Dirección** | GPIO 2    | PWM Signal     | Comparte línea con LED Flash.            |
| **Reservado (I+D)** | GPIO 12   | **NC**         | _No Conectado_ para evitar Boot Fail.    |

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

> **⚠️ NOTA DE SEGURIDAD (REVERSA):**
> La lógica de reversa está **deshabilitada en el firmware base** (Fase A) para prevenir picos de corriente (Back-EMF). La implementación de reversa segura (con Dynamic Dead Time) se gestionará desde el Cliente Python en fases avanzadas.

## ✅ Roadmap de Desarrollo

- [x] **Paso 0:** Configuración de Entorno y GitOps.
- [x] **Paso A:** Implementación de Driver de Motores (Topología Eje Sólido con PWM). _Implementado FWD/Brake/Coast._
- [x] **Paso B:** Control de Servo de Dirección.
- [ ] **Paso C:** Stack de Red (WiFi + mDNS + Video).
- [ ] **Paso D:** Protocolo de Control UDP.
- [ ] **Paso E:** Cliente Python (PC).
  - Implementación de Video y Control Básico.
  - Implementación de **"Caja de Cambios"** (Shift=Lento, Espacio=Turbo, Nada=Normal).
- [ ] **Paso EXTRA (Bonus):** Control de Reversa Dinámica.
  - Implementar lógica de seguridad en Python para calcular el tiempo de frenado necesario según la velocidad previa antes de enviar el comando de reversa.
- [ ] **Fase I+D (Bonus):** Investigación de Diferencial Electrónico. Evaluar viabilidad de uso seguro del GPIO 12 (Strapping Pin) para control independiente de motores.

---

**Licencia:** MIT License. Ver archivo `LICENSE` para el texto completo.
