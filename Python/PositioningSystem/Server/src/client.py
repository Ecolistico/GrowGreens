#!/usr/bin/env python3

class Client:
    def __init__(self, name = "", x = 0, y = 0, z = 0):
        self.name = name
        self.x = x
        self.y = y
        self.z = z

class ClientManager:
    def __init__(self, clientData):
        self.clients = []
        for client in clientData: 
            self.clients.append(Client(clientData[client], 
                                float(clientData[client]["x"]), 
                                float(clientData[client]["y"]), 
                                float(clientData[client]["z"])))

    def returnPositions(self):
        positions = []
        for client in self.clients: positions.append([client.x, client.y, client.z])
        return positions