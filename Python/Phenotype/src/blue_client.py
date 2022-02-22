#!/usr/bin/env python3

# Import Directories
import bluetooth

class BlueCLI:
    def __init__(self):
        self.serverMAC  = None
        self.port = None
        self.socket = None
    
    def connect(self, serverMAC, port = 3):
        try:
            self.serverMAC = serverMAC
            self.port = port
            self.socket = bluetooth.BluetoothSocket(bluetooth.RFCOMM)
            self.socket.connect((self.serverMAC, self.port))
        except Exception as e:
            print("Exception Raised: {}".format(e))
        finally:
            self.close()
            
    def send(self, data):
        if self.serverMAC != None and self.port != None: self.socket.send(data)

    def close(self):
        if self.socket != None:
            self.socket.close()
            self.serverMAC  = None
            self.port = None
            self.socket = None

# Debug
def main():
    from time import sleep

    blue = BlueCLI()
    blue.connect("26:31:AE:E9:7C:69") 
    
    try:
        while True:
                blue.send("HELLO WORLD")
                sleep(1)
    except Exception as e:
        print("Exception Raised")
        raise e
    finally:
        blue.end()
    
if __name__ == '__main__':
    main()