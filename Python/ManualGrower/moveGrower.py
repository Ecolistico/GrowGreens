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
import time

timer = time.time()
flag1=False

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
    nodoX = int(calX)/41
    nodoY = int(calY)/10
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
    timer = time.time()
    flag1=False
    while gui.isOpen:
        gui.run()
        mqttCloud.client.loop(0.05)
        #########FUNCION actualiza la posicion cada 3 segundos. Con timer
        if (time.time()-timer>5 and flag1==True):
            timer = time.time()
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
        ##########
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
            if (1<= gui.gr <= 8):
                calX = mqttCloud.CalX1
                calY = mqttCloud.CalY1
                if (int(calX) != 0 and int(calY) != 0):
                    print("CalXY Ready: ",calX, calY)
                    flag1=True
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
            if (1<= gui.gr <= 8):
                #print("Moving to Pos: X:{}, Y:{}")
                XObj = gui.XObj
                YObj = gui.YObj
                print("Move TO: ",XObj, YObj)
                gui.updateStatus("Moving X: {}, Y: {}...".format(XObj,YObj))
                publish.single(mqttCloud.pub,
                                'movePosXY,{},{},{}'.format(gui.gr,XObj,YObj),
                                hostname=mqttCloud.server_ip)
                gui.move = False

        if gui.up == True:
            if (1<= gui.gr <= 8):
                #print("Moving to Pos: X:{}, Y:{}")
                #X,YObj tamaño de pasos
                XPaso, YPaso = createNodos(mqttCloud.CalX1, mqttCloud.CalY1)
                XObj = XPaso + int(mqttCloud.X1)
                if XObj < float(mqttCloud.CalX1):                 
                    print("Move X: ",XPaso, 0)
                    gui.updateStatus("Moving UP:{},{}".format(XPaso,0))
                    publish.single(mqttCloud.pub,
                                    'movePosXY,{},{},{}'.format(gui.gr,XPaso,0),
                                    hostname=mqttCloud.server_ip)
                gui.up = False
                
        if gui.downB == True:
            if (1<= gui.gr <= 8):
                #print("Moving to Pos: X:{}, Y:{}")
                #X,YObj tamaño de pasos
                XPaso, YPaso = createNodos(mqttCloud.CalX1, mqttCloud.CalY1)
                XObj = int(mqttCloud.X1) - XPaso                  
                print("Move X: ",-(XPaso), 0)
                gui.updateStatus("Moving Down:{},{}".format(-(XPaso),0))
                publish.single(mqttCloud.pub,
                                'movePosXY,{},{},{}'.format(gui.gr,-(XPaso),0),
                                hostname=mqttCloud.server_ip)
                gui.downB = False

        if gui.right == True:
            if (1<= gui.gr <= 8):
                #print("Moving to Pos: X:{}, Y:{}")
                #X,YObj tamaño de pasos
                XPaso, YPaso = createNodos(mqttCloud.CalX1, mqttCloud.CalY1)
                YObj = YPaso + int(mqttCloud.Y1)                 
                print("Move Y: ",0, YObj)
                gui.updateStatus("Moving Right:{},{}".format(0,YPaso))
                publish.single(mqttCloud.pub,
                                'movePosXY,{},{},{}'.format(gui.gr,0,YPaso),
                                hostname=mqttCloud.server_ip)
                gui.right = False

        if gui.left == True:
            if (1<= gui.gr <= 8):
                #print("Moving to Pos: X:{}, Y:{}")
                #X,YObj tamaño de pasos
                XPaso, YPaso = createNodos(mqttCloud.CalX1, mqttCloud.CalY1)
                YObj = int(mqttCloud.Y1) - YPaso                 
                print("Move Y: ",0, YObj)
                gui.updateStatus("Moving Left:{},{}".format(0,-(Ypaso)))
                publish.single(mqttCloud.pub,
                                'movePosXY,{},{},{}'.format(gui.gr,0,-(YPaso)),
                                hostname=mqttCloud.server_ip)
                gui.left = False

        if gui.iron == True:
            if (1<= gui.gr <= 8):
                print("IR ON: ")
                gui.updateStatus("IR ON")
                publish.single('23-009-004/Grower{}/log'.format(gui.gr),
                                'OnOut2',
                                hostname=mqttCloud.server_ip)
                gui.window["data_iron"].update(disabled=True)
                gui.window["data_iroff"].update(disabled=False)
                gui.iron = False

        if gui.iroff == True:
            if (1<= gui.gr <= 8):
                print("IR OFF")
                gui.updateStatus("IR OFF")
                publish.single('23-009-004/Grower{}/log'.format(gui.gr),
                                'OffOut2',
                                hostname=mqttCloud.server_ip)
                gui.window["data_iron"].update(disabled=False)
                gui.window["data_iroff"].update(disabled=True)
                gui.iroff = False

if __name__ == '__main__':
    main()
