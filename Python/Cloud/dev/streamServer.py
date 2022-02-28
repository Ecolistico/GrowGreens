#!/usr/bin/env python3

# Import directories
import io
import socket
import struct

import cv2
import numpy as np

class streamServer:
    
    def __init__(self):
        
        self.host = '0.0.0.0'
        self.port = 8000
        self.sock = socket.socket()
        
        self.path = '/home/pi/Documents/GrowGreens/Python/Tucan/captures/'
        
        self.captures = 0

##############################################
    def serverListen(self):
        
        # Start a socket listening for connections on 0.0.0.0:8000
        # (0.0.0.0 means all interfaces)
        
        self.sock.bind((self.host, self.port))
        self.sock.listen(0)
        print('Listening...')
        
        # Accept a single connection and make a file-like object out of it
        self.connection = self.sock.accept()[0].makefile('rb')
        
    def streaming(self):
        while True:

            # Read the length of the image as a 32-bit unsigned int.
            self.image_len = struct.unpack('<L',
                                           self.connection.read(struct.calcsize('<L')))[0]
            
            # If the length is zero, quit the loop
            if not self.image_len:
                break
            
            # Construct a stream to hold the image data and read the 
            # image data from the connection
            self.image_stream = io.BytesIO()
            self.image_stream.write(self.connection.read(self.image_len))
            
            # Rewind the stream, save it as an image with opencv
            self.image_stream.seek(0)
            
            
            self.img_bytes = self.image_stream.getvalue()
            self.img_arr = np.frombuffer(self.img_bytes, np.uint8)
            self.img = cv2.imdecode(self.img_arr, cv2.IMREAD_COLOR)
        
            self.name = 'test' + str(self.captures) + '.png'
            cv2.imwrite(self.path + self.name, self.img)
            #np.save(path + 'test' + str(captures), img_arr)
            print('Capture: ' + self.name + ' saved.')
            self.captures = int(self.captures) + 1


def main():
    
    cloud = streamServer()
    cloud.serverListen()
    
    try:
        cloud.streaming()

    finally:       
        cloud.connection.close()
        cloud.sock.close()


if __name__ == '__main__':
    main()