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
def signed(n, bits=16):
    n &= (1 << bits) - 1
    if n >> (bits - 1):
        n -= 1 << bits
    return n

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
    def __init__(self, data):
        self.frame = 0
        self.version = 0
        self.battery = 0
        self.accel_X = 0
        self.accel_Y = 0
        self.accel_Z = 0
        self.mac = data["mac"]
        self.name = data["name"]
        self.devName = ""
        if data["rssi_always"] == "True": self.rssi_always = True
        else: self.rssi_always = False
        self.get_rssi = False
        self.dbm = 0
        self.distance = 0
        self.kalman_rssi = Kalman(r=1.4, q=0.065)
        self.kalman_distance = Kalman(r=1.4, q=0.065)

    def rssi_to_dbm(self, rssi):
        self.dbm = self.kalman_rssi.update(rssi) - 256
        return self.dbm

    # Log Distance Path Loss
    def dbm_to_distance(self, dbm, rx_power = -59, n=2):
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

    # Decode Substructures
    def substructure_decode1(self, data):
        for i, b in enumerate(data):
            if i == posNextStructure and i != len(data)-1:
                posBeforeStructure = i
                posNextStructure += int(b) + 1
                print("Structure size = ", int(b))
                for j in range(posNextStructure-posBeforeStructure-1):
                    print("{}:".format(hex(data[posBeforeStructure+j+1])), end = '')
                    i = posNextStructure
                    #elif i == posBeforeStructure: print("Type of data structure is {}".format(b))
                print()
            elif i == len(data)-1: 
                dbm = self.rssi_to_dbm(b)
                dist = self.dbm_to_distance(dbm)
                print("Intensity: {}, Filter RSSI: {}, Dist: {}".format(b, dbm, dist))
    
    # Decode Substructures
    def substructure_decode(self, data):
        if data[0] == 0x01:
            mssg = "Error specs not found"
            if data[1] == 0x00: mssg = "Reserved"
            elif data[1] == 0x01: mssg = "Error Response"
            elif data[1] == 0x02: mssg = "Service Search Request"
            elif data[1] == 0x03: mssg = "Service Search Response"
            elif data[1] == 0x04: mssg = "Service Attribute Request" 
            elif data[1] == 0x05: mssg = "Service Attribute Response"
            elif data[1] == 0x06: mssg = "Service Search Attribute Request" 
            elif data[1] == 0x07: mssg = "Service Search Attribute Response"
            elif data[1] < 0xFF: mssg = "Service Attribute Request"
            print("Service Discover Protocol: {}".format(mssg))
        elif data[0] == 0x03:
            print("UUID service: "+ ':'.join("{0:02x}".format(x) for x in data[1:3]))
        else: print("Substructure is unknown [{0:02x}]".format(data[0]))
                
    # Decode Substructures
    def identify_substructures(self, data):
        posNextStructure = 0
        posBeforeStructure = 0
        aux_counter = 0
        splitData = []
        
        for i in range(len(data)):
            if i == aux_counter and aux_counter+data[aux_counter]<len(data):
                splitData.append(data[aux_counter + 1: aux_counter + 1 + data[aux_counter]])
                aux_counter += data[aux_counter] + 1
        
        #for d in splitData: print(':'.join("{0:02x}".format(x) for x in d))
        if(len(splitData)==3 and len(splitData[0])==2 and len(splitData[1])==3 and splitData[1][1]==0xe1 and splitData[1][2]==0xff):
            if len(splitData[2])==18:
                print("ACC structure identified")
                self.get_rssi = True
                self.frame = splitData[2][3]
                self.version = splitData[2][4]
                self.battery = splitData[2][5]
                self.accel_X = float(struct.unpack('<h', (splitData[2][6:8]))[0] / (2**15))
                self.accel_Y = float(struct.unpack('<h', (splitData[2][8:10]))[0] / (2**15))
                self.accel_Z = float(struct.unpack('<h', (splitData[2][10:12]))[0] / (2**15))
                print("Sensor info. Battery {}% - Accel(x, y, z) = ({}, {}, {})".format(self.battery, self.accel_X, self.accel_Y, self.accel_Z))
            if len(splitData[2])==16:
                print("Info structure identified")
                self.devName = splitData[2][12:].decode("utf-8")
                print("Device name is [{}]".format(self.devName))
            else: print("Structure Unknown")
        elif(len(splitData)==3 and len(splitData[0])==2 and len(splitData[1])==3 and splitData[1][1]==0xaa and splitData[1][2]==0xfe):
            if splitData[2][3]==0x20 and splitData[2][4]==0x00:
                print("TLM structure identified")    
                for s in splitData: self.substructure_decode(s)
            elif splitData[2][3]==0x00 and splitData[2][4]==0xe8:
                name_id = "{}".format(''.join("{0:02x}".format(x) for x in splitData[2][5:15]))
                instance_id = "{}".format(''.join("{0:02x}".format(x) for x in splitData[2][15:]))
                print("UID structure identified")
                print("Name ID: {} Instance ID: {}".format(name_id, instance_id))
            elif splitData[2][3]==0x10 and splitData[2][4]==0xe8:
                prevUrl = "unknown//"
                finalUrl =  ".unknown"
                if splitData[2][5] == 0x01: prevUrl ="https//"
                if splitData[2][5] == 0x01: finalUrl = ".com"
                url = prevUrl + splitData[2][6:-1].decode("utf-8") + finalUrl
                print("URL structure identified")
                print("url: {}".format(url))
            else:
                print("Unknown structure")    
                for s in splitData: self.substructure_decode(s)
        elif(len(splitData)==2 and len(splitData[0])==2 and len(splitData[1])==26 and splitData[1][0]==0xff):
            print("iBeacon Advertise")
            str_company = ':'.join("{0:02x}".format(x) for x in splitData[1][2:0:-1])
            company = "Unknown"
            if str_company == "00:4c": company = "Apple"
            uuid = "{}-{}-{}-{}-{}".format(''.join("{0:02x}".format(x) for x in splitData[1][5:9]),
                                           ''.join("{0:02x}".format(x) for x in splitData[1][9:11]),
                                           ''.join("{0:02x}".format(x) for x in splitData[1][11:13]),
                                           ''.join("{0:02x}".format(x) for x in splitData[1][13:15]),
                                           ''.join("{0:02x}".format(x) for x in splitData[1][15:21])
                                           )
            major = int("{}".format(''.join("{0:02x}".format(x) for x in splitData[1][21:23])), 16)
            minor = int("{}".format(''.join("{0:02x}".format(x) for x in splitData[1][23:25])), 16)
            rssi = splitData[1][25] - 256
            print("Manufacture Data: {} Company ID".format(company))
            print("UUID = {} Major = {} Minor {} RSSI = {}".format(uuid, major, minor, rssi))
        else:   
            print("{} substructures were identified".format(len(splitData)))
            for s in splitData: self.substructure_decode(s)

    # Decode Data
    def decode(self, data):
        if self.mac == (':'.join("{0:02x}".format(x) for x in data[12:6:-1])):
            print("\nMac[{}] - Device Name[{}]".format(self.mac, self.name))
            print(':'.join("{0:02x}".format(x) for x in data))
            print("{} bytes were sended".format(data[13]))
            subData = data[14:]
            self.identify_substructures(subData)
            if self.rssi_always or self.get_rssi:
                self.get_rssi = False
                dbm = self.rssi_to_dbm(data[-1])
                dist = self.dbm_to_distance(dbm)
                print("Intensity: {}, Filter RSSI: {}, Dist: {}".format(data[-1], dbm, dist))

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
bluetoothMac = {"0":{"mac": "ac:23:3f:a9:bf:95", "name": "1", "rssi_always": "False"},
                "1":{"mac": "ac:23:3f:a9:bf:7c", "name": "2", "rssi_always": "False"},
                "2":{"mac": "b8:27:eb:ef:a5:87", "name": "rasp", "rssi_always": "True"}}
#bluetoothMac = {"0":{"mac": "ac:23:3f:a9:bf:95", "name": "1"}, "1":{"mac": "b8:27:eb:ef:a5:87", "name": "rasp"}}
myBeacons = []
for dev in bluetoothMac: myBeacons.append(Beacon(bluetoothMac[dev]))

while True:
    data = sock.recv(1024)
    # print bluetooth address from LE Advert. packet
    filterMac = ':'.join("{0:02x}".format(x) for x in data[12:6:-1])
    if filterMac in [dev.mac for dev in myBeacons]:
        for i in range(len(myBeacons)): myBeacons[i].decode(data)