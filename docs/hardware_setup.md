# 🔧 Guía de Montaje de Hardware

## 1. Tabla de Conexiones (Netlist Maestra - Modo Eje Sólido)

**Esta tabla es la FUENTE DE LA VERDAD.** Si el diagrama visual contradice esta tabla, **haz caso a la tabla**.

Para esta configuración, se deben retirar los Jumpers `ENA` y `ENB` del driver L298N y realizar puentes físicos entre los pines de control para unificar la tracción.

| Función Lógica          | Pin ESP32-CAM | Pines L298N (Puenteados) | Descripción Técnica                                    |
| :---------------------- | :------------ | :----------------------- | :----------------------------------------------------- |
| **Velocidad (PWM)**     | **GPIO 13**   | **ENA** + **ENB**        | Control de potencia global. Permite Coasting si PWM=0. |
| **Sentido Avance**      | **GPIO 14**   | **IN1** + **IN3**        | Activa ambos motores hacia adelante.                   |
| **Sentido Retroceso**   | **GPIO 15**   | **IN2** + **IN4**        | Activa ambos motores hacia atrás.                      |
| **Servo Dirección**     | **GPIO 2**    | **Señal PWM**            | Cable Naranja/Amarillo del Servo.                      |
| **Alimentación Lógica** | **Pin 5V**    | **5V Out**               | Alimentación del ESP32 desde el regulador del L298N.   |
| **Tierra Común**        | **Pin GND**   | **GND**                  | **CRÍTICO:** Referencia común de voltaje.              |
| **Reservado**           | **GPIO 12**   | _Desconectado_           | Ver sección de limitaciones abajo.                     |

## 2. Diagrama Visual de Componentes

> ⚠️ **ADVERTENCIA:** La siguiente imagen muestra la ubicación de componentes, pero **LOS CABLES EN LA IMAGEN ESTÁN MAL**. Úsala solo como referencia visual de piezas, pero conecta los cables **según la tabla de arriba**.

![Diagrama de Componentes](DiagramaComponentes.png)

### Notas sobre el Driver L298N y Alimentación

- **Batería del Proyecto:** LiPo 3S (11.1V) 2200mAh (Ref: ELL-MAX).
  - _Nota Técnica:_ El uso de una batería de 11.1V (12.6V a plena carga) incrementa la carga térmica del regulador lineal de 5V integrado. Aunque la operación es segura, **se recomienda garantizar una ventilación adecuada** alrededor del disipador térmico para facilitar la disipación pasiva.
- **Alimentación de Potencia:** Batería (+) a 12V y Batería (-) a GND.

## 3. Limitaciones Técnicas y Reservas de Pines

### A. Modificación de Antena

Para usar la antena externa y optimizar el rango de video:

1. Localizar el conector IPEX junto al módulo metálico.
2. Verificar la resistencia de 0-ohm.
3. **Acción:** Debe estar soldada en la posición que conecta hacia el conector IPEX, deshabilitando la antena de PCB.

### B. Restricción del GPIO 12 (Strapping Pin)

El GPIO 12 determina el voltaje interno de la memoria flash (VDD_SDIO) durante el arranque (Boot).

- **Riesgo:** Si este pin se encuentra en estado ALTO (HIGH) durante el reinicio, el ESP32 configurará el voltaje de flash a 1.8V en lugar de 3.3V, provocando un fallo de arranque ("Flash voltage mismatch").
- **Decisión de Diseño:** En la fase actual (MVP), este pin se deja **DESCONECTADO** para garantizar la estabilidad del sistema y evitar la necesidad de desconectar cables manualmente en cada reinicio.
- **Futuro (I+D):** Se evaluará su uso para implementar un diferencial electrónico en fases avanzadas, considerando circuitos de aislamiento o pull-down externos.
