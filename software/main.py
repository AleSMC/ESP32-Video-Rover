"""
MAIN CLIENT - ESP32 ROVER
-------------------------
Orquestador principal. Conecta Video y Control.
Ejecutar este archivo para iniciar el sistema.
"""

import cv2
import socket
import time
import numpy as np

# --- IMPORTACIONES MODULARES ---
from modules.VideoStream import VideoStream
from modules.KeyboardPilot import KeyboardPilot

# --- CONFIGURACIÓN ---
# Revisa el Monitor Serie del ESP32 para confirmar esta IP
ROVER_IP = "172.20.10.8" # "192.168.4.1" (AP) "172.20.10.8" (STA)

# URL del Video (Puerto 80 por defecto según tu firmware)
VIDEO_URL = f"http://{ROVER_IP}/stream" 

# Puerto de Control UDP
UDP_PORT = 9999

# Frecuencia de Control (5Hz = Eco Mode / Estable)
SEND_INTERVAL_MS = 200 

def main():
    print(f"--- INICIANDO SISTEMA ROVER ---")
    print(f"Target IP: {ROVER_IP}")
    
    # 1. Preparar Red UDP
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    
    # 2. Iniciar Módulo de Video
    print("Conectando con cámara...")
    try:
        # Instanciamos la clase importada
        vs = VideoStream(VIDEO_URL).start()
        time.sleep(2.0) # Calentamiento de sensor
    except Exception as e:
        print(f"[ERROR] No se pudo conectar al video: {e}")
        return

    # 3. Iniciar Módulo de Piloto
    pilot = KeyboardPilot()
    
    # 4. Configurar Interfaz Gráfica
    window_name = "ESP32 Rover Commander"
    cv2.namedWindow(window_name, cv2.WINDOW_NORMAL)
    
    last_send_time = 0
    print(">> SISTEMA ONLINE. Controles activos (WASD + SHIFT + SPACE).")

    try:
        while True:
            # --- A. GESTIÓN DE VIDEO ---
            frame = vs.read()
            
            if frame is None:
                # Fondo de espera si hay corte
                frame = np.zeros((240, 320, 3), dtype=np.uint8)
            #else:
                # Escalado x2 para mejor visualización en PC (Opcional)
                # [OPTIMIZACIÓN] Comentamos el resize para ganar velocidad de respuesta
                # frame = cv2.resize(frame, (640, 480), interpolation=cv2.INTER_NEAREST)

            cv2.imshow(window_name, frame)

             # --- B. CONTROL DE CIERRE DE VENTANA ---
            if (cv2.waitKey(1) & 0xFF) == 27: 
                break  # ESC para salir

            # --- C. CONTROL DE RED (Rate Limiting) ---
            current_time = time.time() * 1000
            
            if (current_time - last_send_time) > SEND_INTERVAL_MS:
                packet = pilot.get_packet()
                sock.sendto(packet, (ROVER_IP, UDP_PORT))
                last_send_time = current_time
    except KeyboardInterrupt:
        print("\n[INFO] Interrupción por usuario.")
    finally:
        print("[SHUTDOWN] Deteniendo Rover y liberando recursos...")
        # Enviar parada varias veces para asegurar recepción
        stop_cmd = bytes([1, 90])
        for _ in range(3): 
            sock.sendto(stop_cmd, (ROVER_IP, UDP_PORT))
            time.sleep(0.05)
            
        vs.stop()
        cv2.destroyAllWindows()
        print("[SHUTDOWN] Completado.")

if __name__ == "__main__":
    main()