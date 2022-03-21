#!/usr/bin/env python3

class Client:
    def __init__(self, x = 0, y = 0, z = 0):
        self.x = x
        self.y = y
        self.z = z

class ClientManager:
    def __init__(self, clientData):
        self.clients = []
        for client in clientData: self.clients.append(Client(float(client["x"]), float(client["y"]), float(client["z"])))
        print(returnPositions())

    def returnPositions(self):
        positions = []
        for client in self.clients: positions.append([client.x, client.y, client.z])
        return positions