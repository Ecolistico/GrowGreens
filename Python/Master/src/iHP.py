import socket
import subprocess
import xml.etree.ElementTree as ET
#sys.path.insert(0, './src/') # Borrar línea, sólo es necesario agregarla cuando vas a importar librerias desde la carpeta src y el archivo que estas trabajando esta fuera de esa carpeta. Ejemplo: growMaster.py requiere esta línea para acceder a todas las librerías programadas en ./src/
import os
import random
from sysMaster import scan_for_hosts, find_ip_address_for_mac_address # Agregar esta línea para poder acceder sólo a las funciones requeridad de sysMaster
import binascii
import struct

class IHP:
    # Pendiente agregar logger a la clase para poder acceder a las funciones del log y sustituir todos los print
    def __init__(self, MAC, ip_range, port):#, logger = None):
        self.MAC = MAC
        self.ip_range = ip_range
        self.port = port
        #self.log = logger
        #self.msgToSend = msgToSend # Quitar msgToSend se crea el objeto para dejar la comunicación permanente no para cada msj
        self.ScanIP() # initialize IP and connected variables
        self.a = ''
        self.b = ''
        self.c = ''
        self.d = ''
    """
    def log(self, level, mssg):
        if (self.log == None): print(mssg)
        else:
            if level == 0: log.debug(mssg)
            elif
    """     
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
        self.a = int(self.a, 16)
        print(self.a)
        self.b = int(self.b, 16)
        print(self.b)
        self.c = int(self.c, 16)
        print(self.c)
        self.d = int(self.d, 16)
        print(self.d)
        
    def ScanIP(self):
        xml = scan_for_hosts(self.ip_range)
        ip_address = find_ip_address_for_mac_address(xml, self.MAC)
        if ip_address:
            self.connected = True
            self.ip = ip_address
            print('Found IP address {} for MAC address {} in IP address range {}.'.format(ip_address, self.MAC, self.ip_range))
        else:
            self.connected = False
            self.ip = ""
            print('No IP address found for MAC address {} in IP address range {}.'.format(self.MAC, self.ip_range))

    def WriteProtect(self, enable):
        #Enable/Disable Write to ISOCOMM #1
        self.RandomValues()
        #self.EDWrite = EDWrite # Creo que no hace falta crear otra variable de clase para guardar este parámetro
        e = 0xA1
        f = 0x00
        g = 0x01
        h = 0x10
        if(enable == True): # Cambié "ENABLE" por True
            i = 0x00
        elif(enable == False): # Cambie "DISABLE" por False
            i = 0x80
        # Es más sensillo trabajar con booleanos que con strings
        self.z = bytes([self.a,self.b,self.c,self.d,e,f,g,h,i])

    def OperationMode(self, ModuleN):
    #Change Module n to Digital Current Source #2
        print("Change Module n to Digital Current Source")
        self.RandomValues()
        #self.ModuleN = ModuleN No hace falta una variable de clase. El parámetro sólo se usa dentro de esta función
        e = 0xA1
        f = 0x00
        g = 0x02
        h = 0xD3
        # Cambio de comparación es más sencillo trabajar con enteros que con strings
        # Nos permite utilizar for para hacer un loop sobre la función sin hacer funciones adicionales
        # para crear los strings que estamos definiendo
        if(ModuleN == 1): i = 0x00
        elif(ModuleN == 2): i = 0x01
        elif(ModuleN == 3): i = 0x02
        elif(ModuleN == 4): i = 0x03
        elif(ModuleN == 5): i = 0x04
        elif(ModuleN == 6): i = 0x05
        elif(ModuleN == 7): i = 0x06
        elif(ModuleN == 8): i = 0x07
        # else: Pendiente manejar el error de la función ¿qué pasa si doy como parámetro 0,9,10?
        # Nos tiene que devolver la alerta de que el parámetro es incorrecto
        j = 0x08
        self.z = bytes([self.a,self.b,self.c,self.d,e,f,g,h,i,j])

    def Vref(self, ModuleN, Voltage):
        #Change Module N Default Voltage #3
        self.RandomValues()
        #self.ModuleN = ModuleN No hace falta una variable de clase. El parámetro sólo se usa dentro de esta función
        #self.Voltage = Voltage No hace falta una variable de clase. El parámetro sólo se usa dentro de esta función
        e = 0xA1
        # Cambio de comparación es más sencillo trabajar con enteros que con strings
        # Nos permite utilizar for para hacer un loop sobre la función sin hacer funciones adicionales
        # para crear los strings que estamos definiendo
        if(ModuleN == 1): f = 0x10
        elif(ModuleN == 2): f = 0x11
        elif(ModuleN == 3): f = 0x12
        elif(ModuleN == 4): f = 0x13
        elif(ModuleN == 5): f = 0x14
        elif(ModuleN == 6): f = 0x15
        elif(ModuleN == 7): f = 0x16
        elif(ModuleN == 8): f = 0x17
        # else: Pendiente manejar el error de la función ¿qué pasa si doy como parámetro 0,9,10?
        # Nos tiene que devolver la alerta de que el parámetro es incorrecto
        g = 0x04
        h = 0xB1
        i = 0x03
        # Pendiente agregar límites de voltaje para no mandar valores fuera de rango
        # Mejor aún configurar el voltaje como función porcentual 0-100%
        mult = Voltage * 10000
        fmt = "<{}".format("L")
        bytes = struct.pack(fmt, mult)
        F1 = "".join("{:02x}".format(byte) for byte in bytes)
        j1 = int(F1[:2], 16)
        j2 = int(F1[2:4], 16)
        j3 = int(F1[-4:-2], 16)
        self.z = bytearray([self.a,self.b,self.c,self.d,e,f,g,h,i,j1,j2,j3])

    def Iref(self, ModuleN, Current):
        #Change Module N Default Current #4
        self.RandomValues()
        #self.ModuleN = ModuleN No hace falta una variable de clase. El parámetro sólo se usa dentro de esta función
        #self.Current = Current No hace falta una variable de clase. El parámetro sólo se usa dentro de esta función
        e = 0xA1
        # Cambio de comparación es más sencillo trabajar con enteros que con strings
        # Nos permite utilizar for para hacer un loop sobre la función sin hacer funciones adicionales
        # para crear los strings que estamos definiendo
        if(ModuleN == 1): f = 0x10
        elif(ModuleN == 2): f = 0x11
        elif(ModuleN == 3): f = 0x12
        elif(ModuleN == 4): f = 0x13
        elif(ModuleN == 5): f = 0x14
        elif(ModuleN == 6): f = 0x15
        elif(ModuleN == 7): f = 0x16
        elif(ModuleN == 8): f = 0x17
        # else: Pendiente manejar el error de la función ¿qué pasa si doy como parámetro 0,9,10?
        # Nos tiene que devolver la alerta de que el parámetro es incorrecto
        g = 0x04
        h = 0xB2
        i = 0x03
        # Pendiente agregar límites de corriente para no mandar valores fuera de rango
        # Mejor aún configurar la corriente como función porcentual 0-100%
        mult = Current * 10000
        fmt = "<{}".format("L")
        bytes = struct.pack(fmt, mult)
        F1 = "".join("{:02x}".format(byte) for byte in bytes)
        j1 = int(F1[:2], 16)
        j2 = int(F1[2:4], 16)
        j3 = int(F1[-4:-2], 16)
        self.z = bytearray([self.a,self.b,self.c,self.d,e,f,g,h,i,j1,j2,j3])
        

    def Operation(self, ModuleN, OnOff):
        #Module n Turn On/Off #5
        self.RandomValues()
        #self.ModuleN = ModuleN No hace falta una variable de clase. El parámetro sólo se usa dentro de esta función
        #self.OnOff = OnOff No hace falta una variable de clase. El parámetro sólo se usa dentro de esta función
        e = 0xA1
        # Cambio de comparación es más sencillo trabajar con enteros que con strings
        # Nos permite utilizar for para hacer un loop sobre la función sin hacer funciones adicionales
        # para crear los strings que estamos definiendo
        if(ModuleN == 1): f = 0x10
        elif(ModuleN == 2): f = 0x11
        elif(ModuleN == 3): f = 0x12
        elif(ModuleN == 4): f = 0x13
        elif(ModuleN == 5): f = 0x14
        elif(ModuleN == 6): f = 0x15
        elif(ModuleN == 7): f = 0x16
        elif(ModuleN == 8): f = 0x17
        # else: Pendiente manejar el error de la función ¿qué pasa si doy como parámetro 0,9,10?
        # Nos tiene que devolver la alerta de que el parámetro es incorrecto
        g = 0x01
        h = 0x01
        # Cambio de comparación es mpas sencillo trabajar con booleanos que con strings
        if(OnOff == True): i = 0x80
        elif(OnOff == False): i = 0x00
        self.z = bytes([self.a,self.b,self.c,self.d,e,f,g,h,i])

    def WriteProtectDisableM(self, ModuleN):
        #To MODULE #6
        self.RandomValues()
        e = 0xA1
        if(ModuleN == 1): f = 0x10
        elif(ModuleN == 2): f = 0x11
        elif(ModuleN == 3): f = 0x12
        elif(ModuleN == 4): f = 0x13
        elif(ModuleN == 5): f = 0x14
        elif(ModuleN == 6): f = 0x15
        elif(ModuleN == 7): f = 0x16
        elif(ModuleN == 8): f = 0x17
        # else: Pendiente manejar el error de la función ¿qué pasa si doy como parámetro 0,9,10?
        # Nos tiene que devolver la alerta de que el parámetro es incorrecto
        g = 0x01
        h = 0x10
        # Cambio de comparación es mpas sencillo trabajar con booleanos que con strings
        i = 0x00
        self.z = bytes([self.a,self.b,self.c,self.d,e,f,g,h,i])

    def ModuleSave(self):
        #Module Save to ISOCOMM #7
        self.RandomValues()
        #self.EDWrite = EDWrite # Creo que no hace falta crear otra variable de clase para guardar este parámetro
        e = 0xA1
        f = 0x00
        g = 0x01
        h = 0xD7
        i = 0x00
        # Es más sensillo trabajar con booleanos que con strings
        self.z = bytes([self.a,self.b,self.c,self.d,e,f,g,h,i])

    def splitBytes(self, data):
        # self.data no se está usando en nungún lado sólo en está función si quieres guardarlo como variable de clase
        # la puedes guardar pero ya no se la pases como parámetro a esta función
        #self.data = data #Split the ByteArray by the number of bytes of the frame,it returns a list
        hex_var = binascii.hexlify(data)
        info = [hex_var[i:i+2] for i in range(0, len(hex_var),2)]
        lentrama = len(data)
        print("Lentrama", lentrama)
        print(info)
        self.info = info # En lugar de retornar el valor solo guardalo en su variable de clase, y accedes a el desde cualquier otra función dentro de la clase

    def splitByte(self, PosByte): # self.info ya se guardo previamente no es necesario pasar el info como parámetro
        #Returns a byte in a specific position, returns it in decimal format.
        #self.info = info #list # Ya se guardo previamente
        self.PosByte = PosByte #List position
        self.info1 = int(self.info[PosByte], 16)
        info2 = str(self.info1)
        val1 = bin(int(info2, 10))
        print(val1)
        return self.info1

    def splitIntoBits(self):
        #Split byte into its bits, return a list of binary
        val3 = [self.info1 >> i & 1 for i in range(8)]
        val3.reverse()
        print(val3)
        return val3

    def Comp4FBytes(self): # No es necesario pasar info como parámetro ya lo guardaste en la clase como self.info
        #Compara los 4 primeros Bytes con los que le mandamos
        #self.info = tm # Esta declaración está tm no ha sido declarada en ningún lugar
        tm = self.info # Creo que lo que querías hacer era esto
        a1 = self.a
        a2 = self.b
        a3 = self.c
        a4 = self.d
        b1 = int(tm[0], 16)
        b2 = int(tm[1], 16)
        b3 = int(tm[2], 16)
        b4 = int(tm[3], 16)
        if (a1==b1 and a2==b2 and a3==b3 and a4==b4): print("The first 4 bytes are correct", b1, b2, b3, b4, "OK")
        else: print("First 4 bytes ERROR",b1, b2, b3, b4,"Sent:", a1, a2, a3, a4)

    def Comp5Byte(self): # No es necesario pasar info como parámetro ya lo guardaste en la clase como self.info
        #Analyze Byte5
        #self.info = tm # Esta declaración está tm no ha sido declarada en ningún lugar
        #tm = self.info # Creo que lo que querías hacer era esto
        Bytepos = self.splitByte(4)
        listBit = self.splitIntoBits()
        print("List Bit, Byte5", listBit)
        if (listBit[0]==1): print("BYTE5: Bit[0] = 1: Command response is included with this message")
        elif(listBit[0]==0): print("BYTE5: Bit[0]: ERROR ‘0’ Message is an acknowledgement receipt of a BLANK message.")
        valf1 = [listBit[1], listBit[2], listBit[3], listBit[4]]
        ba = "".join(map(str, valf1))
        numero1 = int(ba, 2)
        if (numero1==0): print("BYTE5: Reserved for future use. Values is 0, OK")
        elif(numero1!=0): print("BYTE5: Reserved for future use. Values should be zero. NOT OK")
        if(listBit[5]==1): print("BYTE5:Bit5: Final Bit Reserved for future use. Value should be one. OK")
        elif(listBit[5]!=1): print("BYTE5:Bit5: Final Bit Reserved for future use. Value should be one. NOTOK")
        if(listBit[6]==0): print("BYTE5:Bit6: No error,OK")
        elif(listBit[6]==1): print("BYTE5:Bit6: BIT ERROR, NOT OK")
        if(listBit[7]==0): print("BYTE5:Bit7: Reserved for future use. Value should be zero. OK")
        elif(listBit[7]!=0): print("BYTE5:Bit7: Reserved for future use. Value should be zero. NOT OK")

    def Comp6Byte(self): # No es necesario pasar info como parámetro ya lo guardaste en la clase como self.info
        #Analyze Byte6
        #self.info = tm # Esta declaración está tm no ha sido declarada en ningún lugar
        #tm = self.info # Creo que lo que querías hacer era esto
        Bytepos = self.splitByte(5)
        print("byte", Bytepos)
        listBit = self.splitIntoBits()
        print("List Bit, Byte6", listBit)
        valf1 = [listBit[0], listBit[1], listBit[2], listBit[3]]
        ba = "".join(map(str, valf1))
        numero1 = int(ba, 2)
        print("BYTE6:Bit0-3: Command that is being responded to. should be 0:", numero1)

        valf2 = [listBit[4], listBit[5], listBit[6], listBit[7]]
        ca = "".join(map(str, valf2))
        numero2 = int(ca, 2)
        print("BYTE6:Bit4-7: A non-zero value indicates an error code, zero is reserved for normal responses:", numero2)
        #AGREGAR TABLA DE ERRORES

    def Comp7Byte(self): # No es necesario pasar info como parámetro ya lo guardaste en la clase como self.info
        #Analyze Byte7
        #self.info = tm # Esta declaración está tm no ha sido declarada en ningún lugar
        #tm = self.info # Creo que lo que querías hacer era esto
        Bytepos = self.splitByte(6)
        listBit = self.splitIntoBits()
        print("List Bit, Byte7", listBit)
        valf1 = [listBit[0], listBit[1], listBit[2], listBit[3], listBit[4]]
        ba = "".join(map(str, valf1))
        numero1 = int(ba, 2)
        print("BYTE7:Internal Device Address:", numero1)
        #AGREGAR TABLA DE DIRECCIONES

    def Comp8Byte(self): # No es necesario pasar info como parámetro ya lo guardaste en la clase como self.info
        #Analyze Byte8
        #self.info = tm # Esta declaración está tm no ha sido declarada en ningún lugar
        #tm = self.info # Creo que lo que querías hacer era esto
        Bytepos = self.splitByte(7)
        listBit = self.splitIntoBits()
        print("List Bit, Byte8", listBit)
        valf1 = [listBit[0], listBit[1], listBit[2], listBit[3], listBit[4],listBit[5]]
        ba = "".join(map(str, valf1))
        numero1 = int(ba, 2)
        print("BYTE8:Bit0-5:length of the data included in this message starting from 10th Byte.:",numero1)
        #A implementar lecturas
        print("BYTE8:Bit6: Value should be 0:",listBit[6])
        print("BYTE8:Bit7: Value should be 0:",listBit[6])

    def Comp9Byte(self): # No es necesario pasar info como parámetro ya lo guardaste en la clase como self.info
        #self.info = tm # Esta declaración está tm no ha sido declarada en ningún lugar
        #tm = self.info # Creo que lo que querías hacer era esto
        Bytepos = self.splitByte(8)
        listBit = self.splitIntoBits()
        print("List Bit, Byte9", listBit)
        ba = "".join(map(str, listBit))
        numero1 = int(ba, 2)
        print("Command code for Module, ISOCOMM, or PFC.:",numero1)

