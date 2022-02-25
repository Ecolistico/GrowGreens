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

class Beacon:
    def __init__(self, mac, name):
        self.kalman = Kalman(r=1.4, q=0.065)
        self.dbm = 0
        self.distance = 0
        self.mac = mac
        self.name = name

    def rssi_to_dbm(self, rssi):
        self.dbm = self.kalman.update(rssi) - 256
        return self.dbm

    # Log Distance Path Loss
    def dbm_to_distance(self, dbm, rx_power = -75, n=2):
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
        self.distance = d
        return d

    # David Young´s Distance Algorithm
    def dbm_to_distance_DavidYoung(dbm, rx = -75, tx = -60):
        # dbm is what we measure
        # rx is dbm measure at 1 meter
        # tx is dbm send by BLE
        d = 1.21112*((dbm * rx/tx)**(7.560861)) + 0.251
        self.distance = d
        return d
    
    # Check Data
    def check_data(self, data):
        if self.mac == (':'.join("{0:02x}".format(x) for x in data[12:6:-1])):
            print("Mac[{}] - Device Name[{}]".format(self.mac, self.name))
            posNextStructure = 0
            posBeforeStructure = 0
            for i, b in enumerate(data):
                if i == posNextStructure:
                    print()
                    posBeforeStructure = i
                    posNextStructure += int(b)
                    print("Structure size = ", int(b))
                elif i == posBeforeStructure: print("Type of data structure is {}".format(b))
                elif i == len(data) - 1: 
                    dbm = self.rssi_to_dbm(b)
                    dist = self.dbm_to_distance(dbm)
                    print("Intensity: {}, Filter RSSI: {}, Dist: {}".format(b, dbm, dist))
                else: print("{},".format(b), end = '')

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
bluetoothMac = {"0":{"mac": "ac:23:3f:a9:bf:95", "name": "1"}, "1":{"mac": "ac:23:3f:a9:bf:7c", "name": "2"}, "0":{"mac": "b8:27:eb:ef:a5:87", "name": "rasp"}}
myBeacons = []
for dev in bluetoothMac: myBeacons.append(Beacon(dev["mac"], dev["name"]))

while True:
    data = sock.recv(1024)
    # print bluetooth address from LE Advert. packet
    filterMac = ':'.join("{0:02x}".format(x) for x in data[12:6:-1])
    if filterMac in [dev.mac for dev in myBeacons]:
        for dev in myBeacons: myBeacons.check_data(data)