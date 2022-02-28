#!/usr/bin/env python3

# Import directories
import io
import socket
import struct
import select

import cv2
import numpy as np

class streamServer:
    def __init__(self):
        self.host = '0.0.0.0'
        self.port = 8001
        self.sock = socket.socket()
        self.sock.setblocking(False)
        
        self.path = '/home/pi/Documents/'
        self.captures = 0
        self.connection = None
        self.inCapture = False
        
##############################################
    def serverListen(self):
        # Start a socket listening for connections on 0.0.0.0:8000
        # (0.0.0.0 means all interfaces)
        
        self.sock.bind((self.host, self.port))
        self.sock.listen(0)
        print('Listening on port {}'.format(self.port))
        
        self.read_list = [self.sock]
        
    def streaming(self):
        readable, writable, errored = select.select(self.read_list, [], [], 0)
        for s in readable:
            if s is self.sock:
                client_socket, address = self.sock.accept()
                self.read_list.append(client_socket)
                # Accept a single connection and make a file-like object out of it
                self.connection = client_socket.makefile('rb')
                print ("Connection from", address)
            else:
                if self.connection!= None:
                    
                    # Read the length of the image as a 32-bit unsigned int.
                    self.image_len = struct.unpack('<L',
                                                   self.connection.read(struct.calcsize('<L')))[0]
                    
                    # If the length is zero, quit the loop
                    if not self.image_len: pass
                    
                    # Construct a stream to hold the image data and read the 
                    # image data from the connection
                    self.image_stream = io.BytesIO()
                    myImage = self.connection.read(self.image_len)
                    print(self.image_len, len(myImage))
                    self.image_stream.write(myImage)
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
                    self.inCapture = False
                """
                data = s.recv(1024)
                if data:
                    s.send(data)
                else:
                    s.close()
                    read_list.remove(s)
                """
def main():
    cloud = streamServer()
    cloud.serverListen()
    try:
        while True:
            cloud.streaming()

    finally:       
        cloud.connection.close()
        cloud.sock.close()

if __name__ == '__main__':
    main()