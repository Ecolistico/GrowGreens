import socket
import subprocess
import xml.etree.ElementTree as ET
sys.path.insert(0, './src/')
import os
import random

class IHP:
    def RandomValues(self):
        #Returns 4 Random bytes
        self.a = hex(random.randrange(0,255))
        self.b = hex(random.randrange(0,255))
        self.c = hex(random.randrange(0,255))
        self.d = hex(random.randrange(0,255))
        self.a = self.a[2:]
        self.b = self.b[2:]
        self.c = self.c[2:]
        self.d = self.d[2:]
        if len(self.a)<2:
            self.a = "0" + self.a
        if len(self.b)<2:
            self.b = "0" + self.b
        if len(self.c)<2:
            self.c = "0" + self.c
        if len(self.d)<2:
            self.d = "0" + self.d

    def __init__(self, MAC, msgToSend, ip_range, IHPPort):
        self.MAC = MAC
        self.msgToSend = msgToSend
        self.ip_range = ip_range
        self.port = IHPPort
        #iHP Communication Status
        self.iHpIP = ''


    def ScanIP(self, MAC, ip_range):
        xml = scan_for_hosts(ip_range)
        ip_address = find_ip_address_for_mac_address(xml, MAC)
        if ip_address:
            print('Found IP address {} for MAC address {} in IP address range {}.'
                  .format(ip_address, mac_address, ip_range))
            self.IpToMacconnected = true
            self.iHpIP = ip_address
        else:
            print('No IP address found for MAC address {} in IP address range {}.'
                  .format(MAC, ip_range))


    def WriteProtect(self, EDWrite):
        #Enable/Disable Write #1
        self.RandomValues()
        self.EDWrite = EDWrite
        e = b'\xA1'
        f = b'\x00'
        g = b'\x01'
        h = b'\x10'
        if(EDWrite == "ENABLE"):
            i = b'\x00'
        elif(EDWrite == "DISABLE"):
            i = b'\x80'
        z = [self.a,self.b,self.c,self.d,e,f,g,h,i]
        return z

    def OperationMode(self, ModuleN):
    #Change Module n to Digital Current Source #2
        self.RandomValues()
        self.ModuleN = ModuleN
        e = b'\xA1'
        f = b'\x00'
        g = b'\x02'
        h = b'\xD3'
        if(ModuleN == "Module1"):
            i = b'\x00'
        elif(ModuleN == "Module2"):
            i = b'\x01'
        elif(ModuleN == "Module3"):
            i = b'\x02'
        elif(ModuleN == "Module4"):
            i = b'\x03'
        elif(ModuleN == "Module5"):
            i = b'\x04'
        elif(ModuleN == "Module6"):
            i = b'\x05'
        elif(ModuleN == "Module7"):
            i = b'\x06'
        elif(ModuleN == "Module8"):
            i = b'\x07'
        j = b'\x08'
        z = [self.a,self.b,self.c,self.d,e,f,g,h,i,j]
        return z

    def Vref(self, ModuleN, Voltage):
       #Change Module N Default Voltage #3
        self.RandomValues()
        self.ModuleN = ModuleN
        self.Voltage = Voltage
        e = b'\xA1'
        if(ModuleN == "Module1"):
           f = b'\x10'
        elif(ModuleN == "Module2"):
           f = b'\x11'
        elif(ModuleN == "Module3"):
           f = b'\x12'
        elif(ModuleN == "Module4"):
           f = b'\x13'
        elif(ModuleN == "Module5"):
           f = b'\x14'
        elif(ModuleN == "Module6"):
           f = b'\x15'
        elif(ModuleN == "Module7"):
           f = b'\x16'
        elif(ModuleN == "Module8"):
           f = b'\x17'
        g = b'\x04'
        h = b'\xB1'
        i = b'\x03'
        mult = Voltage * 10000
        fmt = "<{}".format("L")
        bytes = struct.pack(fmt, mult)
        F1 = "".join("{:02x}".format(byte) for byte in bytes)
        j1 = F1[:2]
        j2 = F1[2:4]
        j3 = F1[-4:-2]
        z = [self.a,self.b,self.c,self.d,e,f,g,h,i,j1,j2,j3]
        return z

    def Iref(self, ModuleN, Current):
       #Change Module N Default Current #4
        self.RandomValues()
        self.ModuleN = ModuleN
        self.Current = Current
        e = b'\xA1'
        if(ModuleN == "Module1"):
            f = b'\x10'
        elif(ModuleN == "Module2"):
            f = b'\x11'
        elif(ModuleN == "Module3"):
            f = b'\x12'
        elif(ModuleN == "Module4"):
            f = b'\x13'
        elif(ModuleN == "Module5"):
            f = b'\x14'
        elif(ModuleN == "Module6"):
            f = b'\x15'
        elif(ModuleN == "Module7"):
            f = b'\x16'
        elif(ModuleN == "Module8"):
            f = b'\x17'
        g = b'\x04'
        h = b'\xB2'
        i = b'\x03'
        mult = Current * 10000
        fmt = "<{}".format("L")
        bytes = struct.pack(fmt, mult)
        F1 = "".join("{:02x}".format(byte) for byte in bytes)
        j1 = F1[:2]
        j2 = F1[2:4]
        j3 = F1[-4:-2]
        z = [self.a,self.b,self.c,self.d,e,f,g,h,i,j1,j2,j3]
        return z

    def Operation(self, ModuleN, OnOff):
    #Module n Turn On/Off #5
        self.RandomValues()
        self.ModuleN = ModuleN
        self.OnOff = OnOff
        e = b'\xA1'
        if(ModuleN == "Module1"):
            f = b'\x00'
        elif(ModuleN == "Module2"):
            f = b'\x11'
        elif(ModuleN == "Module3"):
            f = b'\x12'
        elif(ModuleN == "Module4"):
            f = b'\x13'
        elif(ModuleN == "Module5"):
            f = b'\x14'
        elif(ModuleN == "Module6"):
            f = b'\x15'
        elif(ModuleN == "Module7"):
            f = b'\x16'
        elif(ModuleN == "Module8"):
            f = b'\x17'
        g = b'\x01'
        h = b'\x01'
        if(OnOff == "On"):
            i = b'\x80'
        elif(OnOff == "Off"):
            i = b'\x00'
        z = [self.a,self.b,self.c,self.d,e,f,g,h,i]
        return z


    def splitBytes(self, data):
        self.data = data #Split the ByteArray by the number of bytes of the frame,it returns a list
        info = [data[i:i+1] for i in range(0, len(data))]
        lentrama = len(data)
        print("Lentrama", lentrama)
        return info

    def splitByte(self, info, PosByte):
        #Returns a byte in a specific position, returns it in decimal format.
        self.info = info #list
        self.PosByte = PosByte #List position
        info1 = ord(info[PosByte])
        info2 = str(info1)
        val1 = bin(int(info2, 10))
        print(val1)
        return info1

    def splitIntoBits(self, BytePos):
        #Split byte into its bits, return a list of binary
        selg.BytePos = info1
        val3 = [info1 >> i & 1 for i in range(8)]
        val3.reverse()
        print(val3)
        return val3

    def Comp4FBytes(self, info):
        #Compara los 4 primeros Bytes con los que le mandamos
        self.info = tm
        b1 = tm[0]
        b2 = tm[1]
        b3 = tm[2]
        b4 = tm[3]
        if (self.a==b1 and self.b==b2 and self.c==b3 and self.d==b4):
            print("The first 4 bytes are correct", b1, b2, b3, b4, "OK")
        else:
            print("First 4 bytes ERROR")

    def Comp5Byte(self, info):
        #Analyze Byte5
        self.info = tm
        Bytepos = self.splitByte(tm, 5)
        listBit = self.splitIntoBits(BytePos)
        if (listBit[0]==1):
            print("BYTE5: Bit[0] = 1: Command response is included with this message")
        elif(listBit[0]==0):
            print("BYTE5: Bit[0]: ERROR ‘0’ Message is an acknowledgement receipt of a BLANK message.")
        valf1 = [listBit[1], listBit[2], listBit[3], listBit[4]]
        ba = "".join(map(str, valf1))
        numero1 = int(ba, 2)
        if (numero1==0):
            print("BYTE5: Reserved for future use. Values is 0, OK")
        elif(numero1!=0):
            print("BYTE5: Reserved for future use. Values should be zero. NOT OK")
        if(listBit[5]==1):
            print("BYTE5:Bit5: Final Bit Reserved for future use. Value should be one. OK")
        elif(listBit[5]!=1):
            print("BYTE5:Bit5: Final Bit Reserved for future use. Value should be one. NOTOK")
        if(listBit[6]==0):
            print("BYTE5:Bit6: No error,OK")
        elif(listBit[6]==1):
            print("BYTE5:Bit6: BIT ERROR, NOT OK")
        if(listBit[7]==0):
            print("BYTE5:Bit7: Reserved for future use. Value should be zero. OK")
        elif(listBit[7]!=0):
            print("BYTE5:Bit7: Reserved for future use. Value should be zero. NOT OK")

    def Comp6Byte(self, info):
        #Analyze Byte6
        self.info = tm
        Bytepos = self.splitByte(tm, 6)
        listBit = self.splitIntoBits(BytePos)
        valf1 = [listBit[0], listBit[1], listBit[2], listBit[3]]
        ba = "".join(map(str, valf1))
        numero1 = int(ba, 2)
        print("BYTE6:Bit0-3: Command that is being responded to. should be 0:", numero1)

        valf2 = [listBit[4], listBit[5], listBit[6], listBit[7]]
        ca = "".join(map(str, valf2))
        numero2 = int(ca, 2)
        print("BYTE6:Bit4-7: A non-zero value indicates an error code, zero is reserved for normal responses:", numero2)
        #AGREGAR TABLA DE ERRORES

    def Comp7Byte(self, info):
        #Analyze Byte7
        self.info = tm
        Bytepos = self.splitByte(tm, 7)
        listBit = self.splitIntoBits(BytePos)
        valf1 = [listBit[0], listBit[1], listBit[2], listBit[3], listBit[4]]
        ba = "".join(map(str, valf1))
        numero1 = int(ba, 2)
        print("BYTE7:Internal Device Address:", numero1)
        #AGREGAR TABLA DE DIRECCIONES

    def Comp8Byte(self, info):
        #Analyze Byte8
        self.info = tm
        Bytepos = self.splitByte(tm, 8)
        listBit = self.splitIntoBits(BytePos)
        valf1 = [listBit[0], listBit[1], listBit[2], listBit[3], listBit[4],listBit[5]]
        ba = "".join(map(str, valf1))
        numero1 = int(ba, 2)
        print("BYTE8:Bit0-5:length of the data included in this message starting from 10th Byte.:",numero1)
        #A implementar lecturas
        print("BYTE8:Bit6: Value should be 0:",listBit[6])
        print("BYTE8:Bit7: Value should be 0:",listBit[6])

    def Comp9Byte(self, info):
        self.info = tm
        Bytepos = self.splitByte(tm, 9)
        listBit = self.splitIntoBits(BytePos)
        ba = "".join(map(str, listBit))
        numero1 = int(ba, 2)
        print("Command code for Module, ISOCOMM, or PFC.:",numero1)

