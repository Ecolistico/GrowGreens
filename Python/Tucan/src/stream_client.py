#!/usr/bin/env python3

# Import Directories
import io
import socket
import struct
import time
import picamera

class CamStreamer:
    def __init__(self):
        self.host = None
        self.port = None
        self.sock = None
        self.connection = None
        self.stream = None
        self.camera = None

    def start(self, host, port = 8000):
        try:
            self.host = host
            self.port = port
            self.sock = socket.socket()
            # Connect a client socket to my_server:8000 (change my_server to the hostname of your server)
            self.sock.connect((self.host, self.port))
            # Make a file-like object out of the connection
            self.connection = self.sock.makefile('wb')
            self.stream = io.BytesIO()

            self.camera = picamera.PiCamera(stereo_mode='side-by-side', stereo_decimate=0)
            self.camera.resolution = (1280*2, 720)
            self.camera.framerate = 20
            self.camera.hflip = False
            
            # Start a preview
            #camera.start_preview(fullscreen=False, window=(100, 200, 300, 400))

            # Let the camera warm up for 2 seconds
            time.sleep(2)

            # Note the start time and construct a stream to hold image data temporarily 
            # (we could write it directly to connection but in this case we want to find out the size of each capture first to keep our protocol simple)
            start = time.time()
            stream = io.BytesIO()
        except Exception as e:
            print("Exception Raised: {}".format(e))
        finally:
            self.end()

    def stream_video(self):
        if self.sock != None and self.connection != None and self.camera!=None:
            self.camera.capture(self.stream, 'png')
            # Write the length of the capture to the stream and flush to
            # ensure it actually gets sent
            self.connection.write(struct.pack('<L', self.stream.tell()))
            self.connection.flush()
            # Rewind the stream and send the image data over the wire
            self.stream.seek(0)
            self.connection.write(stream.read())
            # Reset the stream for the next capture
            self.stream.seek(0)
            self.stream.truncate()

    def end(self):
        # Write a length of zero to the stream to signal we're done
        if self.connection != None:
            self.connection.write(struct.pack('<L', 0))
            # Close the connection, socket and camera
            self.connection.close()
            self.sock.close()
            self.camera.close()
        # Put as none all variables
        self.host = None
        self.port = None
        self.sock = None
        self.connection = None
        self.stream = None
        self.camera = None

# Debug
def main():
    myStreamer = CamStreamer() 
    # Select the IP from the host
    myStreamer.start("192.168.6.87")
    try:
        while True:
            myStreamer.stream_video()
    except Exception as e:
        print("Exception Raised")
        raise e
    finally:
        myStreamer.end()
    
if __name__ == '__main__':
    main()