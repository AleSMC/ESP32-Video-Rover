"""
VideoStream.py
--------------
Module responsible for asynchronous video reception.
Uses threading to prevent blocking the main loop caused by 
network latency or OpenCV decoding overhead.
"""

import cv2
import threading

class VideoStream:
    def __init__(self, src=0):
        """
        Initializes the video stream.
        :param src: Video URL (e.g., http://192.168.4.1/stream) or local camera ID (0).
        """
        self.stream = cv2.VideoCapture(src)

        # Limit internal buffer to 1 frame.
        # This forces OpenCV to discard old frames if processing is slow,
        # ensuring we always work with the "real-time" image.
        self.stream.set(cv2.CAP_PROP_BUFFERSIZE, 1)

        # Read the first frame to ensure connection is established
        (self.grabbed, self.frame) = self.stream.read()
        self.stopped = False

    def start(self):
        """Starts the capture thread in the background."""
        t = threading.Thread(target=self.update, args=())
        t.daemon = True # Kills thread automatically if main program exits
        t.start()
        return self

    def update(self):
        """Internal thread loop (Do not call manually)."""
        while not self.stopped:
            if not self.grabbed:
                self.stop()
            else:
                # We are only interested in the LATEST frame available
                (self.grabbed, self.frame) = self.stream.read()

    def read(self):
        """Returns the most recent available frame."""
        return self.frame

    def stop(self):
        """Releases the camera resource and stops the thread."""
        self.stopped = True
        self.stream.release()