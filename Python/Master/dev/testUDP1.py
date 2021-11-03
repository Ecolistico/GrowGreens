import socket

msgFromClient = "Hello UDP Server"
bytesToSend = b'\x80\x80\x80\x80\xA1\x00\x01\x10\x00'

serverAddressPort = ("192.168.6.105", 8888)
bufferSize = 1024

# Create a UDP socket at client side
UDPClientSocket = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)
 
# Send to server using created UDP socket
UDPClientSocket.sendto(bytesToSend, serverAddressPort)
msgFromServer = UDPClientSocket.recvfrom(bufferSize)
msg = "Message from Server {}".format(msgFromServer[0])
print(msg)