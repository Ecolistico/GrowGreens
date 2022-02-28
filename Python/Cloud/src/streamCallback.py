import io
import cv2
import socket
import struct
import select
import numpy as np

class streamController:
    def __init__(self, logger = None):
        self.log = logger
        self.captures = 0

        # Socket variables
        self.host = '0.0.0.0'
        self.port = 9999
        self.sock = socket.socket()
        self.sock.setblocking(False)
        
        self.path = '/home/pi/Documents/GrowGreens/Python/Cloud/captures/'
        self.captures = 0
        self.connection = None

        # Start a socket listening for connections on 0.0.0.0:9999
        # (0.0.0.0 means all interfaces)
        self.sock.bind((self.host, self.port))
        self.sock.listen(0)
        self.str2log('Listening on port {}'.format(self.port))
        self.read_list = [self.sock]

    def str2log(self, msg, level = 'DEBUG'):
        if self.log!=None:
            if (level==0 or level=='DEBUG'): self.log.debug(msg)
            elif (level==1 or level=='INFO'): self.log.info(msg)
            elif (level==2 or level=='WARNING'): self.log.warning(msg)
            elif (level==3 or level=='ERROR'): self.log.error(msg)
            elif (level==4 or level=='CRITICAL'): self.log.critical(msg)
        else:
            print(msg)

    def end(self):
        if self.connection is not None:
            self.captures = 0
            self.connection.close()
            self.socket.close()
            self.str2log("streamController - Ending socket", 2)

    def streaming(self):
        readable, writable, errored = select.select(self.read_list, [], [], 0)
        for s in readable:
            if s is self.sock:
                client_socket, address = self.sock.accept()
                self.read_list.append(client_socket)
                # Accept a single connection and make a file-like object out of it
                self.connection = client_socket.makefile('rb')
                self.str2log("Connection from", address)
            else:
                if self.connection!= None:
                    # Read the length of the image as a 32-bit unsigned int.
                    self.image_len = struct.unpack('<L', self.connection.read(struct.calcsize('<L')))[0]
                    
                    # If the length is zero, quit the loop
                    if not self.image_len: 
                        self.captures = 0
                        self.connection.close()
                        self.read_list.remove(s)
                        self.str2log("streamController - Closing Connection", 1)
                    else:
                        # Construct a stream to hold the image data and read the 
                        # image data from the connection
                        self.image_stream = io.BytesIO()
                        myImage = self.connection.read(self.image_len)
                        
                        self.image_stream.write(myImage)
                        # Rewind the stream, save it as an image with opencv
                        self.image_stream.seek(0)
                        
                        self.img_bytes = self.image_stream.getvalue()
                        self.img_arr = np.frombuffer(self.img_bytes, np.uint8)
                        self.img = cv2.imdecode(self.img_arr, cv2.IMREAD_COLOR)
                    
                        self.name = 'test' + str(self.captures) + '.png'
                        cv2.imwrite(self.path + self.name, self.img)
                        self.str2log('Capture: ' + self.name + ' saved.')
                        self.captures = int(self.captures) + 1
                """
                data = s.recv(1024)
                if data:
                    s.send(data)
                else:
                    s.close()
                    read_list.remove(s)
                """

# Debug
def main():
    myStream = streamController()
    try:
        while True: 
            myStream.streaming()
    except Exception as e:
        myStream.str2log(e, 3)
    finally:
        myStream.end()

if __name__ == '__main__':
    main()