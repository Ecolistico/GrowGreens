#!/usr/bin/env python3
import sys
import os
import json
sys.path.insert(0, './src/')
from sysMaster import runShellCommand, clear
import webbrowser
from time import time

if(os.path.isfile("config.json")):
    with open("config.json", "r") as f:
        data = json.load(f)
        state = data["state"]
        city = data["city"]
        num = data["num"]
        ID = data["ID"]
        if "staticIP" in data:
            staticIP = data["staticIP"]
        else:
            staticIP = "No existe"
        f.close()

    with open("index.html", "w") as f:
        f.write('<!DOCTYPE html>\n')
        f.write("<html lang='en'>\n")
        f.write('\t<head>\n\t\t<meta charset="UTF-8">\n')
        f.write('\t\t<title>GrowGreens Status</title>\n')
        f.write('\t\t<link rel="icon" href="img/GrowGreens-Web.png">\n')
        f.write('\t\t<link href="http://fonts.googleapis.com/icon?family=Material+Icons" rel="stylesheet">\n')
        f.write('\t\t<link href= "https://fonts.googleapis.com/css?family=Open+Sans" rel="stylesheet">\n')
        f.write('\t\t<link rel="stylesheet" href="css/style.css">\n')
        f.write('\t</head>\n')
        f.write('\t<body>\n')
        f.write('\t\t<div class="container">\n')
        f.write('\t\t\t<h1 class="header">GrowGreens Status</h1>\n')
        f.write('\t\t\t\t<form class="form-wrapper">\n')
        f.write('\t\t\t\t\t<fieldset class="section is-active">\n')
        f.write('\t\t\t\t\t\t<p align="left"><b>Ubicación: </b>{0}, {1}<br></p>\n'.format(city, state))
        f.write('\t\t\t\t\t\t<p align="left"><b>Número de contenedor: </b>{}<br></p>\n'.format(num))
        f.write('\t\t\t\t\t\t<p align="left"><b>Container ID: </b>{}<br></p>\n'.format(ID))
        if(staticIP!="No existe"):
            f.write('\t\t\t\t\t\t<p align="left"><b>Broker IP: </b>{}<br></p>\n'.format(staticIP))
        else:
            f.write('\t\t\t\t\t\t<p align="left"><b>Broker IP: </b>Este dispósitivo no es el broker' +
                    ' que maneja la comunicación MQTT.<br><br>Por lo general la dirección IP del' +
                    ' broker corresponde con la de la computadora central del primer sistema que' +
                    ' fue instalado en la nave/bodega industrial.</p>\n')
        f.write('\t\t\t\t\t</fieldset>\n')
        f.write('\t\t\t\t</form>\n')
        f.write('\t\t\t</div>\n')
        f.write('\t\t</body>\n')
        f.write('</html>\n')
        
        f.close()
        
    runShellCommand('sudo cp index.html /var/www/growgreens-status.com/public/index.html')
    runShellCommand('sudo rm index.html')
    webbrowser.open('http://growgreens-status.com')  # Go to growgreens-setup.com
    actualTime = time()
    while(time()-actualTime<10):
        pass
    clear()
    input("Presione enter para finalizar")
    
else:
    print("\033[1;31;40mError: No se encuentra el archivo de configuración." +
          " Por favor ejecute primero el programa de configuración 'setup.py'")
    setup = input("\033[0;37;40m¿Desea ejecutarlo ahora? s/n\n")
    if(setup.startswith("s") or setup.startswith("s")):
        command = "./setup.py"
        runShellCommand('lxterminal -t "GrowGreens Setup" --command='+command)
        