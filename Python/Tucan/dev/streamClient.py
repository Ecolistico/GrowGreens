#!/usr/bin/env python3

# Import Directories
import io
import socket
import struct
import time
import picamera



class streamClient:
    def __init__(self):
        
        self.host = "192.168.6.87"
        self.port = 8000     
        self.sock = socket.socket()
        
                
        self.camera = picamera.PiCamera(stereo_mode='side-by-side',
                                        stereo_decimate=0)
        self.xres = 1280
        self.yres = 720 
        
        self.stream = io.BytesIO()
        # Make a file-like object out of the connection
        self.connection = self.sock.makefile('wb')
        
        self.start = time.time()

##############################################################
        
    def clientConnect(self):
        
        # Connect a client socket to host:port
        self.sock.connect((self.host, self.port))
        
        print('Connected to host: ' + str(self.host) +
              ', at port: ' + str(self.port))
        

    
    def cameraSetup(self):

        self.camera.resolution = (self.xres*2, self.yres)
        self.camera.framerate = 20
        self.camera.hflip = False
        
        # Let the camera warm up for 2 seconds
        time.sleep(2)
        print('Camera ready!')
        
        
    def streaming(self):
        # Note the start time and construct a stream to hold image data
        # temporarily (we could write it directly to connection but in this
        # case we want to find out the size of each capture first to keep
        # sour protocol simple)
        for foo in self.camera.capture_continuous(self.stream, 'png'):
            
            # Write the length of the capture to the stream and flush to
            # ensure it actually gets sent
            self.connection.write(struct.pack('<L', self.stream.tell()))
            self.connection.flush()
            
            # Rewind the stream and send the image data over the wire
            self.stream.seek(0)
            self.connection.write(self.stream.read())
            
            # If we've been capturing for more than 30 seconds, quit
            if time.time() - self.start > 30:
                break
            
            # Reset the stream for the next capture
            self.stream.seek(0)
            self.stream.truncate()
            
        # Write a length of zero to the stream to signal we're done
        self.connection.write(struct.pack('<L', 0))
        print(struct.pack('<L', 0))
        
        
######################################################       
def main():
    
    tucan = streamClient()
    
    tucan.clientConnect()
    tucan.cameraSetup()
    
    try:
        tucan.streaming()
        
    finally:
        tucan.connection.close()
        tucan.sock.close()
            
if __name__ == '__main__':
    main()