#!/usr/bin/env python3

# Import directories
import re
import os
import sys
import time
import PySimpleGUI as sg
from datetime import datetime
import paho.mqtt.publish as publish


class GUI:
    def __init__(self, systemID, brokerIP, mqttController, client):
         # Publish require varaibles
        self.ID = systemID
        self.IP = brokerIP
        # Define communication controllers
        self.mqttControl = mqttController
        # Aux Variables
        self.flag1 = False
        self.flagmove = False
        self.timer = time.time()
        self.timer2 = time.time()
        self.client = client
        self.mmToCount = 0

        # Main Window
        self.window = None
        self.isOpen = True

        self.gr = ''
        self.XObj = 0
        self.YObj = 0

        #GuiSymbols
        SYMBOL_UP =    '▲'
        SYMBOL_DOWN =  '▼'
        SYMBOL_RIGHT = '►'
        SYMBOL_LEFT = '◄'

        # Visuals
        # Colors
        self.black90 = '#3c3c3b'
        self.green1 = '#00783e'
        self.green2 = '#368f3f'
        self.green3 = '#00b44f'
        self.green4 = '#82bc00'
        self.disable_color = (self.black90, self.black90)
        self.BORDER_COLOR = '#000000'
        self.DARK_HEADER_COLOR = '#1B2838'
        # Padding
        self.BPAD_TOP = ((0,0), (10, 10))
        self.BPAD_LEFT = ((20,10), (0, 10))
        self.BPAD_LEFT_INSIDE = (0, 10)
        self.BPAD_RIGHT = ((10,20), (10, 20))

        # Add your new theme colors and settings
        sg.LOOK_AND_FEEL_TABLE['Dashboard'] = {'BACKGROUND': self.black90,
                                                'TEXT': 'white',
                                                'INPUT': '#d2d2d2',
                                                'TEXT_INPUT': 'black',
                                                'SCROLL': '#d2d2d2',
                                                'BUTTON': ('black', self.green1),
                                                'PROGRESS': ('white', self.green2),
                                                'BORDER': 3,'SLIDER_DEPTH': 3, 'PROGRESS_DEPTH': 3}
                                                #'BORDER': 1,'SLIDER_DEPTH': 0, 'PROGRESS_DEPTH': 0}
        sg.theme('Dashboard')

        # Tables variables
        actualDirectory = os.getcwd()
        if actualDirectory.endswith('src'): self.timeout = 200;
        else: self.timeout = 0;

        # Variables to update in Window
        timestampStr = datetime.now().strftime("%d-%b-%Y %H:%M")
        PosX = float("NaN")
        PosY = float("NaN")
        status = ''
        self.down = False
        self.down1 = False

        # List of devices
        self.devicesGrower = ['grower{}'.format(i+1) for i in range(4)]
        self.devicesConnected = []

        # List of levels
        self.levels = ['Piso {}'.format(i+1) for i in range(8)]
        top_banner = [[sg.Text(' '*25 + 'Grow Greens', font='Any 28')]]


        #top  = [sg.Image(r'C:\Users\User\Pictures\LOGOTIPO GROW GREENS-04.png')]

        block_2 = [[sg.Text(' '*5 + 'Grower Config', font='Any 18')],
                    [sg.Text('Seleccione:', font='Any 12', key="rut"), sg.Combo(self.levels, font='Any 12', key="pis")],
                    [sg.Text(' '*5), sg.Button('Sincronizar', key="data_sincronizar")],
                    [sg.Text(' ')],
                    [sg.Text('Status:  {}'.format(status), font='Any 12', key="rut_status")]]

        block_3 = [[sg.Text(' '*15 + 'Rutina Manual', font='Any 18')],
                   [sg.Text(' '*10 +'Pos Actual' + ' '*25 + 'Pos Obj', font='Any 14')],
                   [sg.Text(' '*5 +'X:  {}'.format(PosX), font='Any 12', key="posxA_status"), sg.Text(' '*35 +'X', font='Any 12'), sg.Input('PosXObj', size=(20,10), key="posxO_status")],
                   [sg.Text(' '*5 +'Y:  {}'.format(PosY), font='Any 12', key="posyA_status"), sg.Text(' '*36 +'Y', font='Any 12'), sg.Input('PosYObj', size=(20,10), key="posyO_status")],
                   [sg.Text(' '*39 + 'Nodo:  X', font='Any 12'), sg.Input('NoX', size=(5,10), key="nodo_x", disabled=True), sg.Text('Y', font='Any 12'), sg.Input('NoY', size=(5,10), key="nodo_y", disabled=True)],
                   [sg.Text('Control Direc'), sg.Button('Off', size=(4,1), button_color=('white', 'red'), key='_B_'), sg.Text(' '*3), sg.Button(SYMBOL_UP, key="data_up", disabled=True),sg.Text(' '*20), sg.Button('IR-ON', button_color=('white', 'green'), key="data_iron")],
                   [sg.Text(' '*30), sg.Button(SYMBOL_LEFT, key="data_left", disabled=True), sg.Text(' '*5), sg.Button(SYMBOL_RIGHT, key="data_right", disabled=True), sg.Text(' '*10), sg.Button('IR-OFF', button_color=('white', 'red'), key="data_iroff", disabled=True)],
                   [sg.Text(' '*40), sg.Button(SYMBOL_DOWN, key="data_down", disabled=True)],
                   [sg.Text(' '*10), sg.Button('Home', key="data_home"), sg.Text(' '*40), sg.Button('Mover', key="data_move")]]

        top = [[sg.Text('Rutina Automatica', font='Any 18'), sg.Button('Off', size=(4,1), button_color=('white', 'red'), key='_A_')],
                    [sg.Text(' '*5), sg.Button('Start Routine', disabled=True, key="data_AutoRoutine"), sg.Text(' '*10), sg.Button('Stop Routine', button_color=('white','black'), disabled=True, key="data_stopRoutine")]]
        '''
        self.layout = [[sg.Column(top_banner, size=(480, 60), pad=(0,0), background_color=self.black90)],
                  #[sg.Column(top, pad=self.BPAD_TOP)],
                  [sg.Column([[sg.Column(block_2, size=(450,120), pad=self.BPAD_LEFT_INSIDE)],
                               [sg.Column(block_3, size=(450,260),  pad=self.BPAD_LEFT_INSIDE)], [sg.Column(top, size=(450,90), pad=self.BPAD_TOP)]],
                              pad=self.BPAD_LEFT, background_color=self.BORDER_COLOR)]]

        '''
        self.layout = [[sg.Column(top_banner, size=(800, 60), pad=(0,0), background_color=self.black90)],
                  #[sg.Column(top, size=(450, 90), pad=self.BPAD_TOP)],
                  [sg.Column([[sg.Column(block_2, size=(320,220), pad=self.BPAD_LEFT_INSIDE)],
                              [sg.Column(top, size=(320,90),  pad=self.BPAD_LEFT_INSIDE)]], pad=self.BPAD_LEFT, background_color=self.BORDER_COLOR),
                   sg.Column(block_3, size=(420, 320), pad=self.BPAD_RIGHT)]]

    def begin(self):
        self.window = sg.Window('Dashboard', self.layout, background_color=self.BORDER_COLOR, return_keyboard_events=True, finalize=True)
        #self.str2log('GUI started correctly', level = 1)
        self.updateStatus("Sincroniza un piso(1,3,6,8)")
        self.window["data_home"].update(disabled=True)
        self.window["data_move"].update(disabled=True)
        self.window['_A_'].update(disabled=True)
        self.window['_B_'].update(disabled=True)
        self.window["data_iron"].update(disabled=True)

    # update to send serial messages to all devices
    def serialMsg(self, msg):
        if self.ser != None:
            self.ser.write(self.ser.generalControl, msg)
        else:
            self.str2log(msg, 3)

    def getFloor(self, values):
        self.gr = 0
        if values['pis'] == "Piso 1": self.gr = 1
        elif values['pis'] == "Piso 3": self.gr = 3
        elif values['pis'] == "Piso 6": self.gr = 6
        elif values['pis'] == "Piso 8": self.gr = 8
        elif self.gr == 0:
            print("ERROR:Seleccione un piso disponible")
            #sys.exit()

    def getInputXY(self, values):
        x1 = values['posxO_status']
        y1 = values['posyO_status']
        x2 = re.findall('[0-9]+', x1)
        y2 = re.findall('[0-9]+', y1)
        if (bool(x2)==False or bool(y2)==False):
            self.XObj = 0
            self.YObj = 0
        else:
            self.XObj = int(x2[0])
            self.YObj = int(y2[0])

    def getInputNode(self, values):
        x1 = values['nodo_x']
        y1 = values['nodo_y']
        x2 = re.findall('[0-9]+', x1)
        y2 = re.findall('[0-9]+', y1)
        if (bool(x2)==False or bool(y2)==False):
            self.XObj = 0
            self.YObj = 0
        else:
            self.XObj = int(x2[0])
            self.YObj = int(y2[0])

    def updatePos(self, PosX, PosY):
        self.window["posxA_status"].Update(value='X:  {}'.format(PosX))
        self.window["posyA_status"].Update(value='Y:  {}'.format(PosY))

    def updateStatus(self, status):
        self.window["rut_status"].Update(value='Status:  {}'.format(status))

    def createNodos(self, calX, calY):
        nodoX = int(calX)/41
        nodoY = int(calY)/10
        return nodoX, nodoY

    def whileMoving(self):
        mmToTime = self.mmToCount*0.030
        if ((time.time()-self.timer2>mmToTime) and (self.flagmove == True)):
            self.timer2 = time.time()
            publish.single("{}/Master".format(self.mqttControl.containerID),
                            'syncPosCloud,{}'.format(self.gr),
                             hostname=self.mqttControl.brokerIP)
            self.client.loop(0.2)
            posX = self.mqttControl.X1
            posY = self.mqttControl.Y1
            self.updatePos(posX, posY)
            print("Pos: ",posX, posY)
            self.window['_A_'].update(disabled=False)
            self.window['_B_'].update(disabled=False)
            self.window["data_home"].update(disabled=False)
            self.window["data_move"].update(disabled=False)
            self.window["data_iron"].update(disabled=False)
            self.window["data_iroff"].update(disabled=False)
            self.window["data_up"].update(disabled=False)
            self.window["data_down"].update(disabled=False)
            self.window["data_left"].update(disabled=False)
            self.window["data_right"].update(disabled=False)
            self.flagmove = False
            self.updateStatus("Ready to Go!")

        elif self.flagmove == True:
            self.updateStatus("Moving...")
            self.window['_A_'].update(disabled=True)
            self.window['_B_'].update(disabled=True)
            self.window["data_home"].update(disabled=True)
            self.window["data_move"].update(disabled=True)
            self.window["data_iron"].update(disabled=True)
            self.window["data_iroff"].update(disabled=True)
            self.window["data_up"].update(disabled=True)
            self.window["data_down"].update(disabled=True)
            self.window["data_left"].update(disabled=True)
            self.window["data_right"].update(disabled=True)
            self.window["data_AutoRoutine"].update(disabled=True)

    # Needs to be called in an Event Loop
    def run(self):
        self.whileMoving()
        #########FUNCION actualiza la posicion cada 3 segundos. Con timer
        """
        if (time()-self.timer2>1500 and self.flag1==True):
            self.timer2 = time.time()
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
        """
        ##########
        try:
            event, values = self.window.read(timeout=self.timeout, timeout_key='timeout')
            # Update connected devices every 15 seconds
            if time.time()-self.timer>15:
                self.timer = time.time()
                #self.updateDevicesConnected(values)
            if event == sg.WIN_CLOSED:
                sg.popup('ERROR', 'No puedes hacer eso') # Handle error
                self.isOpen = False
            elif event == "_B_":
                self.down = not self.down
                self.window['_B_'].Update(('Off','On')[self.down], button_color=(('white', ('red', 'green')[self.down])))
                self.window["data_up"].update(disabled=not(self.down))
                self.window["data_down"].update(disabled=not(self.down))
                self.window["data_right"].update(disabled=not(self.down))
                self.window["data_left"].update(disabled=not(self.down))
                self.window["nodo_x"].update(disabled=not(self.down))
                self.window["nodo_y"].update(disabled=not(self.down))
                self.window["posxO_status"].update(disabled=(self.down))
                self.window["posyO_status"].update(disabled=(self.down))
            elif event == "_A_":
                self.down1 = not self.down1
                self.window['_A_'].Update(('Off','On')[self.down1], button_color=(('white', ('red', 'green')[self.down1])))
                self.window["data_AutoRoutine"].update(disabled=not(self.down1))
                self.window["data_move"].update(disabled=self.down1)
                self.window["data_home"].update(disabled=self.down1)
                self.window["data_up"].update(disabled=(self.down1))
                self.window["data_down"].update(disabled=(self.down1))
                self.window["data_right"].update(disabled=(self.down1))
                self.window["data_left"].update(disabled=(self.down1))
                self.window["nodo_x"].update(disabled=(self.down1))
                self.window["nodo_y"].update(disabled=(self.down1))
                self.window["posxO_status"].update(disabled=(self.down1))
                self.window["posyO_status"].update(disabled=(self.down1))
                self.window['_B_'].update(disabled=(self.down1))
            elif event == "data_home":
                print("Home")
                self.getFloor(values)
                self.updateStatus("Moving To Home..")
                publish.single("{}/Master".format(self.mqttControl.containerID),
                                'home,{}'.format(self.gr),
                                hostname=self.mqttControl.brokerIP)
            elif event == "data_move":
                self.getFloor(values)
                if self.gr != 0 and self.down == False:
                    self.getInputXY(values)
                    self.updateStatus("Moving X: {}, Y: {}...".format(self.XObj, self.YObj))
                    publish.single("{}/Master".format(self.mqttControl.containerID),
                                'movePosXY,{},{},{}'.format(self.gr,self.XObj,self.YObj),
                                hostname=self.mqttControl.brokerIP)
                    print("Move: ", self.XObj, self.YObj)
                    self.flagmove = True
                    self.mmToCount = int(self.XObj)
                    self.timer2 = time.time()

                if self.gr != 0 and self.down == True:
                    self.getInputNode(values)
                    XPaso, YPaso = self.createNodos(self.mqttControl.CalX1, self.mqttControl.CalY1)
                    self.updateStatus("Moving X: {}, Y: {}...".format(self.XObj*XPaso, self.YObj*YPaso))
                    publish.single("{}/Master".format(self.mqttControl.containerID),
                                'movePosXY,{},{},{}'.format(self.gr,self.XObj*XPaso, self.YObj*YPaso),
                                hostname=self.mqttControl.brokerIP)
                    print("Move: ", self.XObj*XPaso, self.YObj*YPaso)
                    self.flagmove = True
                    self.mmToCount = int(self.XObj*XPaso)
                    self.timer2 = time.time()

            elif event == "data_sincronizar":
                self.getFloor(values)
                if self.gr != 0:
                    self.updateStatus("Sincronizando...")
                    publish.single("{}/Master".format(self.mqttControl.containerID),
                                    'syncCalCloud,{}'.format(self.gr),
                                    hostname=self.mqttControl.brokerIP)
                    self.client.loop(0.2)
                    calX = self.mqttControl.CalX1
                    calY = self.mqttControl.CalY1
                    if (int(calX) != 0 and int(calY) != 0):
                        print("CalXY Ready: ",calX, calY)
                        self.flag1=True
                        ###UpdatePos
                        publish.single("{}/Master".format(self.mqttControl.containerID),
                                        'syncPosCloud,{}'.format(self.gr),
                                        hostname=self.mqttControl.brokerIP)
                        self.client.loop(0.2)
                        posX = self.mqttControl.X1
                        posY = self.mqttControl.Y1
                        self.window["data_home"].update(disabled=False)
                        self.window["data_move"].update(disabled=False)
                        self.window['_A_'].update(disabled=False)
                        self.window['_B_'].update(disabled=False)
                        self.window["data_iron"].update(disabled=False)
                        #self.window["data_sincronizar"].update(disabled=True)
                        self.updateStatus("Ready to go!")
                        print("Sync Ready")

                    else:
                        print("Error, SyncCalXY")
                        self.updateStatus("ERROR: Sync, Try Again!")
                        #break
                else:
                    self.updateStatus("ERROR:Piso Incorrecto")
                    self.window['_A_'].update(disabled=True)
                    self.window['_B_'].update(disabled=True)
                    self.window["data_home"].update(disabled=True)
                    self.window["data_move"].update(disabled=True)
                    #break
            elif event == "data_up":
                if self.gr != 0:
                    #print("Moving to Pos: X:{}, Y:{}")
                    #X,YObj tamaño de pasos
                    XPaso, YPaso = self.createNodos(self.mqttControl.CalX1, self.mqttControl.CalY1)
                    XObj = XPaso + int(self.mqttControl.X1)
                    if XObj < float(self.mqttControl.CalX1):
                        print("Move X: ",XPaso, 0)
                        self.updateStatus("Moving UP:{},{}".format(XPaso,0))
                        publish.single("{}/Master".format(self.mqttControl.containerID),
                                        'movePosXY,{},{},{}'.format(self.gr,XPaso,0),
                                        hostname=self.mqttControl.brokerIP)
                        self.flagmove = True
                        self.mmToCount = int(XPaso)
                        self.timer2 = time.time()
            elif event == "data_down":
                if self.gr != 0:
                    #print("Moving to Pos: X:{}, Y:{}")
                    #X,YObj tamaño de pasos
                    XPaso, YPaso = self.createNodos(self.mqttControl.CalX1, self.mqttControl.CalY1)
                    XObj = int(self.mqttControl.X1) - XPaso
                    print("Move X: ",-(XPaso), 0)
                    self.updateStatus("Moving Down:{},{}".format(-(XPaso),0))
                    publish.single("{}/Master".format(self.mqttControl.containerID),
                                    'movePosXY,{},{},{}'.format(self.gr,-(XPaso),0),
                                    hostname=self.mqttControl.brokerIP)
                    self.flagmove = True
                    self.mmToCount = int(XPaso)
                    self.timer2 = time.time()
            elif event == "data_left":
                if self.gr != 0:
                    #print("Moving to Pos: X:{}, Y:{}")
                    #X,YObj tamaño de pasos
                    XPaso, YPaso = self.createNodos(self.mqttControl.CalX1, self.mqttControl.CalY1)
                    YObj = int(self.mqttControl.Y1) - YPaso
                    print("Move Y: ",0, -(YPaso))
                    self.updateStatus("Moving Left:{},{}".format(0,-(YPaso)))
                    publish.single("{}/Master".format(self.mqttControl.containerID),
                                    'movePosXY,{},{},{}'.format(self.gr,0,-(YPaso)),
                                    hostname=self.mqttControl.brokerIP)
                    self.flagmove = True
                    self.mmToCount = int(YPaso)
                    self.timer2 = time.time()
            elif event == "data_right":
                if self.gr != 0:
                    #print("Moving to Pos: X:{}, Y:{}")
                    #X,YObj tamaño de pasos
                    XPaso, YPaso = self.createNodos(self.mqttControl.CalX1, self.mqttControl.CalY1)
                    YObj = YPaso + int(self.mqttControl.Y1)
                    print("Move Y: ",0, YPaso)
                    self.updateStatus("Moving Right:{},{}".format(0,YPaso))
                    publish.single("{}/Master".format(self.mqttControl.containerID),
                                    'movePosXY,{},{},{}'.format(self.gr,0,YPaso),
                                    hostname=self.mqttControl.brokerIP)
                    self.flagmove = True
                    self.mmToCount = int(YPaso)
                    self.timer2 = time.time()
            elif event == "data_iron":
                if self.gr != 0:
                    print("IR ON: ")
                    self.updateStatus("IR ON")
                    publish.single('{}/Grower{}'.format(self.ID,self.gr),
                                    'OnOut2',
                                    hostname=self.mqttControl.brokerIP)
                    self.window["data_iron"].update(disabled=True)
                    self.window["data_iroff"].update(disabled=False)
            elif event == "data_iroff":
                if self.gr != 0:
                    print("IR OFF")
                    self.updateStatus("IR OFF")
                    publish.single('{}/Grower{}'.format(self.ID,self.gr),
                                    'OffOut2',
                                    hostname=self.mqttControl.brokerIP)
                    self.window["data_iron"].update(disabled=False)
                    self.window["data_iroff"].update(disabled=True)

            elif event == "data_AutoRoutine":
                if self.gr != 0:
                    print("AutoRoutine running..")
                    self.updateStatus("AutoRoutine running..")
                    publish.single('{}/Cloud'.format(self.ID),
                                    'StartRoutine,{}'.format(self.gr),
                                    hostname=self.mqttControl.brokerIP)
                    self.window["data_stopRoutine"].update(disabled=False)
                    self.flagmove = True
                    self.mmToCount = 160000
                    self.timer2 = time.time()

            elif event == "data_stopRoutine":
                print("StopRoutine")
                self.getFloor(values)
                self.updateStatus("Routine finished")
                publish.single("{}/Master".format(self.mqttControl.containerID),
                                'stopRoutine,{}'.format(self.gr),
                                hostname=self.mqttControl.brokerIP)
                self.window["data_stopRoutine"].update(disabled=True)
                self.flagmove = True
                self.mmToCount = 1
                self.timer2 = time.time()

        except Exception as e:
            #self.str2log("GUI Closed: {}".format(e), 2)
            self.isOpen = False
            #print(e)

# Debug
def main():
    gui = GUI()
    gui.begin()
    gui.updatePos(22, 70)
    while gui.isOpen:
        gui.run()

if __name__ == '__main__':
    main()
