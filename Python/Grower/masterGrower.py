import threading
import time
from Grower import Grower
from time import time
from pysftp import Connection
import sys
sys.path.insert(0, '../sysRasp/')
import sysRasp

containerID, floor, brokerIP = sysRasp.getData_JSON(sysRasp.MQTT_PATH)

def check_WiFi():
    global containerID
    global floor
    global brokerIP
    
    WiFiState = 0
    WiFiTime = time.time()
    WiFiCount = 0
    prevWiFiState = 0
    
    while True:
        if(time.time()- WiFiTime > 20):
            if(sysRasp.isWiFi()):
                if(prevWiFiState == 2):
                     ID, fl, IP = sysRasp.getData_JSON(sysRasp.PATH)
                     if(ID!= containerID and ID!=""):
                         containerID = ID
                     if(fl!= floor and fl!=""):
                         floor = fl
                     if(IP!=brokerIP and IP!=""):
                         brokerIP = IP
                prevWiFiState = WiFiState
                WiFiState = 1 # Conected to WiFi
                WiFiCount = 0
            elif(sysRasp.isAP()):
                prevWiFiState = WiFiState
                WiFiState = 2 # Access Point created
                WiFiCount = 0
            else:
                prevWiFiState = WiFiState
                WiFiState = 0 # Not accesPoint or WiFi
                WiFiCount += 1
            WiFiTime = time.time()
        
        if(WiFiCount>=5):
            WiFiCount = 0
            print("Configuring AP...")
            sysRasp.runShellCommand('sudo python APconfig.py')

def tocani():
    TocaniTime = time.time()

    while True:
        if(time.time()- TocaniTime > 10):
            TocaniTime = time.time()
            print(containerID)
            print(floor)
            print(brokerIP)
        
if __name__ == "__main__":
    while True:
        thread1 = threading.Thread(target=check_WiFi, args=())
        thread2 = threading.Thread(target=tocani, args=())

        thread1.start()
        thread2.start()
        
        thread1.join()
        thread2.join()
        
        while True:
            pass
        
        