######   ////marcador para unificar
    def CommunicationIHP(self, NumberF, MAC, ip_range, IHPPort):
        self.NumberF = NumberF #Number function
        self.MAC = MAC
        self.ip_range = ip_range
        self.port = IHPPort
        #iHP Communication Status
        self.iHpIP = ''
        self.ScanIP()
        self.WriteProtect()
        self.OperationMode()
        self.Vref()
        self.Iref()
        self.Operation()

        serverAddressPort = (self.iHpIP, IHPPort)
        bufferSize = 1024
        # Create a UDP socket at client side
        UDPClientSocket = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)

        if(NumberF == "1"):
            self.WriteProtect()##Como le cargo los parametro???? POR LO PRONTO SE LO TIENES QUE PASAR A MANO

        elif(NumberF == "2"):
            self.OperationMode()

        elif(NumberF == "3"):
            self.Vref()

        elif(NumberF == "4"):
            self.Iref()

        elif(NumberF == "5"):
            self.Operation()

        #bytesToSend = self.z
        # Send to server using created UDP socket
        UDPClientSocket.settimeout(1) # Sets the socket to timeout after 1 second of no activity
        UDPClientSocket.sendto(self.z, serverAddressPort)
        try:
            msgFromServer = UDPClientSocket.recvfrom(bufferSize)
            msg = "Message from Server {}".format(msgFromServer[0])
            print(msg)
        except socket.timeout: # fail after 1 second of no activity
            print("Didn't receive data! [Timeout]")
        data = msgFromServer[0]
        info = self.splitBytes(data)
        lentrama = len(data)
        if (lentrama==5):
            self.Comp4FBytes(info)
            self.Comp5Byte(info)
        if (lentrama==6):
            self.Comp4FBytes(info)
            self.Comp5Byte(info)
            self.Comp6Byte(info)
        if (lentrama==7):
            self.Comp4FBytes(info)
            self.Comp5Byte(info)
            self.Comp6Byte(info)
            self.Comp7Byte(info)
        if (lentrama==8):
            self.Comp4FBytes(info)
            self.Comp5Byte(info)
            self.Comp6Byte(info)
            self.Comp7Byte(info)
            self.Comp8Byte(info)
        if (lentrama==9):
            self.Comp4FBytes(info)
            self.Comp5Byte(info)
            self.Comp6Byte(info)
            self.Comp7Byte(info)
            self.Comp8Byte(info)
            self.Comp9Byte(info)
