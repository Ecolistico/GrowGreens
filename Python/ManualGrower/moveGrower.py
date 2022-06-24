#!/usr/bin/env python3

# Import directories
# Import modules
import os
import sys
import paho.mqtt.client as mqtt
import paho.mqtt.publish as publish
from time import time, sleep, strftime, localtime
sys.path.insert(0, './src/')
from gui import GUI
from mqttControl import mqttClient

def updatePos(gr):
    if (1<= gr <= 4):
        publish.single(mqttCloud.pub,
                        'syncPosCloud,{}'.format(gr),
                        hostname=mqttCloud.server_ip)
        mqttCloud.client.loop(0.2)
        publish.single(mqttCloud.pub,
                         'syncPosCloud,{}'.format(gr),
                        hostname=mqttCloud.server_ip)
        mqttCloud.client.loop(0.2)
        posX = mqttCloud.X1
        posY = mqttCloud.Y1
        gui.updatePos(posX, posY)
        print("Pos: ",posX, posY)

def createNodos(calX, calY):
    nodoX = int(calX/41)
    nodoY = int(calY/10)
    return nodoX, nodoY
    
def moveNodo(nX, nY):
    pass

def controlDireccion(evento):
    if evento == "data_up":
        print("Hola")
    elif evento == "data_down":
        print("Hola")
    elif evento == "data_left":
        print("Hola")
    elif evento == "data_right":    
        print("Hola")

def main():
    gui = GUI()
    gui.begin()
    mqttCloud = mqttClient()
    # Initialize MQTT connection
    mqttCloud.mqttInit()
    gui.updateStatus("Sincroniza un piso(1,3,6,8)")
    gui.window["data_home"].update(disabled=True)
    gui.window["data_move"].update(disabled=True)
    #Sync Here######
    while gui.isOpen:
        gui.run()
        mqttCloud.client.loop(0.05)
        if gui.sync == True:
            gui.updateStatus("Sincronizando...")
            print("Sync....")
            publish.single(mqttCloud.pub,
                            'syncCalCloud,{}'.format(gui.gr),
                            hostname=mqttCloud.server_ip)
            mqttCloud.client.loop(0.2)
            publish.single(mqttCloud.pub,
                            'syncCalCloud,{}'.format(gui.gr),
                            hostname=mqttCloud.server_ip)
            mqttCloud.client.loop(0.2)
            if (1<= gui.gr <= 4):
                calX = mqttCloud.CalX1
                calY = mqttCloud.CalY1
                if (int(calX) != 0 and int(calY) != 0):
                    print("CalXY Ready: ",calX, calY)
                    ###UpdatePos
                    publish.single(mqttCloud.pub,
                                    'syncPosCloud,{}'.format(gui.gr),
                                    hostname=mqttCloud.server_ip)
                    mqttCloud.client.loop(0.2)
                    publish.single(mqttCloud.pub,
                                     'syncPosCloud,{}'.format(gui.gr),
                                    hostname=mqttCloud.server_ip)
                    mqttCloud.client.loop(0.2)
                    posX = mqttCloud.X1
                    posY = mqttCloud.Y1
                    gui.updatePos(posX, posY)
                    print("Pos: ",posX, posY)
                 
                    ##
                    
                else:
                    print("Error, SyncCalXY")
                    gui.updateStatus("ERROR: SyncCalXY")
                    break
            else:
                gui.updateStatus("ERROR:Piso Incorrecto")
                gui.window["data_sincronizar"].update(disabled=True)
                gui.window["data_home"].update(disabled=True)
                gui.window["data_move"].update(disabled=True)
                gui.window["pis"].update(disabled=True)
                break
                
            gui.sync = False
            gui.window["data_home"].update(disabled=False)
            gui.window["data_move"].update(disabled=False)
            #gui.window["data_sincronizar"].update(disabled=True)
            gui.updateStatus("Ready to go!")
            print("Sync Ready")

        if gui.home == True:
            print("Home")
            gui.updateStatus("Moving To Home..")
            publish.single(mqttCloud.pub,
                            'home,{}'.format(gui.gr),
                            hostname=mqttCloud.server_ip)
            gui.home = False

        if gui.move == True:
            if (1<= gui.gr <= 4):
                #print("Moving to Pos: X:{}, Y:{}")
                XObj = gui.XObj
                YObj = gui.YObj
                print("Move TO: ",XObj, YObj)
                publish.single(mqttCloud.pub,
                                'movePosXY,{},{},{}'.format(gui.gr,XObj,YObj),
                                hostname=mqttCloud.server_ip)
                gui.move = False

if __name__ == '__main__':
    main()
