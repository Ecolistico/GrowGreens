#!/usr/bin/env python3

class bluetoothDevice:
    def __init__(self, name = "", data = {"mac": "00:00:00:00:00:00"}, clientsNumber = 0):
        self.name = name
        self.data = data
        self.mac = data["mac"]
        self.distance = []
        for i in range(clientsNumber): self.distance.append(0)

    def returnDistance(self):
        return self.distance
    
    def updateDistance(self, client, dist):
        self.distance[client] = dist

class devicesManager:
    def __init__(self, bluetoothDevicesData, clientsNumber = 0):
        self.devices = []
        for device in bluetoothDevicesData: 
            self.devices.append(bluetoothDevice(bluetoothDevicesData[device], device, clientsNumber))