######   ////marcador para unificar
    def CommunicationIHP(self, NumberF):
        self.NumberF = NumberF #Number function

        serverAddressPort = (self.ip, self.port)
        bufferSize = 1024
        # Create a UDP socket at client side
        UDPClientSocket = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)

        if(NumberF == "1"): self.WriteProtect(False)
        elif(NumberF == "2"): self.OperationMode(6)
        elif(NumberF == "3"): self.Vref()
        elif(NumberF == "4"): self.Iref(2, 35)
        elif(NumberF == "5"): self.Operation()
        elif(NumberF == "6"): self.WriteProtectDisableM(3)
        elif(NumberF == "7"): self.ModuleSave()

        bytesToSend = self.z
        # Send to server using created UDP socket
        UDPClientSocket.settimeout(2) # Sets the socket to timeout after 1 second of no activity
        UDPClientSocket.sendto(bytesToSend, serverAddressPort)
        print("Sent:", bytesToSend, "Len:", len(bytesToSend))
        try:
            msgFromServer = UDPClientSocket.recvfrom(bufferSize)
            msg = "Message from Server {}".format(msgFromServer[0])
            print(msg)
        except socket.timeout: # fail after 1 second of no activity
            print("Didn't receive data! [Timeout]")
        data = msgFromServer[0]
        self.splitBytes(data)
        lentrama = len(data)
        # Creo que este if lo podemos mejorar, no recuerdo el mínimo de los bytes que esperamos pero creo que siempre mínimo son 8 o 9
        # No es necesario agregar los casos con menos bytes, si tenemos menos bytes es un error y tenemos que manejar ese error de alguna manera
        if (lentrama==5):
            self.Comp4FBytes() # info ya esta guardada en self.info gracias a la función splitBytes
            self.Comp5Byte() # info ya esta guardada en self.info gracias a la función splitBytes
        if (lentrama==6):
            self.Comp4FBytes() # info ya esta guardada en self.info gracias a la función splitBytes
            self.Comp5Byte() # info ya esta guardada en self.info gracias a la función splitBytes
            self.Comp6Byte() # info ya esta guardada en self.info gracias a la función splitBytes
        if (lentrama==7):
            self.Comp4FBytes() # info ya esta guardada en self.info gracias a la función splitBytes
            self.Comp5Byte() # info ya esta guardada en self.info gracias a la función splitBytes
            self.Comp6Byte() # info ya esta guardada en self.info gracias a la función splitBytes
            self.Comp7Byte() # info ya esta guardada en self.info gracias a la función splitBytes
        if (lentrama==8):
            self.Comp4FBytes() # info ya esta guardada en self.info gracias a la función splitBytes
            self.Comp5Byte() # info ya esta guardada en self.info gracias a la función splitBytes
            self.Comp6Byte() # info ya esta guardada en self.info gracias a la función splitBytes
            self.Comp7Byte() # info ya esta guardada en self.info gracias a la función splitBytes
            self.Comp8Byte() # info ya esta guardada en self.info gracias a la función splitBytes
        if (lentrama>=9):
            self.Comp4FBytes() # info ya esta guardada en self.info gracias a la función splitBytes
            self.Comp5Byte() # info ya esta guardada en self.info gracias a la función splitBytes
            self.Comp6Byte() # info ya esta guardada en self.info gracias a la función splitBytes
            self.Comp7Byte() # info ya esta guardada en self.info gracias a la función splitBytes
            self.Comp8Byte() # info ya esta guardada en self.info gracias a la función splitBytes
            self.Comp9Byte() # info ya esta guardada en self.info gracias a la función splitBytes

# Debug
def main():
    ihp = IHP('D0:03:EB:A2:DD:14', '192.168.6.1/23', 8888)
    ihp.CommunicationIHP('6')
    
if __name__ == '__main__':
    main()