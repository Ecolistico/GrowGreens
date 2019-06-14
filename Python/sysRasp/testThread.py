import threading
import time
import sysRasp

WiFiState = 0
WiFiTime = time.time()
WiFiCount = 0

TocaniTime = time.time()

def check_WiFi():
    global WiFiState
    global WiFiTime
    global WiFiCount
    while True:
        if(time.time()- WiFiTime > 20):
            if(sysRasp.isWiFi()):
                WiFiState = 1
                WiFiCount = 0
            elif(sysRasp.isAP()):
                WiFiState = 2
                WiFiCount = 0
            else:
                WiFiState = 0
                WiFiCount += 1
            WiFiTime = time.time()
            
        if(WiFiCount>=5):
            WiFiCount = 0
            print("Configuring AP...")
            sysRasp.runShellCommand('sudo python APconfig.py')

def tocani():
    global TocaniTime
    while True:
        if(time.time()- TocaniTime > 10):
            TocaniTime = time.time()
            print("Tocani has to move")
        
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
        
        
