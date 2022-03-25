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

        self.camera1 = picamera.PiCamera(camera_num = 0, sensor_mode = 2)
        self.camera2 = picamera.PiCamera(camera_num = 1, sensor_mode = 2) 
        
        self.stream1 = io.BytesIO()
        self.stream2 = io.BytesIO()
    
    def str2log(self, mssg, level = 1):
        if self.log is not None:
            if(level==0 or level=="DEBUG"): self.log.debug(mssg)
            elif(level==1 or level=="INFO"): self.log.info(mssg)
            elif(level==2 or level=="WARNING"): self.log.warning(mssg)
            elif(level==3 or level=="ERROR"): self.log.error(mssg)
            elif(level==4 or level=="CRITICAL"): self.log.critical(mssg)
        else: print(mssg)
        
    def clientConnect(self, host, port):
        self.sock = socket.socket()
        # Make a file-like object out of the connection
        self.connection = self.sock.makefile('wb')
        # Connect a client socket to host:port
        self.sock.connect((host, port))
        mssg = 'Connected to host: ' + str(host) + ', at port: ' + str(port)
        self.str2log(mssg, 1)

    def cameraSetup(self):
        # Set ISO to the desired value
        #self.camera1.iso = 250
        #self.camera2.iso = 250
        # Wait for the automatic gain control to settle
        time.sleep(2)
        # Now fix the values
        #self.camera1.shutter_speed = self.camera1.exposure_speed
        #self.camera1.exposure_mode = 'off'
        #g = self.camera1.awb_gains
        #self.camera1.awb_mode = 'off'
        #self.camera1.awb_gains = g
        
        #self.camera2.shutter_speed = self.camera2.exposure_speed
        #self.camera2.exposure_mode = 'off'
        #g = self.camera2.awb_gains
        #self.camera2.awb_mode = 'off'
        #self.camera2.awb_gains = g
        self.str2log('Cameras ready!', 1)
        
    def captureStreaming(self):
        # Capture the image
        #timer = time.time()
        self.camera1.capture(self.stream1, 'jpeg', bayer = True)
        self.camera2.capture(self.stream2, 'jpeg', bayer = True)
        #print("Time taken to capture: {}".format(time.time() - timer))
        #timer = time.time()
        # Write the length of the capture to the stream and flush to ensure it actually gets sent
        self.connection.write(struct.pack('<L', self.stream1.tell() + self.stream2.tell()))
        self.connection.flush()
        
        # Rewind the stream and send the image data over the wire
        self.stream1.seek(0)
        self.stream2.seek(0)
        self.connection.write(self.stream1.read())
        self.connection.write(self.stream2.read())
        
        # Reset the stream for the next capture
        self.stream1.seek(0)
        self.stream2.seek(0)
        self.stream1.truncate()
        self.stream2.truncate()
        #print("Time taken to send: {}".format(time.time() - timer))

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