"""
MAIN CLIENT - ESP32 ROVER
-------------------------
Main Orchestrator. Bridges Video and Control.
Run this file to start the system.
"""

import cv2
import socket
import time
import numpy as np

# --- MODULAR IMPORTS ---
from modules.VideoStream import VideoStream
from modules.KeyboardPilot import KeyboardPilot

# --- CONFIGURATION ---
# [CRITICAL] SET YOUR ROVER IP HERE
# 1. AP Mode (Default): "192.168.4.1" (If you connect to 'Rover-Emergency' WiFi)
# 2. STA Mode (Home WiFi): Check the ESP32 Serial Monitor (Baud 115200) to see the assigned IP.
ROVER_IP = "192.168.4.1"

# Video URL (Default Port 80 per firmware config)
VIDEO_URL = f"http://{ROVER_IP}/stream" 

# UDP Control Port
UDP_PORT = 9999

# Control Frequency (5Hz = Eco Mode / Stable)
SEND_INTERVAL_MS = 200 

def main():
    print(f"--- STARTING ROVER SYSTEM ---")
    print(f"Target IP: {ROVER_IP}")
    
    # 1. Setup UDP Network
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    
    # 2. Start Video Module
    print("Connecting to camera...")
    try:
        # Instantiate the imported class
        vs = VideoStream(VIDEO_URL).start()
        time.sleep(2.0) # Sensor warmup
    except Exception as e:
        print(f"[ERROR] Could not connect to video: {e}")
        return

    # 3. Start Pilot Module
    pilot = KeyboardPilot()
    
    # 4. Configure GUI
    window_name = "ESP32 Rover Commander"
    cv2.namedWindow(window_name, cv2.WINDOW_NORMAL)
    
    last_send_time = 0
    print(">> SYSTEM ONLINE. Controls active (WASD + SHIFT + SPACE).")

    try:
        while True:
            # --- A. VIDEO MANAGEMENT ---
            frame = vs.read()
            
            if frame is None:
                # Wait background if signal lost
                frame = np.zeros((240, 320, 3), dtype=np.uint8)
            #else:
                # 2x Scaling for better PC viewing (Optional)
                # [OPTIMIZATION] Resize commented out to gain response speed
                # frame = cv2.resize(frame, (640, 480), interpolation=cv2.INTER_NEAREST)

            cv2.imshow(window_name, frame)

             # --- B. WINDOW CLOSE CONTROL ---
            if (cv2.waitKey(1) & 0xFF) == 27: 
                break  # ESC to exit

            # --- C. NETWORK CONTROL (Rate Limiting) ---
            current_time = time.time() * 1000
            
            if (current_time - last_send_time) > SEND_INTERVAL_MS:
                packet = pilot.get_packet()
                sock.sendto(packet, (ROVER_IP, UDP_PORT))
                last_send_time = current_time
    except KeyboardInterrupt:
        print("\n[INFO] User interruption.")
    finally:
        print("[SHUTDOWN] Stopping Rover and releasing resources...")
        # Send stop command multiple times to ensure reception
        stop_cmd = bytes([1, 90])
        for _ in range(3): 
            sock.sendto(stop_cmd, (ROVER_IP, UDP_PORT))
            time.sleep(0.05)
            
        vs.stop()
        cv2.destroyAllWindows()
        print("[SHUTDOWN] Completed.")

if __name__ == "__main__":
    main()