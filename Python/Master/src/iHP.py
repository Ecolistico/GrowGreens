#!/usr/bin/env python3

# Import directories
import struct
import socket
from time import time
from random import randrange
from sysMaster import scan_for_hosts, find_ip_address_for_mac_address, splitBytes2Hex, splitIntoBits, listBytestoDecimal

class IHP:
    # Initialize the class
    def __init__(self, data, logger_main = None, logger_ihp = None):

        self.MAC = data['MAC']
        self.ip_range = data['ip_range']
        self.port = int(data['port'])
        self.log = logger_main
        self.log_ihp = logger_ihp
        
        # Create a UDP socket at client side
        self.TIMEOUT = 2 # timeout grater than 1s is required (from experimental test)
        self.ScanIP() # initialize self.ip and self.connected
        if self.connected:
            self.serverAddressPort = (self.ip, self.port)
            self.ClientSocket = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)
            self.ClientSocket.settimeout(self.TIMEOUT) # Sets the socket to timeout after X time of no activity
        else: 
            self.serverAddressPort = None
            self.ClientSocket = None

        # Default variables
        self.id1 = 0x00
        self.id2 = 0x00
        self.id3 = 0x00
        self.id4 = 0x00
        self.command = 0x00
        self.commandList = []
        self.running = False
        self.actualCommand = 0
        self.errorCounter = 0
        self.timing = 0
        self.actualTime = 0
        self.waitRequest = False
        
        # Constants
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

        self.MAX_ERROR                  = 3 # Max retries when command fails (count start at zero)

        # Command codes
        self.OPERATION      = 0x01  # Set Operation ON/OFF
        self.CLEAR_FAULTS   = 0x03  # Clear faults
        self.WRITE_PROTECT  = 0x10  # Enable/Disable write protection
        self.READ_VOUT      = 0x8B  # Read the output voltage
        self.READ_IOUT      = 0x8C  # Read the output current
        self.VREF           = 0xB1  # Set the reference voltage
        self.IREF           = 0xB2  # Set reference current
        self.READ_VIN       = 0xCA  # Read the input voltage
        self.READ_IIN       = 0xCD  # Read the input current
        self.MODULE_CONFIG  = 0xD3  # Module configuration
        self.SAVE           = 0xD7  # Save configuration

        # Ping or Request
        self.PING       = 0xA0  # Ping (check order if inverted then is 0x05)
        self.REQUEST    = 0xA1  # Request. Indicates that a command code is included (check order if inverted then is 0x85)

        # Read or Write
        self.READ       = 0x40  # Read. Indicates that the command is in read mode
        self.WRITE      = 0x00  # Write. Indicates that the command is in write mode

        # This variables may change according to iHP modules installed
        self.maxVoltage = 80
        self.maxCurrent = 37

    # Manage log if logger object exist if not just print
    def str2log(self, msg, logger, level = 'DEBUG'):
        if(level==0): level = 'DEBUG'
        elif(level==1): level = 'INFO'
        elif(level==2): level = 'WARNING'
        elif(level==3): level = 'ERROR'
        elif(level==4): level = 'CRITICAL'
        
        if logger!=None:
            if (level=='DEBUG'): logger.debug("(iHP PS) {}".format(msg))
            elif (level=='INFO'): logger.info("(iHP PS) {}".format(msg))
            elif (level=='WARNING'): logger.warning("(iHP PS) {}".format(msg))
            elif (level=='ERROR'): logger.error("(iHP PS) {}".format(msg))
            elif (level=='CRITICAL'): logger.critical("(iHP PS) {}".format(msg))
            
        else: print('{},(iHP PS) {}'.format(level, msg))

    # Print the error and manage MAX_ERROR_EXCEED case
    def errorWithCommand(self, error):
        self.str2log(error, self.log_ihp, 3)
        self.errorCounter += 1
        if(self.errorCounter > self.MAX_ERROR):
            cList = self.commandList[0] # Get the first array of commands
            c = cList[self.actualCommand]['command']
            self.str2log("Too many errors with command {}, aborting request {}".format(c, cList['request']), self.log, 4)
            self.commandList = self.commandList[1:] # Delete the first request because it failed
            self.actualCommand = 0
            self.errorCounter = 0

    # When command succeed
    def commandSucceed(self):
        self.actualCommand += 1
        self.errorCounter = 0
        self.waitRequest = False

    # Returns 4 Random bytes
    def RandomValues(self):
        aux1 = hex(randrange(0,255))[2:]
        aux2 = hex(randrange(0,255))[2:]
        aux3 = hex(randrange(0,255))[2:]
        aux4 = hex(randrange(0,255))[2:]
        
        if len(aux1)<=2: self.id1 = int("0" + aux1, 16)
        else:
            self.id1 = 1
            self.str2log("Random Value A is out of range. Giving 1 as default", self.log_ihp, 3)
        if len(aux2)<=2: self.id2 = int("0" + aux2, 16)
        else:
            self.id2 = 2
            self.str2log("Random Value B is out of range. Giving 2 as default", self.log_ihp, 3)
        if len(aux3)<=2: self.id3 = int("0" + aux3, 16)
        else:
            self.id3 = 3
            self.str2log("Random Value C is out of range. Giving 3 as default", self.log_ihp, 3)
        if len(aux4)<=2: self.id4 = int("0" + aux4, 16)
        else:
            self.id4 = 4
            self.str2log("Random Value D is out of range. Giving 4 as default", self.log_ihp, 3)

    # Scan the IP in the local network to find iHP device
    def ScanIP(self):
        xml = scan_for_hosts(self.ip_range)
        ip_address = find_ip_address_for_mac_address(xml, self.MAC)
        if ip_address:
            self.connected = True
            self.ip = ip_address
            self.str2log('Found IP address {} for MAC address {} in IP address range {}'.format(ip_address, self.MAC, self.ip_range), self.log, 1)
        else:
            self.connected = False
            self.ip = ""
            self.str2log('No IP address found for MAC address {} in IP address range {}'.format(self.MAC, self.ip_range), self.log, 2)

    # Enable/Disable Write to Devices
    def WriteProtect(self, device, enable):
        if self.connected:
            self.str2log("WriteProtect {} Device {}".format('enable' if enable else 'disable', device), self.log_ihp)
            self.RandomValues()
            id5 = self.REQUEST
            if(device == 0): id6 = self.COMMS   # ISOCOMM
            elif(device>=1 and device <=8): id6 = self.Module1 + device - 1 # Module 1-8
            else:
                self.str2log("WriteProtect() parameter device is wrong, provide a correct value", self.log_ihp, 3)
                return False
            id7 = self.WRITE + 1 # Write mode + 1 aditional bytes
            self.command = self.WRITE_PROTECT
            if(enable == True): id9 = 0x00
            elif(enable == False): id9 = 0x80
            self.timing = 0.06
            bytes2send = bytes([self.id1, self.id2, self.id3, self.id4, id5, id6, id7, self.command, id9])
            return self.communicationUDP(bytes2send)
        else:
            self.str2log("is disconnected, please provide a correct MAC or IP address", self.log_ihp, 3)
            return False

    # Change device to Digital Current Source (DCS) Mode
    def DCS(self, device):
        if self.connected:
            self.str2log("Change Module {} to Digital Current Source".format(device), self.log_ihp)
            self.RandomValues()
            id5 = self.REQUEST
            id6 = self.COMMS
            id7 = self.WRITE + 2 # Write mode + 2 aditional bytes
            self.command = self.MODULE_CONFIG
            if(device>=1 and device<=8): id9 = 0x00 + device - 1   # Module 1-8
            else:
                self.str2log("DCS() parameter device is wrong, provide a correct value", self.log_ihp, 3)
                return False
            id10 = 0x08
            self.timing = 0.06
            bytes2send = bytes([self.id1, self.id2, self.id3, self.id4, id5, id6, id7, self.command, id9, id10])
            return self.communicationUDP(bytes2send)
        else:
            self.str2log("is disconnected, please provide a correct MAC or IP address", self.log_ihp, 3)
            return False

    # Change device Voltage
    def setVoltage(self, device, voltage):
        if self.connected:
            self.str2log("Change voltage from module {} to {} V".format(device, voltage), self.log_ihp)
            self.RandomValues()
            id5 = self.REQUEST
            if(device>=1 and device<=8): id6 = self.Module1 + device - 1 # Module 1-8
            else:
                self.str2log("setVoltage() parameter device is wrong, provide a correct value", self.log_ihp, 3)
                return False
            id7 = self.WRITE + 4 # Write mode + 4 aditional bytes
            self.command = self.VREF
            id9 = 0x03

            if(voltage>self.maxVoltage):
                self.str2log("voltage has to be between 0 and {} V".format(self.maxVoltage), self.log_ihp, 3)
                return False
            if(voltage < 0): voltage = 0    
            mult = voltage * 10000
            fmt = "<{}".format("L")
            bytes = struct.pack(fmt, int(mult))
            parameters = "".join("{:02x}".format(byte) for byte in bytes)
            id10 = int(parameters[:2], 16)
            id11 = int(parameters[2:4], 16)
            id12 = int(parameters[-4:-2], 16)
            self.timing = 0.06
            bytes2send = bytearray([self.id1, self.id2, self.id3, self.id4, id5, id6, id7, self.command, id9, id10, id11, id12])
            return self.communicationUDP(bytes2send)
        else:
            self.str2log("is disconnected, please provide a correct MAC or IP address", self.log_ihp, 3)
            return False

    # DEBUG function does not work properly
    def setVoltagePercentage(self, device, voltage): return self.setVoltage(device, voltage*self.maxVoltage/100)

    # Change device Current
    def setCurrent(self, device, current):
        if self.connected:
            self.str2log("Change current from module {} to {} A".format(device, current), self.log_ihp)
            self.RandomValues()
            id5 = self.REQUEST
            if(device>=1 and device<=8): id6 = self.Module1 + device - 1 # Module 1-8
            else:
                self.str2log("setCurrent() parameter device is wrong, provide a correct value", self.log_ihp, 3)
                return False
            id7 = self.WRITE + 4 # self.WRITE + 4 aditional bytes
            self.command = self.IREF
            id9 = 0x03

            if(current>self.maxCurrent):
                self.str2log("current has to be between 0 and {} A".format(self.maxCurrent), self.log_ihp, 3)
                return False
            if(current < 0): current = 0
            mult = current * 10000
            fmt = "<{}".format("L")
            bytes = struct.pack(fmt, int(mult))
            parameters = "".join("{:02x}".format(byte) for byte in bytes)
            id10 = int(parameters[:2], 16)
            id11 = int(parameters[2:4], 16)
            id12 = int(parameters[-4:-2], 16)
            self.timing = 0.06
            bytes2send = bytearray([self.id1, self.id2, self.id3, self.id4, id5, id6, id7, self.command, id9, id10, id11, id12])
            return self.communicationUDP(bytes2send)
        else:
            self.str2log("is disconnected, please provide a correct MAC or IP address", self.log_ihp, 3)
            return False

    # DEBUG function does not work properly
    def setCurrentPercentage(self, device, current): return self.setCurrent(device, current*self.maxCurrent/100)

    # Device Turn On/Off
    def operation(self, device, en):
        if self.connected:
            self.str2log("Turning {} Module {}".format('on' if en else 'off', device), self.log_ihp)
            self.RandomValues()
            id5 = self.REQUEST
            if(device>=1 and device<=8): id6 = self.Module1 + device - 1 # Module 1-8
            else:
                self.str2log("enable() parameter device is wrong, provide a correct value", self.log_ihp, 3)
                return False
            id7 = self.WRITE + 1 # Write Mode + 1 aditional bytes
            self.command = self.OPERATION
            if(en == True): id9 = 0x80
            elif(en == False): id9 = 0x00
            self.timing = 0.06
            bytes2send = bytes([self.id1, self.id2, self.id3, self.id4, id5, id6, id7, self.command, id9])
            return self.communicationUDP(bytes2send)
        else:
            self.str2log("is disconnected, please provide a correct MAC or IP address", self.log_ihp, 3)
            return False

    # Module Save to ISOCOMM
    def save(self, device):
        if self.connected:
            self.str2log("Saving parameters to ISOCOMM", self.log_ihp)
            self.RandomValues()
            id5 = self.REQUEST
            id6 = self.COMMS
            id7 = self.WRITE + 1 # Write mode + 1 aditional bytes
            self.command = self.SAVE
            if(device>=1 and device<=8): id9 = 0x00 + device - 1
            else:
                self.str2log("save() parameter device is wrong, provide a correct value", self.log_ihp, 3)
                return False
            self.timing = 0.06
            bytes2send = bytes([self.id1, self.id2, self.id3, self.id4, id5, id6, id7, self.command, id9])
            return self.communicationUDP(bytes2send)
        else:
            self.str2log("is disconnected, please provide a correct MAC or IP address", self.log_ihp, 3)
            return False

    # Read Voltage from device
    def readVout(self, device):
        if self.connected:
            self.str2log("Request voltage to Module {}".format(device), self.log_ihp)
            self.RandomValues()
            id5 = self.REQUEST
            if(device>=1 and device<=8): id6 = self.Module1 + device - 1
            else:
                self.str2log("readVout() parameter device is wrong, provide a correct value", self.log_ihp, 3)
                return False
            id7 = self.READ + 0 # Read Mode without aditional bytes
            self.command = self.READ_VOUT
            self.timing = 0
            bytes2send = bytes([self.id1, self.id2, self.id3, self.id4, id5, id6, id7, self.command])
            return self.communicationUDP(bytes2send)
        else:
            self.str2log("is disconnected, please provide a correct MAC or IP address", self.log_ihp, 3)
            return False
    
    # Read Current from device
    def readIout(self, device):
        if self.connected:
            self.str2log("Request current to Module {}".format(device), self.log_ihp)
            self.RandomValues()
            id5 = self.REQUEST
            if(device>=1 and device<=8): id6 = self.Module1 + device - 1
            else:
                self.str2log("readIout() parameter device is wrong, provide a correct value", self.log_ihp, 3)
                return False
            id7 = self.READ + 0 # Read Mode without aditional bytes
            self.command = self.READ_IOUT
            self.timing = 0
            bytes2send = bytes([self.id1, self.id2, self.id3, self.id4, id5, id6, id7, self.command])
            return self.communicationUDP(bytes2send)
        else:
            self.str2log("is disconnected, please provide a correct MAC or IP address", self.log_ihp, 3)
            return False

    # Read Input Voltage from line
    def readVin(self, line):
        if self.connected:
            self.str2log("Request input voltage to line {}".format(line), self.log_ihp)
            self.RandomValues()
            id5 = self.REQUEST
            id6 = self.COMMS
            id7 = self.READ + 0 # Read Mode without aditional bytes
            if(line>=1 and line<=3): self.command = self.READ_VIN + line - 1
            else:
                self.str2log("readVin() parameter line is wrong, provide a correct value", self.log_ihp, 3)
                return False
            self.timing = 0
            bytes2send = bytes([self.id1, self.id2, self.id3, self.id4, id5, id6, id7, self.command])
            return self.communicationUDP(bytes2send)
        else:
            self.str2log("is disconnected, please provide a correct MAC or IP address", self.log_ihp, 3)
            return False
    
    # Read Input Current from line
    def readIin(self, line):
        if self.connected:
            self.str2log("Request input current to line {}".format(line), self.log_ihp)
            self.RandomValues()
            id5 = self.REQUEST
            id6 = self.COMMS
            id7 = self.READ + 0 # Read Mode without aditional bytes
            if(line>=1 and line<=3): self.command = self.READ_IIN + line - 1
            else:
                self.str2log("readIin() parameter line is wrong, provide a correct value", self.log_ihp, 3)
                return False
            self.timing = 0
            bytes2send = bytes([self.id1, self.id2, self.id3, self.id4, id5, id6, id7, self.command])
            return self.communicationUDP(bytes2send)
        else:
            self.str2log("is disconnected, please provide a correct MAC or IP address", self.log_ihp, 3)
            return False
    
    # Clear Faults works on (PFC1, PFC2, ISOCOMM, any MODULE and GROUP)
    def clearFaults(self, device):
        if self.connected:
            self.str2log("Clear faults to Device {}".format(device), self.log_ihp)
            self.RandomValues()
            id5 = self.REQUEST
            id6 = device # Condition to avoid wrong device addresses is missing here
            id7 = self.WRITE + 0 # Write Mode without aditional bytes
            self.command = self.CLEAR_FAULTS
            self.timing = 0
            bytes2send = bytes([self.id1, self.id2, self.id3, self.id4, id5, id6, id7, self.command])
            return self.communicationUDP(bytes2send)
        else: 
            self.str2log("is disconnected, please provide a correct MAC or IP address", self.log_ihp, 3)
            return False
    
    # Compare first 4 bytes with the first ones that we sent
    def check4FBytes(self, bytes):
        id1 = int(bytes[0], 16)
        id2 = int(bytes[1], 16)
        id3 = int(bytes[2], 16)
        id4 = int(bytes[3], 16)
        if (self.id1==id1 and self.id2==id2 and self.id3==id3 and self.id4==id4):
            self.str2log("The first 4 bytes are correct {} {} {} {}".format(id1, id2, id3, id4), self.log_ihp)
            return True
        else:
            self.str2log("First 4 bytes ERROR Recieve: {} {} {} {} Sent: {} {} {} {}".format(id1, id2, id3, id4, self.id1, self.id2, self.id3, self.id4), self.log_ihp, 3)
            return False

    # Analyze Byte 5. In this byte we could know if there is a error message
    def check5Byte(self, bytes):
        byte = int(bytes[4], 16)
        listBit = splitIntoBits(byte)
        #self.str2log("List Byte5 {}".format(listBit), self.log)
        joinBits = [listBit[4], listBit[3], listBit[2], listBit[1]]
        auxBits = "".join(map(str, joinBits))
        resp = int(auxBits, 2)

        if (listBit[0]==1): self.str2log("Byte 5 Bit[0]: Command response is included with this message", self.log_ihp)
        elif(listBit[0]==0): self.str2log("Byte 5 Bit[0]: Message is an acknowledgement receipt of a BLANK message", self.log_ihp)
        
        if(resp!=0 or listBit[5]!=1 or listBit[7]!=0): self.str2log("Byte 5 ERROR: Check Bits 1,2,3,4,5 and 7 with Artesyn documentation values are wrong", self.log_ihp, 3)

        if(listBit[6]==0): return True
        elif(listBit[6]==1):
            self.str2log("Byte 5 bit[6]: Error code sent in next byte", self.log_ihp, 3)
            return False

    # Analyze Byte 6. Check error messages
    def check6Byte(self, bytes):
        byte = int(bytes[5], 16)
        listBit = splitIntoBits(byte)
        #self.str2log("List Bit, Byte6 {}".format(listBit), self.log)
        joinBits = [listBit[3], listBit[2], listBit[1], listBit[0]]
        auxBits = "".join(map(str, joinBits))
        resp = int(auxBits, 2)

        if (resp!=0): self.str2log("Byte 6 ERROR: Check Bits 0,1,2,3 with Artesyn documentation values should be zero", self.log_ihp)

        joinBits = [listBit[7], listBit[6], listBit[5], listBit[4]]
        auxBits = "".join(map(str, joinBits))
        resp = int(auxBits, 2)

        if(resp==self.ERR_SUCCESS): return True
        else:
            if(resp==self.ERR_RACK_NOT_EXISTING): self.str2log("No iHP Rack available on the specified address", self.log_ihp, 3)
            elif(resp==self.ERR_DEVICE_NOT_EXISTING): self.str2log("Invalid / No devices existing on the specified device address", self.log_ihp, 3)
            elif(resp==self.ERR_UNSUPPORTED_CMD): self.str2log("Unsupported command code", self.log_ihp, 3)
            elif(resp==self.ERR_OPERATION_INVALID): self.str2log("Operation not supported – that command is not valid for reading/writing (depending on what operation was issued)", self.log_ihp, 3)
            elif(resp==self.ERR_LENGTH_INVALID): self.str2log("The length given is invalid for the command code", self.log_ihp, 3)
            elif(resp==self.ERR_DATA_INVALID): self.str2log("The data provided doesn’t match what was expected", self.log_ihp, 3)
            elif(resp==self.ERR_WRITE_PROTECT): self.str2log("The command is valid but the data is write protected", self.log_ihp, 3)
            elif(resp==self.ERR_PROTOCOL_INVALID): self.str2log("There was an error parsing the command", self.log_ihp, 3)
            else: self.str2log("Error Command {} reserved for future use. Please ask Artesyn for further information".format(resp), self.log_ihp, 3)
            return False

    # Analyze Byte 7. Check the internal device address
    def check7Byte(self, bytes):
        byte = int(bytes[6], 16)
        listBit = splitIntoBits(byte)
        #self.str2log("List Bit, Byte7 {}".format(listBit), self.log)
        joinBits = [listBit[4], listBit[3], listBit[2], listBit[1], listBit[0]]
        auxBits = "".join(map(str, joinBits))
        resp = int(auxBits, 2)

        if(resp==self.COMMS): self.str2log("Internal Device Address (ISOCOMM)", self.log_ihp)
        elif(resp==self.PFC1): self.str2log("Internal Device Address (PFC1)", self.log_ihp)
        elif(resp==self.PFC2): self.str2log("Internal Device Address (PFC2)", self.log_ihp)
        elif(resp==self.Module1): self.str2log("Internal Device Address (Module 1)", self.log_ihp)
        elif(resp==self.Module2): self.str2log("Internal Device Address (Module 2)", self.log_ihp)
        elif(resp==self.Module3): self.str2log("Internal Device Address (Module 3)", self.log_ihp)
        elif(resp==self.Module4): self.str2log("Internal Device Address (Module 4)", self.log_ihp)
        elif(resp==self.Module5): self.str2log("Internal Device Address (Module 5)", self.log_ihp)
        elif(resp==self.Module6): self.str2log("Internal Device Address (Module 6)", self.log_ihp)
        elif(resp==self.Module7): self.str2log("Internal Device Address (Module 7)", self.log_ihp)
        elif(resp==self.Module8): self.str2log("Internal Device Address (Module 8)", self.log_ihp)
        elif(resp==self.Group1): self.str2log("Internal Device Address (Group 1)", self.log_ihp)
        elif(resp==self.Group2): self.str2log("Internal Device Address (Group 2)", self.log_ihp)
        elif(resp==self.Group3): self.str2log("Internal Device Address (Group 3)", self.log_ihp)
        elif(resp==self.Group4): self.str2log("Internal Device Address (Group 4)", self.log_ihp)
        elif(resp==self.Group5): self.str2log("Internal Device Address (Group 5)", self.log_ihp)
        elif(resp==self.Group6): self.str2log("Internal Device Address (Group 6)", self.log_ihp)
        elif(resp==self.Group7): self.str2log("Internal Device Address (Group 7)", self.log_ihp)
        else: self.str2log("Internal Device Address (Not recognized)", self.log_ihp, 3)

    # Analyze Byte 8. Number of bytes that are included in the response
    def check8Byte(self, bytes):
        byte = int(bytes[7], 16)
        listBit = splitIntoBits(byte)
        #self.str2log("List Bit, Byte8 {}".format(listBit), self.log)
        joinBits = [listBit[5], listBit[4], listBit[3], listBit[2], listBit[1],listBit[0]]
        auxBits = "".join(map(str, joinBits))
        outputLen = int(auxBits, 2)
        self.str2log("Byte 8 bit[0-5]: length of the data included in this message starting from 10th Byte= {}".format(outputLen), self.log_ihp)

        if(listBit[6]!=0): self.str2log("Byte 8 bit[6]: Value should be 0", self.log_ihp, 3)
        if(listBit[7]!=0): self.str2log("Byte 8 bit[7]: Value should be 0", self.log_ihp, 3)
        return outputLen

    # Analyze Byte 9. Check the command code
    def check9Byte(self, bytes):
        byte = int(bytes[8], 16)
        if (byte==self.command): 
            self.str2log("Byte 9: Command code match with the command sent", self.log_ihp)
            return True
        else: 
            self.str2log("Byte 9: Command code does not match with the command sent. Sent: {} Recieved: {}".format(self.command, byte), self.log_ihp, 3)
            return False

    # Analyze Byte 10 to N
    def decodeResponse(self, bytes, outputLen):
        if(outputLen>0):
            response = []
            for i in range(outputLen): response.append(bytes[i+9])
            response.reverse()
            if (self.command == self.OPERATION):
                byte = int(response[0], 16)
                listBit = splitIntoBits(byte)
                if(listBit[7]==1): self.str2log("Module On", self.log_ihp)
                else: self.str2log("Module Off", self.log_ihp)
            elif(self.command == self.READ_VOUT):
                number = listBytestoDecimal(response)
                multiplier = 10000
                self.str2log("Voltage = {} V".format(number/multiplier), self.log_ihp)
            elif(self.command == self.READ_IOUT):
                number = listBytestoDecimal(response)
                multiplier = 10000
                self.str2log("Current = {} A".format(number/multiplier), self.log_ihp)
            elif(self.command>=self.READ_VIN and self.command<=self.READ_IIN+2):
                number = listBytestoDecimal(response)
                if(self.command>=self.READ_VIN and self.command<self.READ_IIN):
                    multiplier = 10
                    self.str2log("Voltage Line {}= {} V".format(self.command - self.READ_VIN + 1, number/multiplier), self.log_ihp)
                else:
                    multiplier = 100
                    self.str2log("Current Line {}= {} A".format(self.command - self.READ_IIN + 1, number/multiplier), self.log_ihp)
            # WRITE_PROTECT does not return byte according to Artesyn docs
            elif(self.command == self.WRITE_PROTECT): self.str2log(''.join(response), self.log_ihp, 2)
            # VREF does not return byte according to Artesyn docs
            elif(self.command == self.VREF):  self.str2log(''.join(response), self.log_ihp, 2)
            # IREF does not return byte according to Artesyn docs
            elif(self.command == self.IREF):  self.str2log(''.join(response), self.log_ihp, 2)
            # SAVE does not return byteaccoding to Artesyn docs
            elif(self.command == self.SAVE): self.str2log(''.join(response), self.log_ihp, 2)
            # MODULE_CONFIG write only on ISOCOMM when use it in another device it will be read only
            elif(self.command == self.MODULE_CONFIG): self.str2log(''.join(response), self.log_ihp, 2)
            else: self.str2log("Command ({}) does not list. Please see Artesyn documentation for further information".format(self.command), self.log_ihp, 3)

    # Main Communication Function
    def communicationUDP(self, bytes2send):
        bufferSize = 1024
        
        # Send to server using created UDP socket
        self.ClientSocket.sendto(bytes2send, self.serverAddressPort)
        #self.str2log("Sent: {} Len: {}".format(bytes2send, len(bytes2send)), self.log)
        
        try:
            msgFromServer = self.ClientSocket.recvfrom(bufferSize)
            msg = "Message from iHP {}".format(msgFromServer[0])
            self.str2log(msg, self.log_ihp)
            data = msgFromServer[0]
            bytes = splitBytes2Hex(data)

            checkId = self.check4FBytes(bytes)                      # Check the first 4 bytes
            checkError = self.check5Byte(bytes)                     # Check if there is an error
            checkErrorCode = self.check6Byte(bytes)                 # Print the error code. Return true if there is not error
            
            if(checkId and checkError and checkErrorCode):
                self.check7Byte(bytes)                              # Print device address
                outputLen = self.check8Byte(bytes)                  # Get the len of the output bytes
                command = self.check9Byte(bytes)                    # Return true if command is the same that was sent
                if(command):
                    self.decodeResponse(bytes, outputLen)  # Decode the response
                    self.actualTime = time()
                    self.waitRequest = True
                    return True

        # fail after self.TIMEOUT seconds of no activity
        except socket.timeout: self.str2log("Didn't receive data! [Timeout]", self.log, 4)
        # Untracked error
        except Exception as e: self.str2log("Error Unknown: {}".format(e), self.log ,3)

        return False

    def request(self, command, data):
        try:
            if(command == self.OPERATION):
                dev, en = data['device'], data['enable']
                if(dev>=1 and dev<=8):
                    commandNumber = 4
                    command1 = self.WRITE_PROTECT
                    data1 = {'device': self.COMMS, 'enable': True}
                    command2 = self.OPERATION
                    data2 = {'device': dev, 'enable': en}
                    command3 = self.WRITE_PROTECT
                    data3 = {'device': self.COMMS, 'enable': False}
                    command4 = self.SAVE
                    data4 = {'device': dev}
                    self.commandList.append({'commandNumber': commandNumber, 
                                            'request': 'Operation',
                                            0: {'command': command1, 'data': data1}, 
                                            1: {'command': command2, 'data': data2}, 
                                            2: {'command': command3, 'data': data3}, 
                                            3: {'command': command4, 'data': data4}})
                else: self.str2log("Operation() device number should be between 1 and 8", self.log_ihp, 3)

            elif(command == self.VREF):
                dev, type, vref = data['device'], data['type'], data['vref']
                if(dev>=1 and dev<=8):
                    commandNumber = 2
                    command1 = self.WRITE_PROTECT
                    data1 = {'device': dev, 'enable': True}
                    command2 = self.VREF
                    data2 = {'device': dev, 'type': type, 'vref': vref}
                    self.commandList.append({'commandNumber': commandNumber, 
                                            'request': 'Vref',
                                            0: {'command': command1, 'data': data1}, 
                                            1: {'command': command2, 'data': data2}})
                else: self.str2log("Vref() device number should be between 1 and 8", self.log_ihp, 3)

            elif(command == self.IREF):
                dev, type, iref = data['device'], data['type'], data['iref']
                if(dev>=1 and dev<=8):
                    commandNumber = 2
                    command1 = self.WRITE_PROTECT
                    data1 = {'device': dev, 'enable': True}
                    command2 = self.IREF
                    data2 = {'device': dev, 'type': type, 'iref': iref}
                    self.commandList.append({'commandNumber': commandNumber, 
                                            'request': 'Iref',
                                            0: {'command': command1, 'data': data1}, 
                                            1: {'command': command2, 'data': data2}})
                else: self.str2log("Iref() device number should be between 1 and 8", self.log_ihp, 3)

            elif(command == self.MODULE_CONFIG):
                dev, type = data['device'], data['type']
                if(dev>=1 and dev<=8):
                    commandNumber = 4
                    command1 = self.WRITE_PROTECT
                    data1 = {'device': self.COMMS, 'enable': True}
                    command2 = self.MODULE_CONFIG
                    data2 = {'device': dev, 'type': type}
                    command3 = self.WRITE_PROTECT
                    data3 = {'device': self.COMMS, 'enable': False}
                    command4 = self.SAVE
                    data4 = {'device': dev}
                    self.commandList.append({'commandNumber': commandNumber, 
                                            'request': 'Module Config',
                                            0: {'command': command1, 'data': data1}, 
                                            1: {'command': command2, 'data': data2}, 
                                            2: {'command': command3, 'data': data3}, 
                                            3: {'command': command4, 'data': data4}})
                else: self.str2log("ModuleConfig() device number should be between 1 and 8", self.log_ihp, 3)

            elif(command == self.READ_VOUT):
                dev = data['device']
                if(dev>=1 and dev<=8):
                    commandNumber = 1
                    command1 = self.READ_VOUT
                    data1 = {'device': dev}
                    self.commandList.append({'commandNumber': commandNumber, 
                                            'request': 'Read Vout',
                                            0: {'command': command1, 'data': data1}})
                else: self.str2log("ReadVout() device number should be between 1 and 8", self.log_ihp, 3)

            elif(command == self.READ_IOUT):
                dev = data['device']
                if(dev>=1 and dev<=8):
                    commandNumber = 1
                    command1 = self.READ_IOUT
                    data1 = {'device': dev}
                    self.commandList.append({'commandNumber': commandNumber, 
                                            'request': 'Read Iout',
                                            0: {'command': command1, 'data': data1}})
                else: self.str2log("ReadIout() device number should be between 1 and 8", self.log_ihp, 3)
            
            elif(command == self.READ_VIN):
                line = data['line']
                if(line>=1 and line<=3):
                    commandNumber = 1
                    command1 = self.READ_VIN + line - 1
                    self.commandList.append({'commandNumber': commandNumber, 
                                            'request': 'Read Vin',
                                            0: {'command': command1, 'data': {} }})
                else: self.str2log("ReadVin() device number should be between 1 and 3", self.log_ihp, 3)
            
            elif(command == self.READ_IIN):
                line = data['line']
                if(line>=1 and line<=3):
                    commandNumber = 1
                    command1 = self.READ_IIN + line - 1
                    self.commandList.append({'commandNumber': commandNumber, 
                                            'request': 'Read Iin',
                                            0: {'command': command1, 'data': {} }})
                else: self.str2log("ReadIin() device number should be between 1 and 3", self.log_ihp, 3)

            elif(command == self.CLEAR_FAULTS):
                commandNumber = 1
                command1 = self.CLEAR_FAULTS
                data1 = {'device': data['device']}
                self.commandList.append({'commandNumber': commandNumber, 
                                        'request': 'Clear Faults',
                                        0: {'command': command1, 'data': data1}})

        except Exception as e: self.str2log("Critical Error: {}".format(e), self.log, 4)

    def run(self):
        # Pass to the next command in the list only if the waiting time has passed
        if(time() - self.actualTime > self.timing and self.waitRequest): self.commandSucceed()
        
        # Run the next command in the list
        if(self.running and len(self.commandList)>0 and not self.waitRequest):
            cList = self.commandList[0] # Get the first array of commands
            if(self.actualCommand<cList['commandNumber']):
                c = cList[self.actualCommand]['command']
                data = cList[self.actualCommand]['data']
                if(c == self.WRITE_PROTECT):
                    if(not self.WriteProtect(data['device'], data['enable'])): self.errorWithCommand("We have problems with command WRITE_PROTECT")
                elif(c == self.OPERATION):
                    if(not self.operation(data['device'], data['enable'])): self.errorWithCommand("We have problems with command OPERATION")
                elif(c == self.VREF):
                    if(data['type']=='normal'):
                        if(not self.setVoltage(data['device'], data['vref'])): self.errorWithCommand("We have problems with command VREF")
                    elif(data['type']=='percentage'):
                        if(not self.setVoltagePercentage(data['device'], data['vref'])): self.errorWithCommand("We have problems with command VREF")
                    else: self.errorWithCommand("Vref() type should be 'normal' or 'percentage'")
                elif(c == self.IREF):
                    if(data['type']=='normal'):
                        if(not self.setCurrent(data['device'], data['iref'])): self.errorWithCommand("We have problems with command IREF")
                    elif(data['type']=='percentage'):
                        if(not self.setCurrentPercentage(data['device'], data['iref'])): self.errorWithCommand("We have problems with command IREF")
                    else: self.errorWithCommand("Iref() type should be 'normal' or 'percentage'")
                elif(c == self.MODULE_CONFIG):
                    if(data['type']=='DCS'):
                        if(not self.DCS(data['device'])): self.errorWithCommand("We have problems with command MODULE_CONFIG")
                    else: self.errorWithCommand("ModuleConfig() types supported currently: 'DCS'")
                elif(c == self.SAVE):
                    if(not self.save(data['device'])): self.errorWithCommand("We have problems with command SAVE")
                elif(c == self.READ_VOUT):
                    if(not self.readVout(data['device'])): self.errorWithCommand("We have problems with command READ_VOUT")
                elif(c == self.READ_IOUT):
                    if(not self.readIout(data['device'])): self.errorWithCommand("We have problems with command READ_IOUT")
                elif(c>=self.READ_VIN and c<=self.READ_VIN+2):
                    if(not self.readVin(c-self.READ_VIN+1)): self.errorWithCommand("We have problems with command READ_VIN")
                elif(c>=self.READ_IIN and c<=self.READ_IIN+2):
                    if(not self.readIin(c-self.READ_IIN+1)): self.errorWithCommand("We have problems with command READ_IIN")
                elif(c == self.CLEAR_FAULTS):
                    if(not self.clearFaults(data['device'])): self.errorWithCommand("We have problems with command CLEAR_FAULTS")
                else: self.errorWithCommand("Command {} not supported".format(c))
            else:
                self.commandList = self.commandList[1:] # Delete the first request because it is finished
                self.actualCommand = 0
                self.str2log("Request {} solved".format(cList['request']), self.log_ihp)
        
        # If there are new command, start the thread
        elif(not self.running and len(self.commandList)>0): 
            self.running = True
            self.str2log("There is(are) request(s). Communication is running", self.log)
        
        # If there is no more commands to run, stop the thread
        elif(self.running and len(self.commandList)==0):
            self.running = False
            self.str2log("All request are solved. Communication is stopped", self.log)

# Debug
def main():
    from time import sleep
    data = {'MAC': 'D0:03:EB:A2:DD:14', 'ip_range': '192.168.6.1/23', 'port': 8888}
    
    # Look the MAC address, range and PORT
    ihp = IHP(data)

    #for i in range(2,3,1): ihp.request(ihp.MODULE_CONFIG, {'device': i, 'type': 'DCS'})

    # Functions tested
    ihp.request(ihp.CLEAR_FAULTS, {'device': ihp.COMMS})
    ihp.request(ihp.CLEAR_FAULTS, {'device': ihp.PFC1})
    ihp.request(ihp.CLEAR_FAULTS, {'device': ihp.PFC2})
    """
    for i in range(1,9,1): ihp.request(ihp.IREF, {'device': i, 'type': 'percentage', 'iref': 0})
    
    for i in range(1,4,1):
        ihp.request(ihp.READ_VIN, {'line': i})
        ihp.request(ihp.READ_IIN, {'line': i})
    
    while True:
        ihp.run()
    """
if __name__ == '__main__':
    main()
