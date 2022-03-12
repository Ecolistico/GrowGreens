#!/usr/bin/env python3

# Import Directories
import io
import time
import zlib
import socket
import struct
import picamera

class streamClient:
    def __init__(self, logger = None):
        self.log = logger
        self.sock = socket.socket()

        self.camera = picamera.PiCamera(stereo_mode='side-by-side', stereo_decimate=0)
        self.xres = 1280
        self.yres = 720 
        
        self.stream = io.BytesIO()
        # Make a file-like object out of the connection
        self.connection = self.sock.makefile('wb')
    
    def str2log(self, mssg, level = 1):
        if self.log is not None:
            if(level==0 or level=="DEBUG"): self.log.debug(mssg)
            elif(level==1 or level=="INFO"): self.log.info(mssg)
            elif(level==2 or level=="WARNING"): self.log.warning(mssg)
            elif(level==3 or level=="ERROR"): self.log.error(mssg)
            elif(level==4 or level=="CRITICAL"): self.log.critical(mssg)
        else: print(mssg)
        
    def clientConnect(self, host, port):
        # Connect a client socket to host:port
        self.sock.connect((host, port))
        mssg = 'Connected to host: ' + str(host) + ', at port: ' + str(port)
        self.str2log(mssg, 1)

    def cameraSetup(self):
        self.camera.resolution = (self.xres*2, self.yres)
        self.camera.framerate = 20
        self.camera.hflip = False
        # Add camera settings
        # Set ISO to the desired value
        self.camera.iso = 150
        # Now fix the values
        self.camera.shutter_speed = self.camera.exposure_speed
        self.camera.exposure_mode = 'off'
        g = self.camera.awb_gains
        self.camera.awb_mode = 'off'
        self.camera.awb_gains = g
        # Let the camera warm up for 2 seconds
        time.sleep(2)
        self.str2log('Camera ready!', 1)
        
    def captureStreaming(self):
        # Capture the image
        self.camera.capture(self.stream, 'png')
        # Compress the image to speed up the process
        data = zlib.compress(self.stream.read())
        self.str2log("Byte of images = {}".format(len(data)), 1) # Debug differents sizes of data using png, raw, yuv or any other format
        # Write the length of the capture to the stream and flush to ensure it actually gets sent
        self.connection.write(struct.pack('<L', len(data)))
        self.connection.flush()
        
        # Rewind the stream and send the image data over the wire
        self.stream.seek(0)
        self.connection.write(data)
        
        # Reset the stream for the next capture
        self.stream.seek(0)
        self.stream.truncate()
        
    def endStreaming(self):
        # Write a length of zero to the stream to signal we're done
        self.connection.write(struct.pack('<L', 0))
        self.connection.flush()
        self.sock.close()

def main():    
    tucan = streamClient()
    
    tucan.clientConnect("192.168.6.87", 8001) # Search correct IP and Port to succed in connection
    tucan.cameraSetup()
  
    try:
        while True:
            tucan.captureStreaming()
        
    finally:
        tucan.endStreaming()
        tucan.connection.close()
        tucan.sock.close()
            
if __name__ == '__main__':
    main()