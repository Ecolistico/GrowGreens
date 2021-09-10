#!/usr/bin/env python3

# Import directories
import os
import sys
import time
import getpass
import webbrowser
sys.path.insert(0, './src/')
from sysMaster import runShellCommand, isWiFi, getGatewayIPaddr, clear, ping
    
config = False
reconfig = ""
attemp = 0
WiFi = True
staticIP = ""

print("\033[1;32;40mConfigurando Grow Greens")
print("\033[0;37;40mRevisando configuración WiFi...")
time.sleep(2)
if(not isWiFi()): # Checking WiFi status
    WiFi = False
    print("No se detecto ninguna red WiFi por favor configurela antes de continuar")
    input("Presione enter para continuar")

if(WiFi or isWiFi()): # Check if previous configuration files exists
    print("Revisando configuraciones previas...")
    time.sleep(2)
    if(os.path.isfile("config.json")):
        print("El sistema ya había sido previamente configurado, si lo haces nuevamente se perdera la configuración" +
              " anterior.")
        reconfig = input("¿Deseas continuar? s/n\n")
        if(reconfig.startswith("s") or reconfig.startswith("S")):
            while(attemp<3):
                password = getpass.getpass()
                if(password == "yomero"):
                    attemp = 10
                    runShellCommand('sudo rm config.json')
                    config = True
                else:
                    attemp += 1
    else:
        config = True
        
    if(config): # Open browser to config system
        runShellCommand('sudo rm /var/www/growgreens-setup.com/public/data/config.json')
        print("A continuación, se abrirá una página web solicitando información acerca de su ubicación" +
              " actual. Por favor siga las indicaciones y después regrese a está ventana.")
        time.sleep(3.5)
        webbrowser.open('http://growgreens-setup.com')  # Go to growgreens-setup.com
        while(not os.path.isfile("/var/www/growgreens-setup.com/public/data/config.json")):
            pass
        clear()
        runShellCommand('sudo cp /var/www/growgreens-setup.com/public/data/config.json config.json')
        print("La ubicación fue configurada correctamente")
        
        static = input("¿Es el primer sistema que se configura en la bodega? s/n\n")
        # If we do not have an MQTT Broker we need to set an static IP
        if(static.startswith("s") or static.startswith("s")):
            Gateway_IP = getGatewayIPaddr()
            IP_array = Gateway_IP.split('.')
            red = IP_array[0]+"."+IP_array[1]+"."+IP_array[2]+"."
        
            # Check availables IP
            print("A continuación se realizará un escaneo de la red WiFi")
            print("[*] El escaneo se está realizando desde",red+str(100),"hasta",red+str(125))
            busy = False
            for subred in range(100, 125):
                if(int(IP_array[3])!=subred):
                    print(".", end = "")
                    direccion = red+str(subred)
                    response = os.popen(ping()+" "+direccion)
                    for line in response.readlines():
                        if ("ttl" in line.lower()):
                            print()
                            busy = True
                            break
                    if(not busy):
                        print("\nSe encontró una IP disponible para ser configurada")
                        staticIP = direccion
                        break
            
            """
            Raspberry Pi is just a client. This step is not necessary
            # Config new static IP on raspberry pi
            runShellCommand('sudo cp ./src/configFiles/dhcpcd.conf.orig dhcpcd.conf')
            runShellCommand('sudo python ./src/setStaticIP.py ./dhcpcd.conf {0} {1}'.format(staticIP, Gateway_IP))
            runShellCommand('sudo cp dhcpcd.conf /etc/dhcpcd.conf')
            runShellCommand('sudo rm dhcpcd.conf')
            runShellCommand('sudo systemctl daemon-reload')
            runShellCommand('sudo systemctl stop dhcpcd.service')
            for net_dev in os.listdir('/sys/class/net'):
                runShellCommand('sudo ip addr flush dev {}'.format(net_dev))
            runShellCommand('sudo systemctl start dhcpcd.service')
            runShellCommand('sudo systemctl restart networking.service')
            print("Se configuró este dispósitivo con una IP estática={}".format(staticIP))
            """
            
        if(len(staticIP)>3): runShellCommand("sudo python ./src/genID.py config.json ./src/municipios.json {}".format(staticIP))
        print("\033[0;37;40mSe reiniciará el equipo para que los cambios surtan efecto.")
        input("Presione enter para finalizar")
        runShellCommand('sudo reboot')
        
    if(attemp==3):
        print("No esta autorizado para hacer las modificaciones correspondientes. Por favor contacte a su supervisor.")
        
else:
    print("Red WiFi no configurada, por favor configurela para continuar")
