import bluetooth

serverMACAddress = '26:31:AE:E9:7C:69'
port = 3

s = bluetooth.BluetoothSocket(bluetooth.RFCOMM)
s.connect((serverMACAddress, port))
"""
while 1:
    text = input("input:") 
    if text == "quit":
        break
    s.send(text)
s.close()
"""
text = "MF#1"
s.send(text)

s.close()