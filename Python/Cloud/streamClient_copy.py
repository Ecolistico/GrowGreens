#!/usr/bin/env python3

# Import Directories
import io
import socket
import struct
import time
import picamera

class streamClient:
    def __init__(self):
        self.sock = socket.socket()

        self.camera = picamera.PiCamera(stereo_mode='side-by-side', stereo_decimate=0)
        self.xres = 1280
        self.yres = 720 
        
        self.stream = io.BytesIO()
        # Make a file-like object out of the connection
        self.connection = self.sock.makefile('wb')
        
    def clientConnect(self, host, port):
        # Connect a client socket to host:port
        self.sock.connect((host, port))
        print('Connected to host: ' + str(self.host) + ', at port: ' + str(self.port))
        
    def cameraSetup(self):
        self.camera.resolution = (self.xres*2, self.yres)
        self.camera.framerate = 20
        self.camera.hflip = False        
        # Let the camera warm up for 2 seconds
        time.sleep(2)
        print('Camera ready!')
        
    def captureStreaming(self):
        self.camera.capture(self.stream, 'png')
        # Write the length of the capture to the stream and flush to ensure it actually gets sent
        self.connection.write(struct.pack('<L', self.stream.tell()))
        self.connection.flush()
        
        # Rewind the stream and send the image data over the wire
        self.stream.seek(0)
        self.connection.write(self.stream.read())
        
        # Reset the stream for the next capture
        self.stream.seek(0)
        self.stream.truncate()
        
    def endStreaming():
        # Write a length of zero to the stream to signal we're done
        self.connection.write(struct.pack('<L', 0))

def main():    
    tucan = streamClient()
    
    tucan.clientConnect("192.168.6.87", "8001") # Search correct IP and Port to succed in connection
    tucan.cameraSetup()
  
    try:
        while True:
            tucan.streaming()
        
    finally:
        tucan.endStreaming()
        tucan.connection.close()
        tucan.sock.close()
            
if __name__ == '__main__':
    main()