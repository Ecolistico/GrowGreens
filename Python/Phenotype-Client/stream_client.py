import io
import socket
import struct
import time
import picamera

# Connect a client socket to my_server:8000 (change my_server to the
# hostname of your server)
client_socket = socket.socket()
client_socket.connect(('192.168.6.87', 8000))

# Make a file-like object out of the connection
connection = client_socket.makefile('wb')
try:
    #camera = picamera.PiCamera()
    camera = picamera.PiCamera(stereo_mode='side-by-side', stereo_decimate=0)
    camera.resolution = (1280*2, 720)
    camera.framerate = 20
    camera.hflip = False
    
    # Start a preview and let the camera warm up for 2 seconds
    #camera.start_preview(fullscreen=False, window=(100, 200, 300, 400))
    time.sleep(2)

    # Note the start time and construct a stream to hold image data
    # temporarily (we could write it directly to connection but in this
    # case we want to find out the size of each capture first to keep
    # our protocol simple)
    start = time.time()
    stream = io.BytesIO()
    
    for foo in camera.capture_continuous(stream, 'png'):
        # Write the length of the capture to the stream and flush to
        # ensure it actually gets sent
        connection.write(struct.pack('<L', stream.tell()))
        connection.flush()
        # Rewind the stream and send the image data over the wire
        stream.seek(0)
        connection.write(stream.read())
        # If we've been capturing for more than x seconds, quit
        if time.time() - start > 30:
            break
        # Reset the stream for the next capture
        stream.seek(0)
        stream.truncate()
    # Write a length of zero to the stream to signal we're done
    connection.write(struct.pack('<L', 0))
    
finally:
    connection.close()
    client_socket.close()