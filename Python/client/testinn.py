import bluetooth

serverMACAddress = 'DC:A6:32:9F:F2:2E'
port = 3

s = bluetooth.BluetoothSocket(bluetooth.RFCOMM)
#s.connect((serverMACAddress, port))

s.listen(1024)