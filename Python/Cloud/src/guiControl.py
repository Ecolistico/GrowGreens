import os
import sys
import paho.mqtt.client as mqtt
import paho.mqtt.publish as publish
from time import time, sleep, strftime, localtime
import time

class guiController:
    def __init__(self, systemID, brokerIP, mqttController, gui):
        # Publish require varaibles
        self.ID = systemID
        self.IP = brokerIP
        # Define communication controllers
        self.mqttControl = mqttController
        # Define connection with gui
        self.gui = gui
        # Aux Variables
        self.timer = time()
        self.flag1=False

    def createNodos(self, calX, calY):
        nodoX = int(calX)/41
        nodoY = int(calY)/10
        return nodoX, nodoY

    def loop(self):
        #########FUNCION actualiza la posicion cada 3 segundos. Con timer
        if (time.time()-self.timer>1500 and self.flag1==True):
            self.timer = time.time()
            ###UpdatePos
            publish.single("{}/Master".format(self.mqttControl.ID),
                            'syncPosCloud,{}'.format(self.gui.gr),
                             hostname=self.mqttControl.brokerIP)
            self.mqttControl.client.loop(0.2)
            publish.single("{}/Master".format(self.mqttControl.ID),
                             'syncPosCloud,{}'.format(self.gui.gr),
                             hostname=self.mqttControl.brokerIP)
            self.mqttControl.client.loop(0.2)
            posX = self.mqttControl.X1
            posY = self.mqttControl.Y1
            self.gui.updatePos(posX, posY)
            print("Pos: ",posX, posY)
        ##########
        if self.gui.sync == True:
            self.gui.updateStatus("Sincronizando...")
            print("Sync....")
            publish.single("{}/Master".format(self.mqttControl.ID),
                            'syncCalCloud,{}'.format(self.gui.gr),
                            hostname=self.mqttControl.brokerIP)
            self.mqttControl.client.loop(0.2)
            publish.single("{}/Master".format(self.mqttControl.ID),
                            'syncCalCloud,{}'.format(self.gui.gr),
                            hostname=self.mqttControl.brokerIP)
            self.mqttControl.client.loop(0.2)
            if (1<= self.gui.gr <= 8):
                calX = self.mqttControl.CalX1
                calY = self.mqttControl.CalY1
                if (int(calX) != 0 and int(calY) != 0):
                    print("CalXY Ready: ",calX, calY)
                    self.flag1=True
                    ###UpdatePos
                    publish.single("{}/Master".format(self.mqttControl.ID),
                                    'syncPosCloud,{}'.format(gui.gr),
                                    hostname=self.mqttControl.brokerIP)
                    self.mqttControl.client.loop(0.2)
                    posX = self.mqttControl.X1
                    posY = self.mqttControl.Y1
                    self.gui.updatePos(posX, posY)
                    print("Pos: ",posX, posY)

                    ##

                else:
                    print("Error, SyncCalXY")
                    self.gui.updateStatus("ERROR: SyncCalXY")
                    #break
            else:
                self.gui.updateStatus("ERROR:Piso Incorrecto")
                self.gui.window["data_sincronizar"].update(disabled=True)
                self.gui.window["data_home"].update(disabled=True)
                self.gui.window["data_move"].update(disabled=True)
                self.gui.window["pis"].update(disabled=True)
                #break

            self.gui.sync = False
            self.gui.window["data_home"].update(disabled=False)
            self.gui.window["data_move"].update(disabled=False)
            #gui.window["data_sincronizar"].update(disabled=True)
            self.gui.updateStatus("Ready to go!")
            print("Sync Ready")

        if self.gui.home == True:
            print("Home")
            self.gui.updateStatus("Moving To Home..")
            publish.single("{}/Master".format(self.mqttControl.ID),
                            'home,{}'.format(self.gui.gr),
                            hostname=self.mqttControl.brokerIP)
            self.gui.home = False

        if self.gui.move == True:
            if (1<= self.gui.gr <= 8):
                #print("Moving to Pos: X:{}, Y:{}")
                XObj = self.gui.XObj
                YObj = self.gui.YObj
                print("Move TO: ",XObj, YObj)
                self.gui.updateStatus("Moving X: {}, Y: {}...".format(XObj,YObj))
                publish.single("{}/Master".format(self.mqttControl.ID),
                                'movePosXY,{},{},{}'.format(gui.gr,XObj,YObj),
                                hostname=self.mqttControl.brokerIP)
                self.gui.move = False

        if self.gui.up == True:
            if (1<= self.gui.gr <= 8):
                #print("Moving to Pos: X:{}, Y:{}")
                #X,YObj tamaño de pasos
                XPaso, YPaso = self.createNodos(self.mqttControl.CalX1, self.mqttControl.CalY1)
                XObj = XPaso + int(self.mqttControl.X1)
                if XObj < float(self.mqttControl.CalX1):
                    print("Move X: ",XPaso, 0)
                    self.gui.updateStatus("Moving UP:{},{}".format(XPaso,0))
                    publish.single("{}/Master".format(self.mqttControl.ID),
                                    'movePosXY,{},{},{}'.format(self.gui.gr,XPaso,0),
                                    hostname=self.mqttControl.brokerIP)
                self.gui.up = False

        if self.gui.downB == True:
            if (1<= self.gui.gr <= 8):
                #print("Moving to Pos: X:{}, Y:{}")
                #X,YObj tamaño de pasos
                XPaso, YPaso = self.createNodos(self.mqttControl.CalX1, self.mqttControl.CalY1)
                XObj = int(self.mqttControl.X1) - XPaso
                print("Move X: ",-(XPaso), 0)
                self.gui.updateStatus("Moving Down:{},{}".format(-(XPaso),0))
                publish.single("{}/Master".format(self.mqttControl.ID),
                                'movePosXY,{},{},{}'.format(self.gui.gr,-(XPaso),0),
                                hostname=self.mqttControl.brokerIP)
                self.gui.downB = False

        if self.gui.right == True:
            if (1<= self.gui.gr <= 8):
                #print("Moving to Pos: X:{}, Y:{}")
                #X,YObj tamaño de pasos
                XPaso, YPaso = self.createNodos(self.mqttControl.CalX1, self.mqttControl.CalY1)
                YObj = YPaso + int(self.mqttControl.Y1)
                print("Move Y: ",0, YObj)
                self.gui.updateStatus("Moving Right:{},{}".format(0,YPaso))
                publish.single("{}/Master".format(self.mqttControl.ID),
                                'movePosXY,{},{},{}'.format(self.gui.gr,0,YPaso),
                                hostname=self.mqttControl.brokerIP)
                self.gui.right = False

        if self.gui.left == True:
            if (1<= self.gui.gr <= 8):
                #print("Moving to Pos: X:{}, Y:{}")
                #X,YObj tamaño de pasos
                XPaso, YPaso = self.createNodos(self.mqttControl.CalX1, self.mqttControl.CalY1)
                YObj = int(self.mqttControl.Y1) - YPaso
                print("Move Y: ",0, YObj)
                self.gui.updateStatus("Moving Left:{},{}".format(0,-(Ypaso)))
                publish.single("{}/Master".format(self.mqttControl.ID),
                                'movePosXY,{},{},{}'.format(self.gui.gr,0,-(YPaso)),
                                hostname=self.mqttControl.brokerIP)
                self.gui.left = False

        if self.gui.iron == True:
            if (1<= self.gui.gr <= 8):
                print("IR ON: ")
                self.gui.updateStatus("IR ON")
                publish.single('{}/Grower{}'.format(self.ID,self.gui.gr),
                                'OnOut2',
                                hostname=self.mqttControl.brokerIP)
                self.gui.window["data_iron"].update(disabled=True)
                self.gui.window["data_iroff"].update(disabled=False)
                self.gui.iron = False

        if self.gui.iroff == True:
            if (1<= self.gui.gr <= 8):
                print("IR OFF")
                self.gui.updateStatus("IR OFF")
                publish.single('{}/Grower{}'.format(self.ID,self.gui.gr),
                                'OffOut2',
                                hostname=self.mqttControl.brokerIP)
                self.gui.window["data_iron"].update(disabled=False)
                self.gui.window["data_iroff"].update(disabled=True)
                self.gui.iroff = False
