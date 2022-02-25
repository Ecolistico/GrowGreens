import sys
import os
import struct
from ctypes import (CDLL, get_errno)
from ctypes.util import find_library
from socket import (
    socket,
    AF_BLUETOOTH,
    SOCK_RAW,
    BTPROTO_HCI,
    SOL_HCI,
    HCI_FILTER,
)

class Kalman:
    def __init__(self, r, q):
        # r=1.4, q=0.065 values recomended for BLE applications
        self.r = r          # Noιse related to sensor
        self.q = q          # Noise related to process
        self.p = 0          # Estimated error
        self.k = 0          # Kalman gain
        self.value = 0.0    # Predicted value
        self.first = True   # First iteration flag

    def update(self, measurement):
        # Prediction
        self.p = self.p + self.q
        # Correction
        self.k = self.p / (self.p + self.r)
        if self.first: 
            self.value = measurement # First time prediction and value are the same
            self.first = False
        else: self.value = self.value + self.k * (measurement - self.value)
        self.p = (1 - self.k) * self.p
        return self.value

def rssi_to_dbm(rssi):
    return rssi - 256

# Log Distance Path Loss
def dbm_to_distance(dbm, rx_power = -75, n=2):
    # dbm is what we measure
    # rx_power is dbm measure at 1 meter
    # n is a constant for the exponential loss. It is calculated from experiments and they are tables to use as reference
    """
    Place				    Suggested value
	Free area  			    	2
	Urban area 		    		2.7-3.5
	Suburban area     			3-5
	Indoor line-of-sight		1.6-1.8
	Building blocked    		4-6
	Factory blocked     		2-3
    """
    d = pow(10, ((rx_power - dbm) / (10 * n)))
    return d

# David Young´s Distance Algorithm
def dbm_to_distance_DavidYoung(dbm, rx = -75, tx = -60):
    # dbm is what we measure
    # rx is dbm measure at 1 meter
    # tx is dbm send by BLE
    d = 1.21112*((dbm * rx/tx)**(7.560861)) + 0.251

if not os.geteuid() == 0:
    sys.exit("script only works as root")

btlib = find_library("bluetooth")
if not btlib:
    raise Exception(
        "Can't find required bluetooth libraries"
        " (need to install bluez)"
    )
bluez = CDLL(btlib, use_errno=True)

dev_id = bluez.hci_get_route(None)

sock = socket(AF_BLUETOOTH, SOCK_RAW, BTPROTO_HCI)
sock.bind((dev_id,))

err = bluez.hci_le_set_scan_parameters(sock.fileno(), 0, 0x10, 0x10, 0, 0, 1000);
if err < 0:
    raise Exception("Set scan parameters failed")
    # occurs when scanning is still enabled from previous call

# allows LE advertising events
hci_filter = struct.pack(
    "<IQH", 
    0x00000010, 
    0x4000000000000000, 
    0
)
sock.setsockopt(SOL_HCI, HCI_FILTER, hci_filter)

err = bluez.hci_le_set_scan_enable(
    sock.fileno(),
    1,  # 1 - turn on;  0 - turn off
    0, # 0-filtering disabled, 1-filter out duplicates
    1000  # timeout
)
if err < 0:
    errnum = get_errno()
    raise Exception("{} {}".format(
        errno.errorcode[errnum],
        os.strerror(errnum)
    ))

import time
while True:
    data = sock.recv(1024)
    # print bluetooth address from LE Advert. packet
    filterMac = ':'.join("{0:02x}".format(x) for x in data[12:6:-1])
    if filterMac in ["ac:23:3f:a9:bf:95", "ac:23:3f:a9:bf:7c", "b8:27:eb:ef:a5:87", "dc:a6:32:c1:20:46"]:
        if filterMac=="ac:23:3f:a9:bf:95": print("1 ->", end = '')
        elif filterMac=="ac:23:3f:a9:bf:7c": print("2 ->", end = '')
        elif filterMac=="b8:27:eb:ef:a5:87": print("rasp ->", end = '')
        elif filterMac=="dc:a6:32:c1:20:46": print("local ->", end = '')
        #print(','.join("{}".format(int(x)) for x in data),end = ' ->')
        #offset = 3
        #if(data[0+offset]==0x02): print("data[0]: OK with iBeacon")
        #if(data[1+offset]==0x01): print("data[1]: OK with iBeacon")
        #if(data[2+offset]==0x01): print("data[2]: OK with iBeacon")
        #print("Len of data = {} vs {}".format(data[3+offset], len(data)-offset))
        #if(data[4+offset:5+offset].hex()==0x4c00): print("APPLE iBeacon")
        #else: print("Another company iBeacon")
        
        #print(data[2+4])
        #if(data[2+3]==0x06): print("data[2]: OK")

        dbm = rssi_to_dbm(data[-1])
        dist = dbm_to_distance(dbm)
        print("RSSI: {}, Dist: {}".format(dbm, dist))
    """""
    print(':'.join("{0:02x}".format(x) for x in data),end = '')
    print(1)
    for x in data[12:6:-1]:
        print("{0:02x}".format(x))
    print(1)
    time.sleep(10)
    """""
    #print(':'.join("{0:02x}".format(x) for x in data[12:6:-1]))