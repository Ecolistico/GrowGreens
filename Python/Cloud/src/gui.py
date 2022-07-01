#!/usr/bin/env python3

# Import directories
import os
import PySimpleGUI as sg
from datetime import datetime
import time
import sys
import re

class GUI:
    def __init__(self, ID = "99-999-999"):
        # Main Window
        self.window = None
        self.isOpen = True

        self.gr = ''
        self.home = False
        self.move = False
        self.sync = False
        self.up = False
        self.downB = False
        self.left = False
        self.right = False
        self.iron = False
        self.iroff = False
        self.XObj = 0
        self.YObj = 0

        #GuiSymbols
        SYMBOL_UP =    '▲'
        SYMBOL_DOWN =  '▼'
        SYMBOL_RIGHT = '►'
        SYMBOL_LEFT = '◄'

        # Timer
        self.timer = time.time()

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
        ID = ID
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
                    [sg.Text(' '*5), sg.Button('Sincronizar', key="data_sincronizar"), sg.Text(' '*20), sg.Button('Calibrar', key="data_calibrar")],
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

    def updatePos(self, PosX, PosY):
        self.window["posxA_status"].Update(value='X:  {}'.format(PosX))
        self.window["posyA_status"].Update(value='Y:  {}'.format(PosY))

    def updateStatus(self, status):
        self.window["rut_status"].Update(value='Status:  {}'.format(status))

    # Needs to be called in an Event Loop
    def run(self):
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
                self.window["data_stopRoutine"].update(disabled=not(self.down1))
                self.window["data_home"].update(disabled=self.down1)
            elif event == "data_home":
                print("Home")
                self.getFloor(values)
                self.home = True
            elif event == "data_move":
                print("Move")
                self.getFloor(values)
                self.move = True
                self.getInputXY(values)
            elif event == "data_sincronizar":
                print("Sync")
                self.getFloor(values)
                self.sync = True
            elif event == "data_up":
                print("moveUp")
                self.up = True
            elif event == "data_down":
                print("moveDown")
                self.downB = True
            elif event == "data_left":
                print("moveLeft")
                self.left = True
            elif event == "data_right":
                print("moveRight")
                self.right = True
            elif event == "data_iron":
                self.iron = True
            elif event == "data_iroff":
                self.iroff = True

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
