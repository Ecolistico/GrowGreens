import io
import time
import socket
import struct
import numpy as np
from PIL import Image

class streamController:
    def __init__(self, logger = None):
        self.log = logger
        self.captures = 0
        self.savePicture = False
        self.isStreaming = False

    def str2log(self, msg, level = 'DEBUG'):
        if self.log!=None:
            if (level==0 or level=='DEBUG'): self.log.debug(msg)
            elif (level==1 or level=='INFO'): self.log.info(msg)
            elif (level==2 or level=='WARNING'): self.log.warning(msg)
            elif (level==3 or level=='ERROR'): self.log.error(msg)
            elif (level==4 or level=='CRITICAL'): self.log.critical(msg)
        else:
            print(msg)

    def openSocket(self):
        # Start a socket listening for connections on 0.0.0.0:8000 (0.0.0.0 means all interfaces)
        self.socket = socket.socket()
        self.socket.bind(('0.0.0.0', 8000))
        self.socket.listen(0)
        
        # Accept a single connection and make a file-like object out of it
        self.connection = self.socket.accept()[0].makefile('rb')
        self.isStreaming = True

    def closeSocket(self):
        if self.isStreaming:
            self.captures = 0
            self.isStreaming = False
            self.connection.close()
            self.socket.close()
            self.str2log("streamController - Closing socket", 1)

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
                self.str2log('Image is %dx%d' % image.size)
                image.verify()
                self.str2log('Image #' + str(captures) + ' is verified')
                
                img_bytes = image_stream.getvalue()
                img_arr = np.frombuffer(img_bytes, np.uint8)
                path = '/home/pi/Documents/GrowGreens/Python/Data-Server/captures'
                if self.savePicture:
                    np.save(path + 'test' + str(captures), img_arr)
                    self.savePicture = False
                captures  += 1

            except Exception as e:
                self.str2log(e, 4)
                self.closeSocket()

# Debug
def main():
    myStream = streamController()
    myStream.openSocket()
    while True: myStream.streaming()
        
if __name__ == '__main__':
    main()