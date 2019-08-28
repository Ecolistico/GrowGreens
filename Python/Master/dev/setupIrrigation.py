#!/usr/bin/env python3

# Import directories
import os
import sys
import time
import json
import getpass
sys.path.insert(0, './src/')
from sysMaster import runShellCommand

# Aux Variables
config = False
reconfig = ""
attemp = 0
cyclesConfig = False
beginConfig = False
orderConfig = False
percentConfig = False

print("\033[1;32;40mConfigurando orden riego Grow Greens")
print("\033[0;37;40mRevisando configuraciones previas...")
time.sleep(2)
if(os.path.isfile("irrigation.json")):
    print("El sistema ya había sido previamente configurado, si lo haces nuevamente se perdera la configuración" +
          " anterior.")
    reconfig = input("¿Deseas continuar? s/n\n")
    if(reconfig.startswith("s") or reconfig.startswith("S")):
        while(attemp<3):
            password = getpass.getpass()
            if(password == "yomero"):
                attemp = 10
                runShellCommand('sudo rm irrigation.json')
                config = True
            else:
                attemp += 1
else:
    config = True

if(config):
    while not cyclesConfig:
        cycles = input("¿Cuántos ciclos de riego habrá? ")
        try:
            cycles = int(cycles)
            if 24%cycles==0 :
                cyclesConfig = True
            else: print("El número de ciclos debe ser divisor de 24")
        except: print("Debe introducir un número")
    while not beginConfig:
        begin = input("¿A qué hora comienza el primer ciclo? ")
        try:
            begin = int(begin)
            if(begin>=0 and begin<24): beginConfig = True
            else: print("La hora debe ser un número entre 0 y 23.99")
        except: print("Debe introducir un número")
    while not orderConfig:
        order = input("Escriba el orden en qué quiere que se rieguen las soluciones (separado por comas): ")
        param = order.split(",")
        if(len(param)==4 and '1' in param and '2' in param and '3' in param and '4' in param):
            orderConfig = True
        else: print("Debe introducir los números del 1 al 4 separados por comas en el orden que lo desee")
    
    while not percentConfig:
        percent = input("Escriba el porcentaje de riego que desea otorgar a cada solución (separado por comas): ")
        param1 = percent.split(",")
        try:
            percentTotal = 0
            for x in param1: percentTotal += float(x)
            if(percentTotal==100):
                percentConfig = True
            else: print("Debe introducir 4 números separados por comas cuya suma sea igual 100")
        except: print("Debe introducir 4 números separados por comas")
        
    # Put begin hours into a dictionary
    beginHour = {}
    for i in range(cycles):
        beginHour['{}'.format(i+1)] = begin+(24/cycles*i)
    
    # Put order list into a dictionary
    orderData = {}
    for i,orderNumber in enumerate(param): orderData['{}'.format(i+1)] = int(orderNumber)
    
    # Put percentage list into a dictionary
    percentData = {}
    for i, percentNumber in enumerate(param1): percentData['{}'.format(i+1)] = percentNumber
    
    data = {'beginHour' : beginHour,
            'order' : orderData,
            'percent': percentData}
    
    # Writing JSON data
    with open('irrigation.json', 'w') as f:
        json.dump(data, f)
    
    print("La configuración fue exitosa")
    input("Presione enter para finalizar")
    
if(attemp==3):
    print("No esta autorizado para hacer las modificaciones correspondientes. Por favor contacte a su supervisor.")
        