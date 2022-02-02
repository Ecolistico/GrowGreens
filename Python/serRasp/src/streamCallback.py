import io
import time
import socket
import struct
import numpy as np
from PIL import Image


class streamController:
    def __init__(self, logger):
        self.log = logger
        self.captures = 0
        self.isStreaming = False

    def openSocket(self):
        # Start a socket listening for connections on 0.0.0.0:8000 (0.0.0.0 means all interfaces)
        self.server_socket = socket.socket()
        self.server_socket.bind(('0.0.0.0', 8000))
        self.server_socket.listen(0)
        
        # Accept a single connection and make a file-like object out of it
        self.connection = server_socket.accept()[0].makefile('rb')
        self.isStreaming = True

    def closeSocket(self):
        self.captures = 0
        self.isStreaming = False
        self.connection.close()
        self.server_socket.close()

    def streaming(self):
        if self.isStreaming:
            try:
                # Read the length of the image as a 32-bit unsigned int. If the length is zero, quit the loop
                image_len = struct.unpack('<L', self.connection.read(struct.calcsize('<L')))[0]
                if not image_len: break

                # Construct a stream to hold the image data and read the image data from the connection
                image_stream = io.BytesIO()
                image_stream.write(self.connection.read(image_len))
                
                # Rewind the stream, open it as an image with PIL and do some processing on it
                image_stream.seek(0)
                image = Image.open(image_stream)
                print('Image is %dx%d' % image.size)
                image.verify()
                print('Image #' + str(captures) + ' is verified')
                
                img_bytes = image_stream.getvalue()
                img_arr = np.frombuffer(img_bytes, np.uint8)
                path = '/home/pi/Documents/GrowGreens/Python/serRasp/captures'
                np.save(path + 'test' + str(captures), img_arr)
                captures  = captures + 1

            except Exception as e:
                print(e)
                self.closeSocket()