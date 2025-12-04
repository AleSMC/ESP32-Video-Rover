# 🔧 Guía de Montaje de Hardware

## 1. Tabla de Conexiones (Netlist Maestra)

**Esta tabla es la FUENTE DE LA VERDAD.** Si el diagrama visual contradice esta tabla, **haz caso a la tabla**.

| Componente          | Pin Componente  | Pin ESP32-CAM | Notas Técnicas                      |
| :------------------ | :-------------- | :------------ | :---------------------------------- |
| **L298N (Motor A)** | IN1             | **GPIO 14**   | Rueda Izquierda                     |
| **L298N (Motor A)** | IN2             | **GPIO 15**   | Rueda Izquierda                     |
| **L298N (Motor B)** | IN3             | **GPIO 13**   | Rueda Derecha                       |
| **L298N (Motor B)** | IN4             | **GPIO 12**   | ⚠️ _Ver nota abajo sobre Boot Fail_ |
| **Servo Dirección** | Señal (Naranja) | **GPIO 2**    | Comparte pin con LED Flash          |
| **Energía**         | Salida 5V       | **Pin 5V**    | No usar el pin 3.3V                 |
| **Energía**         | GND             | **Pin GND**   | **CRÍTICO:** Unir Tierras           |

## 2. Diagrama Visual de Componentes

> ⚠️ **ADVERTENCIA:** La siguiente imagen muestra la ubicación de componentes, pero **LOS CABLES EN LA IMAGEN ESTÁN MAL**. Úsala solo como referencia visual de piezas, pero conecta los cables **según la tabla de arriba**.

![Diagrama de Componentes](DiagramaComponentes.png)

## 3. Detalles Críticos

### A. Modificación de Antena

Para usar la antena externa y tener buen video:

1. Localizar el conector IPEX junto al módulo metálico.
2. Verificar la resistencia de 0-ohm.
3. **Acción:** Debe estar soldada en la posición que conecta hacia el conector IPEX, deshabilitando la antena de PCB.

### B. Problema del GPIO 12 (Boot Fail)

El GPIO 12 es un "Strapping Pin" (MTDI). Determina el voltaje interno de la memoria flash al arrancar.

- **Síntoma:** El ESP32 entra en bucle de reinicios o da error "Flash voltage mismatch".
- **Causa:** El driver L298N mantiene este pin en estado ALTO (HIGH) durante el arranque.
- **Solución:** Si no arranca, desconecta el cable del GPIO 12 momentáneamente, pulsa Reset, y vuélvelo a conectar cuando el código haya iniciado.
