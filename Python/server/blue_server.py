import bluetooth

# The MAC address of a Bluetooth adapter on the server.
# The server might have multiple Bluetooth adapters.
hostMACAddress = '5B:8E:B5:41:A1:A0' 
port = 3
backlog = 1
size = 1024

s = bluetooth.BluetoothSocket(bluetooth.RFCOMM)
s.bind((hostMACAddress, port))
s.listen(backlog)

try:
    client, clientInfo = s.accept()
    while 1:
        data = client.recv(size)
        if data:
            print(data.decode("utf-8"))
            #client.send(data) # Echo back to client
except:	
    print("Closing socket")
    client.close()
    s.close()