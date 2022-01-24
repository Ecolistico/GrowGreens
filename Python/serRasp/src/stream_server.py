import io
import socket
import struct
import time
import numpy as np
from PIL import Image

# Start a socket listening for connections on 0.0.0.0:8000 (0.0.0.0 means
# all interfaces)
server_socket = socket.socket()
server_socket.bind(('0.0.0.0', 8000))
server_socket.listen(0)

captures = 0

# Accept a single connection and make a file-like object out of it
connection = server_socket.accept()[0].makefile('rb')

try:
    #while True:
        
    while captures <= 5:
        # Read the length of the image as a 32-bit unsigned int. If the
        # length is zero, quit the loop
        image_len = struct.unpack('<L', connection.read(struct.calcsize('<L')))[0]
        if not image_len:
            break
        # Construct a stream to hold the image data and read the image
        # data from the connection
        image_stream = io.BytesIO()
        image_stream.write(connection.read(image_len))
        # Rewind the stream, open it as an image with PIL and do some
        # processing on it
        image_stream.seek(0)
        image = Image.open(image_stream)
        print('Image is %dx%d' % image.size)
        image.verify()
        print('Image #' + str(captures) + ' is verified')
        
        img_bytes = image_stream.getvalue()
        img_arr = np.frombuffer(img_bytes, np.uint8)
        path = '/home/pi/Documents/GrowGreens/Python/serRasp/data'
        np.save(path + 'test' + str(captures), img_arr)
    
        captures  = captures + 1

finally:
    connection.close()
    server_socket.close()