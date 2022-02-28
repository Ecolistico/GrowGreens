#!/usr/bin/env python3

# Import Directories
import bluetooth

class BlueServer:
    def __init__(self, MAC, logger = None, port = 3, backlog = 1, size = 1024):
        self.hostMAC  = None
        self.port = port
        self.backlog = backlog
        self.size = size
        self.log = logger
        self.socket = bluetooth.BluetoothSocket(bluetooth.RFCOMM)
        self.socket.bind((hostMACAddress, port))
        self.socket.listen(backlog)
        self.client = None 
        self.clientInfo = None

    def str2log(self, msg, level = 'DEBUG'):
        if self.log!=None:
            if (level==0 or level=='DEBUG'): self.log.debug(msg)
            elif (level==1 or level=='INFO'): self.log.info(msg)
            elif (level==2 or level=='WARNING'): self.log.warning(msg)
            elif (level==3 or level=='ERROR'): self.log.error(msg)
            elif (level==4 or level=='CRITICAL'): self.log.critical(msg)
        else:
            print(msg)

    def available(self):
        self.client, self.clientInfo = self.socket.accept()
        running = True
        while running:
            data = self.client.recv(self.size)
            data = data.decode('utf-8')
            if data.startswith("END"): running = False
            elif data.startswith("HELLO"): self.client.send("GOOD BYE".encode('utf-8'))
            self.str2log(data.decode("utf-8"))
            #client.send(data) # Echo back to client

    def close(self):
        if self.client!= None:
            self.client.close()
            self.socket.close()
        self.str2log("BlueServer - Closing socket", 1)

# Debug
def main():
    # The MAC address of a Bluetooth adapter on the server.
    # The server might have multiple Bluetooth adapters.
    blue = BlueServer("5B:8E:B5:41:A1:A0")
    blue.connect("26:31:AE:E9:7C:69") 
    blue.available()
    blue.close()

if __name__ == '__main__':
    main()