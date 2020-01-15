#!/usr/bin/env python3
import os
import sys
import time
sys.path.insert(0, './src/')
from sysMaster import runShellCommand

deviceDetected = False
serialNumber1 = ""
idProduct1 = ""
idVendor1 = ""
serialNumber2 = ""
idProduct2 = ""
idVendor2 = ""
serialNumber3 = ""
idProduct3 = ""
idVendor3 = ""

# Start
print("\033[1;32;40mGrow Greens Configuración (USB)")
print("\033[0;37;40mPor favor desconecte todos los dispósitivos USB de la computadora.")
ready = input("Una vez que termine presione enter para continuar")
# Initial Status
runShellCommand('dmesg > dmesg.log')
print("Obteniendo estado actual del dispósitivo...")
dmesgSize = os.path.getsize('dmesg.log')
time.sleep(1.5)
# Connecting new device
print("Conecte el microcontrolador que será usado para el control general del sistema.")
print("Buscando dispósitivo", end = "")
actualTime = time.time()
# Waiting to detect device
while not deviceDetected:
    if(time.time()-actualTime>1):
        actualTime = time.time()
        print(".", end="", flush=True)
        runShellCommand('dmesg > dmesg1.log')
        dmesg1Size = os.path.getsize('dmesg1.log')
        # If new device is detected
        if(dmesgSize!=dmesg1Size):
            print("\nDispósitivo encontrado")
            runShellCommand('cp dmesg1.log dmesg.log')
            runShellCommand('rm dmesg1.log')
            # Get relevant info
            for line in reversed(list(open("dmesg.log"))):
                if("SerialNumber" in line and not "New USB device" in line):
                    serialNumber1 = line
                if("idProduct" in line):
                    idProduct1 = line
                if(serialNumber1!="" and idProduct1!=""):
                    deviceDetected = True
                    break
                
print("Buscando información necesaria para configurar el dispositivo...")
serialArray = serialNumber1.split()
for parameter in serialArray:
    if "SerialNumber" in parameter:
        if(parameter[-1]==","):
            serialNumber1 = parameter[0:-2]+"="+serialArray[-1]
        else:
            serialNumber1 = parameter[0:-1]+"="+serialArray[-1]
        break
for parameter in idProduct1.split():
    if "idProduct" in parameter:
        if(parameter[-1]==","):
            idProduct1 = parameter[0:-1]
        else:
            idProduct1 = parameter
    if "idVendor" in parameter:
        if(parameter[-1]==","):
            idVendor1 = parameter[0:-1]
        else:
            idVendor1 = parameter

time.sleep(1)
print("Información recolectada\n")
time.sleep(1)

# Search 2nd device
deviceDetected = False
runShellCommand('dmesg > dmesg.log')
dmesgSize = os.path.getsize('dmesg.log')
print("Conecte el microcontrolador que será usado para el control del movimiento de los tocanis.")
print("Buscando dispósitivo", end = "")
actualTime = time.time()

while not deviceDetected:
    if(time.time()-actualTime>1):
        actualTime = time.time()
        print(".", end="", flush=True)
        runShellCommand('dmesg > dmesg1.log')
        dmesg1Size = os.path.getsize('dmesg1.log')
        # If new device is detected
        if(dmesgSize!=dmesg1Size):
            print("\nDispósitivo encontrado")
            runShellCommand('cp dmesg1.log dmesg.log')
            runShellCommand('rm dmesg1.log')
            # Get relevant info
            for line in reversed(list(open("dmesg.log"))):
                if("SerialNumber" in line and not "New USB device" in line):
                    serialNumber2 = line
                if("idProduct" in line):
                    idProduct2 = line
                if(serialNumber2!="" and idProduct2!=""):
                    deviceDetected = True
                    break

print("Buscando información necesaria para configurar el dispositivo...")
serialArray = serialNumber2.split()
for parameter in serialArray:
    if "SerialNumber" in parameter:
        if(parameter[-1]==","):
            serialNumber2 = parameter[0:-2]+"="+serialArray[-1]
        else:
            serialNumber2 = parameter[0:-1]+"="+serialArray[-1]
        break
for parameter in idProduct2.split():
    if "idProduct" in parameter:
        if(parameter[-1]==","):
            idProduct2 = parameter[0:-1]
        else:
            idProduct2 = parameter
    if "idVendor" in parameter:
        if(parameter[-1]==","):
            idVendor2 = parameter[0:-1]
        else:
            idVendor2 = parameter

time.sleep(1)
print("Información recolectada\n")
time.sleep(1)

