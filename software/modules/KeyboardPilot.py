"""
KeyboardPilot.py
----------------
Autor: Alejandro Moyano (@AleSMC)
Descripción: Lógica de pilotaje basada en hardware (pynput).
Replica EXACTAMENTE la lógica de prioridades del diseño original
pero usando eventos de teclado físico para eliminar el lag y permitir diagonales.
"""

from pynput import keyboard

class KeyboardPilot:
    def __init__(self):
        # --- CONFIGURACIÓN DE PARÁMETROS (Tus valores originales) ---
        
        # Mapeo de Velocidades PWM
        self.PWM_COAST = 0
        self.PWM_BRAKE = 1
        self.PWM_SLOW = 130   # Shift (Modo Precisión)
        self.PWM_NORMAL = 180 # Sin teclas (Modo Crucero)
        self.PWM_TURBO = 255  # Espacio + W (Modo Turbo)
        
        # Mapeo de Ángulos
        self.ANGLE_CENTER = 90
        self.ANGLE_LEFT = 70
        self.ANGLE_RIGHT = 110

        # --- ESTADO INTERNO ---
        # Usamos un set para guardar qué teclas están físicamente abajo ahora mismo
        self.pressed_keys = set()

        # Iniciamos el "Escucha" del teclado en segundo plano
        self.listener = keyboard.Listener(
            on_press=self._on_press,
            on_release=self._on_release)
        self.listener.start()

    def _on_press(self, key):
        """Se activa automáticamente al pulsar una tecla"""
        try:
            # Guardamos la letra (ej: 'w')
            if hasattr(key, 'char') and key.char:
                self.pressed_keys.add(key.char.lower())
            else:
                # Guardamos teclas especiales (Shift, Space, etc)
                self.pressed_keys.add(key)
        except Exception:
            pass

    def _on_release(self, key):
        """Se activa automáticamente al SOLTAR una tecla"""
        try:
            if hasattr(key, 'char') and key.char:
                self.pressed_keys.discard(key.char.lower())
            else:
                self.pressed_keys.discard(key)
        except Exception:
            pass

    def get_packet(self):
        """
        Traduce el estado físico del teclado a tu protocolo.
        Aplica TU LÓGICA exacta de resolución de conflictos.
        """
        # 1. Definir qué teclas están activas para facilitar la lectura
        k_w = 'w' in self.pressed_keys
        k_s = 's' in self.pressed_keys
        k_a = 'a' in self.pressed_keys
        k_d = 'd' in self.pressed_keys
        
        # Teclas especiales en pynput
        k_space = keyboard.Key.space in self.pressed_keys
        # Shift puede ser izquierdo o derecho
        k_shift = (keyboard.Key.shift in self.pressed_keys) or \
                  (keyboard.Key.shift_l in self.pressed_keys) or \
                  (keyboard.Key.shift_r in self.pressed_keys)

        # --- A. LÓGICA DE DIRECCIÓN (A vs D) ---
        angle_out = self.ANGLE_CENTER
        
        if k_a and not k_d:
            angle_out = self.ANGLE_LEFT
        elif k_d and not k_a:
            angle_out = self.ANGLE_RIGHT
        # Si pulsan las dos (A+D) o ninguna -> CENTER (90)

        # --- B. LÓGICA DE TRACCIÓN (W vs S vs Space) ---
        pwm_out = self.PWM_COAST

        # CASO 1: Freno Activo (S gana a W en tu lógica)
        if k_s:
            pwm_out = self.PWM_BRAKE
        
        # CASO 2: Avance (W pulsada y S no la bloquea)
        elif k_w:
            # Lógica de "Caja de Cambios"
            # Prioridad 1: Shift (Lento)
            if k_shift:
                pwm_out = self.PWM_SLOW
            # Prioridad 2: Space + W (Turbo)
            elif k_space:
                pwm_out = self.PWM_TURBO
            # Prioridad 3: Normal
            else:
                pwm_out = self.PWM_NORMAL
                
        # CASO 3: Freno de Mano (Espacio solo, sin W)
        elif k_space and not k_w:
            pwm_out = self.PWM_BRAKE
            
        # Retornamos el paquete listo
        return bytes([int(pwm_out), int(angle_out)])

    def stop(self):
        self.listener.stop()