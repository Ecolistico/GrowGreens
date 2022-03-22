#!/usr/bin/env python3
from triangulation import triangulation

class Client:
    def __init__(self, x = 0, y = 0, z = 0):
        self.x = x
        self.y = y
        self.z = z
    
    def returnPosition(self): return [self.x, self.y, self.z]

class bluetoothDevice:
    def __init__(self, data = {"mac": "00:00:00:00:00:00"}, clientData = {}):
        self.data = data
        self.mac = data["mac"]
        self.distance = {}
        for client in clientData: self.distance[client] = 0
    
    def updateDistance(self, client, dist):
        self.distance[client] = dist

    def getDistances(self, clientData):
        distances = []
        for client in clientData: distances.append(self.distance[client])
        return distances

class devicesManager:
    def __init__(self, bluetoothDevicesData, clientData):
        # Initialize devices and positions
        self.devices = {}
        self.positions = {}
        for device in bluetoothDevicesData: 
            self.devices[device] = bluetoothDevice(bluetoothDevicesData[device], clientData)
            self.positions[device] = [0, 0, 0]

        # Define Clients
        self.clients = {}
        for client in clientData: 
            self.clients[client] = Client(float(clientData[client]["x"]), 
                                          float(clientData[client]["y"]),  
                                          float(clientData[client]["z"]))

        # Save the information from the clients for later use
        self.clientData = clientData

    def returnClientPositions(self, clientData):
        positions = []
        for client in clientData: positions.append(self.clients[client].returnPosition())
        return positions

    def getBeaconsPositions(self):
        pos = self.returnClientPositions(self.clientData)
        for device in self.devices:
            dist = self.devices[device].getDistances(self.clientData)
            self.positions[device] = triangulation(pos, dist)
            print(self.positions[device])
        print()

    