# Search 3rd device
deviceDetected = False
runShellCommand('dmesg > dmesg.log')
dmesgSize = os.path.getsize('dmesg.log')
print("Conecte el microcontrolador que será usado para realizar las soluciones de riego.")
print("Buscando dispósitivo", end = "")
actualTime = time.time()

while not deviceDetected:
    if(time.time()-actualTime>1):
        actualTime = time.time()
        print(".", end="", flush=True)
        runShellCommand('dmesg > dmesg1.log')
        dmesg1Size = os.path.getsize('dmesg1.log')
        # If new device is detected
        if(dmesgSize!=dmesg1Size):
            print("\nDispósitivo encontrado")
            runShellCommand('cp dmesg1.log dmesg.log')
            runShellCommand('rm dmesg1.log')
            # Get relevant info
            for line in reversed(list(open("dmesg.log"))):
                if("SerialNumber" in line and not "New USB device" in line):
                    serialNumber3 = line
                if("idProduct" in line):
                    idProduct3 = line
                if(serialNumber3!="" and idProduct3!=""):
                    deviceDetected = True
                    break

print("Buscando información necesaria para configurar el dispositivo...")
serialArray = serialNumber3.split()
for parameter in serialArray:
    if "SerialNumber" in parameter:
        if(parameter[-1]==","):
            serialNumber3 = parameter[0:-2]+"="+serialArray[-1]
        else:
            serialNumber3 = parameter[0:-1]+"="+serialArray[-1]
        break
for parameter in idProduct3.split():
    if "idProduct" in parameter:
        if(parameter[-1]==","):
            idProduct3 = parameter[0:-1]
        else:
            idProduct3 = parameter
    if "idVendor" in parameter:
        if(parameter[-1]==","):
            idVendor3 = parameter[0:-1]
        else:
            idVendor3 = parameter

time.sleep(1)
print("Información recolectada\n")
time.sleep(1)

# Clean results
serialNumber1 = serialNumber1.split("=")[1]
idProduct1 = idProduct1.split("=")[1]
idVendor1 = idVendor1.split("=")[1]
serialNumber2 = serialNumber2.split("=")[1]
idProduct2 = idProduct2.split("=")[1]
idVendor2 = idVendor3.split("=")[1]
serialNumber3 = serialNumber3.split("=")[1]
idProduct3 = idProduct3.split("=")[1]
idVendor3 = idVendor3.split("=")[1]

with open("microcontroller.rules", "w") as f:
    
    f.write('SUBSYSTEM=="tty",')
    f.write(' ATTRS{idVendor}=="{value}",'.format(idVendor="{idVendor}", value=idVendor1))
    f.write(' ATTRS{idProduct}=="{value}",'.format(idProduct="{idProduct}", value=idProduct1))
    f.write(' ATTRS{serial}=="{value}",'.format(serial="{serial}", value=serialNumber1))
    f.write(' SYMLINK+="generalControl",')
    f.write(' GROUP="dialout",')
    f.write(' MODE="0664"\n')
    f.write('SUBSYSTEM=="tty",')
    f.write(' ATTRS{idVendor}=="{value}",'.format(idVendor="{idVendor}", value=idVendor2))
    f.write(' ATTRS{idProduct}=="{value}",'.format(idProduct="{idProduct}", value=idProduct2))
    f.write(' ATTRS{serial}=="{value}",'.format(serial="{serial}", value=serialNumber2))
    f.write(' SYMLINK+="motorsGrower",')
    f.write(' GROUP="dialout",')
    f.write(' MODE="0664"\n')
    f.write('SUBSYSTEM=="tty",')
    f.write(' ATTRS{idVendor}=="{value}",'.format(idVendor="{idVendor}", value=idVendor3))
    f.write(' ATTRS{idProduct}=="{value}",'.format(idProduct="{idProduct}", value=idProduct3))
    f.write(' ATTRS{serial}=="{value}",'.format(serial="{serial}", value=serialNumber3))
    f.write(' SYMLINK+="solutionMaker",')
    f.write(' GROUP="dialout",')
    f.write(' MODE="0664"\n')
    f.close()

# Finish and clean
runShellCommand("sudo cp microcontroller.rules /etc/udev/rules.d/microcontroller.rules")
runShellCommand("sudo rm microcontroller.rules")
runShellCommand("sudo rm dmesg.log")
runShellCommand("sudo /etc/init.d/udev restart")

print("Para que los cambios surtan efecto se deben desconectar y volver a conectar todos" +
      " los microcontroladores")
time.sleep(1)
input("Presione enter para finalizar")