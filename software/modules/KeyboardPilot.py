"""
KeyboardPilot.py
----------------
Author: Alejandro Moyano (@AleSMC)
Description: Hardware-based piloting logic (pynput).
Replicates EXACTLY the original priority logic but uses physical 
keyboard events to eliminate lag and allow diagonals.
"""

from pynput import keyboard

class KeyboardPilot:
    def __init__(self):
        # --- PARAMETER CONFIGURATION ---
        
        # PWM Speed Mapping
        self.PWM_COAST = 0
        self.PWM_BRAKE = 1
        
        # For PWM_SLOW: 130 is approximately 50% duty cycle (255 is 100%).
        # This causes a lot of electrical noise through the cables and triggers the servo.
        # With 90, it still introduces some noise, but less.
        # So we set it to 40 to minimize it until we separate the power sources
        # for the motors and the ESP32/Servo.
        self.PWM_SLOW = 40    # Shift (Precision Mode)
        
        self.PWM_NORMAL = 190 # No keys (Cruise Mode)
        self.PWM_TURBO = 255  # Space + W (Turbo Mode)
        
        # Angle Mapping
        self.ANGLE_CENTER = 90
        self.ANGLE_LEFT = 40   # Calibrated Limit
        self.ANGLE_RIGHT = 140 # Calibrated Limit

        # --- INTERNAL STATE ---
        # We use a set to store which keys are physically pressed right now
        self.pressed_keys = set()

        # Start keyboard listener in background
        self.listener = keyboard.Listener(
            on_press=self._on_press,
            on_release=self._on_release)
        self.listener.start()

    def _on_press(self, key):
        """Automatically triggered on key press"""
        try:
            # Save the character (e.g., 'w')
            if hasattr(key, 'char') and key.char:
                self.pressed_keys.add(key.char.lower())
            else:
                # Save special keys (Shift, Space, etc.)
                self.pressed_keys.add(key)
        except Exception:
            pass

    def _on_release(self, key):
        """Automatically triggered on key release"""
        try:
            if hasattr(key, 'char') and key.char:
                self.pressed_keys.discard(key.char.lower())
            else:
                self.pressed_keys.discard(key)
        except Exception:
            pass

    def get_packet(self):
        """
        Translates physical keyboard state to protocol.
        Applies EXACT conflict resolution logic.
        """
        # 1. Check active keys
        k_w = 'w' in self.pressed_keys
        k_s = 's' in self.pressed_keys
        k_a = 'a' in self.pressed_keys
        k_d = 'd' in self.pressed_keys
        
        # Special keys in pynput
        k_space = keyboard.Key.space in self.pressed_keys
        
        # Shift can be Left or Right key
        k_shift = (keyboard.Key.shift in self.pressed_keys) or \
                  (keyboard.Key.shift_l in self.pressed_keys) or \
                  (keyboard.Key.shift_r in self.pressed_keys)

        # --- A. STEERING LOGIC (A vs D) ---
        angle_out = self.ANGLE_CENTER
        
        if k_a and not k_d:
            angle_out = self.ANGLE_LEFT
        elif k_d and not k_a:
            angle_out = self.ANGLE_RIGHT
        # If both (A+D) or neither -> CENTER (90)

        # --- B. TRACTION LOGIC (W vs S vs Space) ---
        pwm_out = self.PWM_COAST

        # CASE 1: Active Brake (S wins over W)
        if k_s:
            pwm_out = self.PWM_BRAKE
        
        # CASE 2: Drive (W pressed and not blocked by S)
        elif k_w:
            # Gearbox Logic
            # Priority 1: Shift (Slow/Precision)
            if k_shift:
                pwm_out = self.PWM_SLOW
            # Priority 2: Space + W (Turbo)
            elif k_space:
                pwm_out = self.PWM_TURBO
            # Priority 3: Normal
            else:
                pwm_out = self.PWM_NORMAL
                
        # CASE 3: Handbrake (Space only, no W)
        elif k_space and not k_w:
            pwm_out = self.PWM_BRAKE
            
        # Return packet ready for UDP
        return bytes([int(pwm_out), int(angle_out)])

    def stop(self):
        self.listener.stop()
