"""
VideoStream.py
--------------
Módulo encargado de la recepción de video asíncrona.
Usa threading para evitar el bloqueo del hilo principal (Main Loop)
causado por la latencia de red o decodificación de OpenCV.
"""

import cv2
import threading

class VideoStream:
    def __init__(self, src=0):
        """
        Inicializa el stream de video.
        :param src: URL del video (ej: http://192.168.4.1/stream) o ID de cámara local (0).
        """
        self.stream = cv2.VideoCapture(src)

        # Limitamos el buffer interno a 1 frame.
        # Esto obliga a OpenCV a descartar frames viejos si no le da tiempo a procesarlos.
        self.stream.set(cv2.CAP_PROP_BUFFERSIZE, 1)

        # Leemos el primer frame para asegurar conexión
        (self.grabbed, self.frame) = self.stream.read()
        self.stopped = False

    def start(self):
        """Inicia el hilo de captura en segundo plano."""
        t = threading.Thread(target=self.update, args=())
        t.daemon = True # Se cierra si el programa principal muere
        t.start()
        return self

    def update(self):
        """Bucle interno del hilo (No llamar manualmente)."""
        while not self.stopped:
            if not self.grabbed:
                self.stop()
            else:
                # Solo nos interesa el último frame (LATEST)
                (self.grabbed, self.frame) = self.stream.read()

    def read(self):
        """Devuelve el frame más reciente disponible."""
        return self.frame

    def stop(self):
        """Libera la cámara y detiene el hilo."""
        self.stopped = True
        self.stream.release()