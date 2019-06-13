import threading
import time
import sysRasp

WiFiTime = time.time()
TocaniTime = time.time()
number = 1

def calc_square(number):
    print('Square: ', number * number)
    
def calc_quad(number):
    print('Quad: ', number * number * number * number)

def check_WiFi():
    global WiFiTime
    if(time.time()- WiFiTime > 60):
        if(sysRasp.isWiFi()): print("You are connected to WLAN")
        elif(isAP()): print("You has an AP")
        else: print("Need to setup an AP")
        WiFiTime = time.time()

def tocani():
    global TocaniTime
    global number
     
    if(time.time()- TocaniTime > 10):
        calc_square(number)
        calc_quad(number)
        TocaniTime = time.time()
        number += 1
    
if __name__ == "__main__":
    while True:
        thread1 = threading.Thread(target=check_WiFi, args=())
        thread2 = threading.Thread(target=tocani, args=())
        
        thread1.start()
        thread2.start()
        
        thread1.join()
        thread2.join()
        
