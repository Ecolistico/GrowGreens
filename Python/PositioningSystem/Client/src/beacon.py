#!/usr/bin/env python3

# Import Directories
import struct
from time import time
from filter import Kalman

class Beacon:
    def __init__(self, name, data, logger = None):
        self.frame = 0
        self.version = 0
        self.battery = 0
        self.accel_X = 0
        self.accel_Y = 0
        self.accel_Z = 0
        self.name = name
        self.mac = data["mac"]
        self.rx_power = int(data["rx_power"]) # dBm at 1 meter
        self.devName = ""
        if data["rssi_always"] == "True": self.rssi_always = True
        else: self.rssi_always = False
        self.get_rssi = False
        self.dbm = 0
        self.distance = 0
        self.updateTimer = time()
        self.kalman_rssi = Kalman(r=1.4, q=0.065)
        self.kalman_distance = Kalman(r=1.4, q=0.065)
        self.log = logger

    def Msg2Log(self, mssg, level = 0):
        if self.log != None:
            if(level == 0 or level == "DEBUG"): self.log.debug(mssg)
            elif(level == 1 or level == "INFO"): self.log.info(mssg)
            elif(level == 2 or level == "WARNING"): self.log.warning(mssg)
            elif(level == 3 or level == "ERROR"): self.log.error(mssg)
            elif(level == 4 or level == "CRITICAL"): self.log.critical(mssg)
            else: self.log.debug(mssg)
        else: print(mssg)

    def rssi_to_dbm(self, rssi):
        self.dbm = self.kalman_rssi.update(rssi) - 256
        return self.dbm

    # Log Distance Path Loss
    def dbm_to_distance(self, dbm, rx_power = -59, n=1.6):
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
        self.updateTimer = time()
        return d

    # David Young´s Distance Algorithm
    def dbm_to_distance_DavidYoung(self, dbm, rx = -75, tx = -60):
        # dbm is what we measure
        # rx is dbm measure at 1 meter
        # tx is dbm send by BLE
        d = 1.21112*((dbm * rx/tx)**(7.560861)) + 0.251
        self.distance = d
        self.updateTimer = time()
        return d

    def getDistance(self): return self.distance

    # Decode Substructures
    def substructure_decode1(self, data):
        for i, b in enumerate(data):
            if i == posNextStructure and i != len(data)-1:
                posBeforeStructure = i
                posNextStructure += int(b) + 1
                self.Msg2Log("Structure size = {}".format(int(b)), 1)
                auxStr = ''
                for j in range(posNextStructure-posBeforeStructure-1):
                    auxStr += hex(data[posBeforeStructure+j+1]) + ':'
                    i = posNextStructure
                self.Msg2Log("{}" .format(auxStr), 1)
            elif i == len(data)-1: 
                dbm = self.rssi_to_dbm(b)
                dist = self.dbm_to_distance(dbm, self.rx_power)
                self.Msg2Log("Intensity: {}, Filter RSSI: {}, Dist: {}".format(b, dbm, dist), 1)
    
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
            self.Msg2Log("Service Discover Protocol: {}".format(mssg), 1)
        elif data[0] == 0x03:
            self.Msg2Log("UUID service: "+ ':'.join("{0:02x}".format(x) for x in data[1:3]), 1)
        else: self.Msg2Log("Substructure is unknown [{0:02x}]".format(data[0]), 1)
                
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
        
        if(len(splitData)==3 and len(splitData[0])==2 and len(splitData[1])==3 and splitData[1][1]==0xe1 and splitData[1][2]==0xff):
            if len(splitData[2])==18:
                self.Msg2Log("ACC structure identified", 1)
                self.get_rssi = True
                self.frame = splitData[2][3]
                self.version = splitData[2][4]
                self.battery = splitData[2][5]
                self.accel_X = float(struct.unpack('<h', (splitData[2][6:8]))[0] / (2**15))
                self.accel_Y = float(struct.unpack('<h', (splitData[2][8:10]))[0] / (2**15))
                self.accel_Z = float(struct.unpack('<h', (splitData[2][10:12]))[0] / (2**15))
                self.Msg2Log("Sensor info. Battery {}% - Accel(x, y, z) = ({}, {}, {})".format(self.battery, self.accel_X, self.accel_Y, self.accel_Z), 1)
            if len(splitData[2])==16:
                self.Msg2Log("Info structure identified, 1")
                self.devName = splitData[2][12:].decode("utf-8")
                self.Msg2Log("Device name is [{}]".format(self.devName), 1)
            else: self.Msg2Log("Structure Unknown", 1)
        elif(len(splitData)==3 and len(splitData[0])==2 and len(splitData[1])==3 and splitData[1][1]==0xaa and splitData[1][2]==0xfe):
            if splitData[2][3]==0x20 and splitData[2][4]==0x00:
                self.Msg2Log("TLM structure identified", 1)    
                for s in splitData: self.substructure_decode(s)
            elif splitData[2][3]==0x00 and splitData[2][4]==0xe8:
                name_id = "{}".format(''.join("{0:02x}".format(x) for x in splitData[2][5:15]))
                instance_id = "{}".format(''.join("{0:02x}".format(x) for x in splitData[2][15:]))
                self.Msg2Log("UID structure identified", 1)
                self.Msg2Log("Name ID: {} Instance ID: {}".format(name_id, instance_id), 1)
            elif splitData[2][3]==0x10 and splitData[2][4]==0xe8:
                prevUrl = "unknown//"
                finalUrl =  ".unknown"
                if splitData[2][5] == 0x01: prevUrl ="https//"
                if splitData[2][5] == 0x01: finalUrl = ".com"
                url = prevUrl + splitData[2][6:-1].decode("utf-8") + finalUrl
                self.Msg2Log("URL structure identified", 1)
                self.Msg2Log("url: {}".format(url), 1)
            else:
                self.Msg2Log("Unknown structure", 1)    
                for s in splitData: self.substructure_decode(s)
        elif(len(splitData)==2 and len(splitData[0])==2 and len(splitData[1])==26 and splitData[1][0]==0xff):
            self.Msg2Log("iBeacon Advertise", 1)
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
            self.Msg2Log("Manufacture Data: {} Company ID".format(company), 1)
            self.Msg2Log("UUID = {} Major = {} Minor {} RSSI = {}".format(uuid, major, minor, rssi), 1)
        else:   
            self.Msg2Log("{} substructures were identified".format(len(splitData)), 1)
            for s in splitData: self.substructure_decode(s)

    # Decode Data
    def decode(self, data):
        if self.mac == (':'.join("{0:02x}".format(x) for x in data[12:6:-1])):
            self.Msg2Log("", 1)
            self.Msg2Log("Mac[{}] - Device Name[{}]".format(self.mac, self.name), 1)
            mac = ':'.join("{0:02x}".format(x) for x in data)
            self.Msg2Log("{}".format(mac), 1)
            self.Msg2Log("{} bytes were sended".format(data[13]), 1)
            subData = data[14:]
            self.identify_substructures(subData)
            if self.rssi_always or self.get_rssi:
                self.get_rssi = False
                dbm = self.rssi_to_dbm(data[-1])
                dist = self.dbm_to_distance(dbm, self.rx_power)
                self.Msg2Log("Intensity: {}, Filter RSSI: {}, Dist: {}".format(data[-1], dbm, dist), 1)
