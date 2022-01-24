import socket
import random

a = hex(random.randrange(0,255))
b = hex(random.randrange(0,255))
c = hex(random.randrange(0,255))
d = hex(random.randrange(0,255))
a = a[2:]
b = b[2:]
c = c[2:]
d = d[2:]
if len(a)<2:
    a = "0" + a
if len(b)<2:
    b = "0" + b
if len(c)<2:
    c = "0" + c
if len(d)<2:
    d = "0" + d



e = 0xA1
f = 0x00
g = 0x01
h = 0x10
i = 0x00
a1 = int(hex(a))
b1 = int(hex(b))
c1 = int(hex(c))
d1 = int(hex(d))
print(a1)
print(b1)
print(c1)
print(d1)
print(e)
z = [a1,b1,c1,d1,e,f,g,h,i]
#s=bytes(z)
#type(s)



msgFromClient = "Hello UDP Server"
bytesToSend = bytes(z)

serverAddressPort = ("192.168.6.82", 8888)
bufferSize = 1024

# Create a UDP socket at client side
UDPClientSocket = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)
 
# Send to server using created UDP socket
UDPClientSocket.sendto(bytesToSend, serverAddressPort) 
msgFromServer = UDPClientSocket.recvfrom(bufferSize)
msg = "Message from Server {}".format(msgFromServer[0])
print(msg)