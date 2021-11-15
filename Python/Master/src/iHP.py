import os
import struct
import socket
import random
import binascii
import subprocess
import xml.etree.ElementTree as ET
from sysMaster import scan_for_hosts, find_ip_address_for_mac_address, splitBytes2Hex, splitByte, splitIntoBits

class IHP:
    # Initialize the class
    def __init__(self, MAC, ip_range, port, logger = None):
        self.MAC = MAC
        self.ip_range = ip_range
        self.port = port
        self.log = logger
        self.ScanIP() # initialize self.ip and self.connected

        # Default variables
        self.a = ''
        self.b = ''
        self.c = ''
        self.d = ''
        self.maxVoltage = 80
        self.maxCurrent = 37

        # Timing
        self.TIMING = 0.1 # 100ms between each command is send

        # Internal Device Address
        self.COMMS   = 0x00
        self.PFC1    = 0x07
        self.PFC2    = 0x08
        self.Module1 = 0x10
        self.Module2 = 0x11
        self.Module3 = 0x12
        self.Module4 = 0x13
        self.Module5 = 0x14
        self.Module6 = 0x15
        self.Module7 = 0x16
        self.Module8 = 0x17
        self.Group1  = 0x18
        self.Group2  = 0x19
        self.Group3  = 0x1A
        self.Group4  = 0x1B
        self.Group5  = 0x1C
        self.Group6  = 0x1D
        self.Group7  = 0x1E

        # Error Codes
        self.ERR_SUCCESS                = 0 # Success / No Error
        self.ERR_RACK_NOT_EXISTING      = 1 # No iHP Rack available on the specified address
        self.ERR_DEVICE_NOT_EXISTING    = 2 # Invalid / No devices existing on the specified device address
        self.ERR_UNSUPPORTED_CMD        = 3 # Unsupported command code
        self.ERR_OPERATION_INVALID      = 4 # Operation not supported – that command is not valid for reading/writing (depending on what operation was issued)
        self.ERR_LENGTH_INVALID         = 5 # The length given is invalid for the command code
        self.ERR_DATA_INVALID           = 6 # The data provided doesn’t match what was expected
        self.ERR_WRITE_PROTECT          = 7 # The command is valid but the data is write protected
        self.ERR_PROTOCOL_INVALID       = 8 # There was an error parsing the command.

    # Manage log if logger object exist if not just print
    def str2log(self, msg, level = 'DEBUG'):
        if self.log!=None:
            if (level==0 or level=='DEBUG'): self.log.debug("(iHP PS) {}".format(msg))
            elif (level==1 or level=='INFO'): self.log.info("(iHP PS) {}".format(msg))
            elif (level==2 or level=='WARNING'): self.log.warning("(iHP PS) {}".format(msg))
            elif (level==3 or level=='ERROR'): self.log.error("(iHP PS) {}".format(msg))
            elif (level==4 or level=='CRITICAL'): self.log.critical("(iHP PS) {}".format(msg))
        else: print('{},(iHP PS) {}'.format(level, msg))

    # Returns 4 Random bytes
    def RandomValues(self):
        aux1 = hex(random.randrange(0,255))[2:]
        aux2 = hex(random.randrange(0,255))[2:]
        aux3 = hex(random.randrange(0,255))[2:]
        aux4 = hex(random.randrange(0,255))[2:]
        if len(aux1)<2: self.a = int("0" + aux1, 16)
        else:
            self.a = 1
            self.str2log("Random Value A is out of range. Giving 1 as default", 3)
        if len(aux2)<2: self.b = int("0" + aux2, 16)
        else:
            self.b = 2
            self.str2log("Random Value B is out of range. Giving 2 as default", 3)
        if len(aux3)<2: self.c = int("0" + aux3, 16)
        else:
            self.c = 3
            self.str2log("Random Value C is out of range. Giving 3 as default", 3)
        if len(aux4)<2: self.d = int("0" + aux4, 16)
        else:
            self.d = 4
            self.str2log("Random Value D is out of range. Giving 4 as default", 3)

    # Scan the IP in the local network to find iHP device
    def ScanIP(self):
        xml = scan_for_hosts(self.ip_range)
        ip_address = find_ip_address_for_mac_address(xml, self.MAC)
        if ip_address:
            self.connected = True
            self.ip = ip_address
            self.str2log('Found IP address {} for MAC address {} in IP address range {}'.format(ip_address, self.MAC, self.ip_range), 1)
        else:
            self.connected = False
            self.ip = ""
            self.str2log('No IP address found for MAC address {} in IP address range {}'.format(self.MAC, self.ip_range), 2)

    # Enable/Disable Write to Devices
    def WriteProtect(self, device, enable):
        if self.connected:
            self.RandomValues()
            e = 0xA1
            if(device == 0): f = 0x00   # ISOCOMM
            elif(device>=1 and device <=8): f = 0x10 + device - 1 # Module 1-8
            else:
                self.str2log("WriteProtect() parameter device is wrong, provide a correct value", 3)
                return False
            g = 0x01
            h = 0x10
            if(enable == True): i = 0x00
            elif(enable == False): i = 0x80
            bytes2send = bytes([self.a, self.b, self.c, self.d, e, f, g, h, i])
            return communicationUDP(self, bytes2send)
        else:
            self.str2log("is disconnected, please provide a correct MAC or IP address", 3)
            return False

    # Change device to Digital Current Source (DCS) Mode
    def DCS(self, device):
        if self.connected:
            self.str2log("Change Module n to Digital Current Source")
            self.RandomValues()
            e = 0xA1
            f = 0x00
            g = 0x02
            h = 0xD3
            if(device>=1 and device<=8): i = 0x00 + device - 1   # Module 1-8
            else:
                self.str2log("DCS() parameter device is wrong, provide a correct value", 3)
                return False
            j = 0x08
            bytes2send = bytes([self.a, self.b, self.c, self.d, e, f, g, h, i, j])
            return communicationUDP(self, bytes2send)
        else:
            self.str2log("is disconnected, please provide a correct MAC or IP address", 3)
            return False

    # Change device Voltage
    def setVoltage(self, device, voltage):
        if self.connected:
            self.RandomValues()
            e = 0xA1
            if(device>=1 and device<=8): f = 0x10 + device - 1 # Module 1-8
            else:
                self.str2log("setVoltage() parameter device is wrong, provide a correct value", 3)
                return False
            g = 0x04
            h = 0xB1
            i = 0x03
            # Pendiente agregar límites de voltaje para no mandar valores fuera de rango
            # Mejor aún configurar el voltaje como función porcentual 0-100%
            mult = voltage * 10000
            fmt = "<{}".format("L")
            bytes = struct.pack(fmt, mult)
            F1 = "".join("{:02x}".format(byte) for byte in bytes)
            j1 = int(F1[:2], 16)
            j2 = int(F1[2:4], 16)
            j3 = int(F1[-4:-2], 16)
            bytes2send = bytearray([self.a, self.b, self.c, self.d, e, f, g, h, i, j1, j2, j3])
            return communicationUDP(self, bytes2send)
        else:
            self.str2log("is disconnected, please provide a correct MAC or IP address", 3)
            return False

    def setVoltagePercentage(self, device, voltage):
        self.Voltage(device, voltage*self.maxVoltage/100)

    # Change device Current
    def setCurrent(self, device, current):
        if self.connected:
            self.RandomValues()
            e = 0xA1
            if(device>=1 and device<=8): f = 0x10 + device - 1 # Module 1-8
            else:
                self.str2log("setCurrent() parameter device is wrong, provide a correct value", 3)
                return False
            g = 0x04
            h = 0xB2
            i = 0x03
            # Pendiente agregar límites de corriente para no mandar valores fuera de rango
            # Mejor aún configurar la corriente como función porcentual 0-100%
            mult = current * 10000
            fmt = "<{}".format("L")
            bytes = struct.pack(fmt, mult)
            F1 = "".join("{:02x}".format(byte) for byte in bytes)
            j1 = int(F1[:2], 16)
            j2 = int(F1[2:4], 16)
            j3 = int(F1[-4:-2], 16)
            bytes2send = bytearray([self.a, self.b, self.c, self.d, e, f, g, h, i, j1, j2, j3])
            return communicationUDP(self, bytes2send)
        else:
            self.str2log("is disconnected, please provide a correct MAC or IP address", 3)
            return False

    def setCurrentPercentage(self, device, current):
        self.Voltage(device, current*self.maxCurrent/100)

    # Device Turn On/Off
    def enable(self, device, en):
        if self.connected:
            self.RandomValues()
            e = 0xA1
            if(device>=1 and device<=8): f = 0x10 + device - 1 # Module 1-8
            else:
                self.str2log("enable() parameter device is wrong, provide a correct value", 3)
                return False
            g = 0x01
            h = 0x01
            if(en == True): i = 0x80
            elif(en == False): i = 0x00
            bytes2send = bytes([self.a, self.b, self.c, self.d, e, f, g, h, i])
            return communicationUDP(self, bytes2send)
        else:
            self.str2log("is disconnected, please provide a correct MAC or IP address", 3)
            return False

    # Module Save to ISOCOMM
    def save(self):
        if self.connected:
            self.RandomValues()
            e = 0xA1
            f = 0x00
            g = 0x01
            h = 0xD7
            i = 0x00
            bytes2send = bytes([self.a, self.b, self.c, self.d, e, f, g, h, i])
            return communicationUDP(self, bytes2send)
        else:
            self.str2log("is disconnected, please provide a correct MAC or IP address", 3)
            return False

    # Compare first 4 bytes with the first ones that we sent. It is like our command ID
    def check4FBytes(self, bytes):
        a = int(bytes[0], 16)
        b = int(bytes[1], 16)
        c = int(bytes[2], 16)
        d = int(bytes[3], 16)
        if (self.a==a and self.b==b and self.c==c and self.d==d):
            self.str2log("The first 4 bytes are correct {} {} {} {}".format(a, b, c, d))
            return True
        else:
            self.str2log("First 4 bytes ERROR Recieve: {} {} {} {} Sent: {} {} {} {}".format(a, b, c, d, self.a, self.b, self.c, self.d), 3)
            return False

    # Analyze Byte 5. In this byte we could know if there is a error message
    def check5Byte(self, bytes):
        byte = int(bytes[4], 16)
        listBit = splitIntoBits(byte)
        #self.str2log("List Byte5 {}".format(listBit))
        joinBits = [listBit[1], listBit[2], listBit[3], listBit[4]]
        auxBits = "".join(map(str, joinBits))
        resp = int(auxBits, 2)

        if (listBit[0]==1): self.str2log("Byte 5 Bit[0]: Command response is included with this message")
        elif(listBit[0]==0): self.str2log("Byte 5 Bit[0]: Message is an acknowledgement receipt of a BLANK message")

        if(auxBits!=0 or listBit[5]!=1 or listBit[7]!=0): self.str2log("Byte 5 ERROR: Check Bits 1,2,3,4,5 and 7 with Artesyn documentation values are wrong", 3)

        if(listBit[6]==0): return True
        elif(listBit[6]==1):
            self.str2log("Byte 5 bit[6]: Error code sent in next byte", 3)
            return False

    # Analyze Byte 6. Check error messages
    def check6Byte(self, bytes):
        byte = int(bytes[5], 16)
        listBit = splitIntoBits(byte)
        #self.str2log("List Bit, Byte6 {}".format(listBit))
        joinBits = [listBit[0], listBit[1], listBit[2], listBit[3]]
        auxBits = "".join(map(str, joinBits))
        resp = int(auxBits, 2)

        if (resp!=0) self.str2log("Byte 6 ERROR: Check Bits 0,1,2,3 with Artesyn documentaion values should be zero")

        joinBits = [listBit[4], listBit[5], listBit[6], listBit[7]]
        auxBits = "".join(map(str, joinBits))
        resp = int(auxBits, 2)

        if(resp==self.ERR_SUCCESS): return True
        else:
            if(resp==self.ERR_RACK_NOT_EXISTING): self.str2log("No iHP Rack available on the specified address", 3)
            elif(resp==self.ERR_DEVICE_NOT_EXISTING): self.str2log("Invalid / No devices existing on the specified device address", 3)
            elif(resp==self.ERR_UNSUPPORTED_CMD): self.str2log("Unsupported command code", 3)
            elif(resp==self.ERR_OPERATION_INVALID): self.str2log("Operation not supported – that command is not valid for reading/writing (depending on what operation was issued)", 3)
            elif(resp==self.ERR_LENGTH_INVALID): self.str2log("The length given is invalid for the command code", 3)
            elif(resp==self.ERR_DATA_INVALID): self.str2log("The data provided doesn’t match what was expected", 3)
            elif(resp==self.ERR_WRITE_PROTECT): self.str2log("The command is valid but the data is write protected", 3)
            elif(resp==self.ERR_PROTOCOL_INVALID): self.str2log("There was an error parsing the command", 3)
            else: self.str2log("Error Command {} reserved for future use. Please ask Artesyn for further information".format(resp), 3)
            return False

    # Analyze Byte 7. Check the internal device address
    def check7Byte(self, bytes):
        byte = int(bytes[6], 16)
        listBit = splitIntoBits(byte)
        #self.str2log("List Bit, Byte7 {}".format(listBit))
        joinBits = [listBit[0], listBit[1], listBit[2], listBit[3], listBit[4]]
        auxBits = "".join(map(str, joinBits))
        resp = int(auxBits, 2)
        # PENDIENTE
        if(resp==self.COMMS): self.str2log("Internal Device Address: {}".format(resp))
        elif(resp==self.PFC1)
        elif(resp==self.PFC2)
        elif(resp==self.Module1)
        elif(resp==self.Module2)
        elif(resp==self.Module3)
        elif(resp==self.Module4)
        elif(resp==self.Module5)
        elif(resp==self.Module6)
        elif(resp==self.Module7)
        elif(resp==self.Module8)
        elif(resp==self.Group1)
        elif(resp==self.Group2)
        elif(resp==self.Group3)
        elif(resp==self.Group4)
        elif(resp==self.Group5)
        elif(resp==self.Group6)
        elif(resp==self.Group7)

    # Analyze Byte8
    def check8Byte(self, bytes):
        byte = int(bytes[7], 16)
        listBit = splitIntoBits(byte)
        self.str2log("List Bit, Byte8 {}".format(listBit))
        valf1 = [listBit[0], listBit[1], listBit[2], listBit[3], listBit[4],listBit[5]]
        ba = "".join(map(str, valf1))
        numero1 = int(ba, 2)
        self.str2log("BYTE8:Bit0-5:length of the data included in this message starting from 10th Byte: {}".format(numero1))
        #A implementar lecturas
        self.str2log("BYTE8:Bit6: Value should be 0: {}".format(listBit[6]))
        self.str2log("BYTE8:Bit7: Value should be 0: {}".format(listBit[6]))

    # Analyze Byte9
    def check9Byte(self, bytes):
        byte = int(bytes[8], 16)
        listBit = splitIntoBits(byte)
        self.str2log("List Bit, Byte9 {}".format(listBit))
        ba = "".join(map(str, listBit))
        numero1 = int(ba, 2)
        self.str2log("Command code for Module, ISOCOMM, or PFC: {}".format(numero1))

    # Main Communication Function
    def communicationUDP(self, bytes2send):
        success = False

        serverAddressPort = (self.ip, self.port)
        bufferSize = 1024

        # Create a UDP socket at client side
        with socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM) as udp:
            # Send to server using created UDP socket
            udp.settimeout(self.TIMING) # Sets the socket to timeout after X time of no activity
            udp.sendto(bytes2send, serverAddressPort)
            self.str2log("Sent: {} Len: {}".format(bytes2send, len(bytes2send)))
            try:
                msgFromServer = udp.recvfrom(bufferSize)
                msg = "Message from Server {}".format(msgFromServer[0])
                self.str2log(msg)
                data = msgFromServer[0]
                bytes = splitBytes2Hex(data)

                self.check4FBytes(bytes)
                self.check5Byte(bytes)
                self.check6Byte(bytes)
                self.check7Byte(bytes)
                self.check8Byte(bytes)
                self.check9Byte(bytes)
            except socket.TimeoutError: # fail after 1 second of no activity
                self.str2log("Didn't receive data! [Timeout]")
                return False

        return success

# Debug
def main():
    ihp = IHP('D0:03:EB:A2:DD:14', '192.168.6.1/23', 8888)
    ihp.self.WriteProtect(6, True)
    #ihp.self.DCS(6)
    #ihp.self.setVoltage()
    #ihp.self.setVoltagePercentage()
    #ihp.self.setCurrent(2, 35)
    #ihp.self.setCurrentPercentage()
    #ihp.self.enable()
    #ihp.self.save()

if __name__ == '__main__':
    main()
