"""
A simple Python script to send messages to a server over Bluetooth
using PyBluez (with Python 3).
"""

#!/usr/bin/env python3

import bluetooth
import blue_search as search
import json

with open("client_config.json") as f:
    data = json.load(f)
    serverMACAddress = data["ambimodulo6/1"]
    
#rssi = search.rssi()
#print('Device: ' + str(rssi[0]) + ', RSSI: ' + str(rssi[1]))

port = 3
size = 1024

s = bluetooth.BluetoothSocket(bluetooth.RFCOMM)
s.connect((serverMACAddress, port))

while 1:
    text = input('mensaje: ')
    if text == "adios":
        break
    s.send(text)
    
s.close()