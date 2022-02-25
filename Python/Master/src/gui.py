#!/usr/bin/env python3

# Import directories
import os
import PySimpleGUI as sg
from datetime import datetime
import time

class GUI:
    def __init__(self, ID = "99-999-999", logger = None, serialControl = None):
        # Logger
        self.log = logger
        # Serial controller
        self.ser = serialControl
        
        # Main Window
        self.window = None
        self.isOpen = True
        
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
        self.BORDER_COLOR = '#C7D5E0'
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
        hum = float("NaN")
        temp = float("NaN")
        ID = ID
        lastCommand = ""
        
        # List of devices
        self.devicesUSB = ['master', 'generalControl', 'motorsG1', 'motorsG2']
        self.devicesGrower = ['grower{}'.format(i+1) for i in range(4)]
        self.devicesESP = ['esp32front1', 'esp32front2', 'esp32center1', 'esp32center2', 'esp32back1', 'esp32back2']
        self.devicesConnected = []
        
        # List of levels
        self.levels = ['Piso {}'.format(i+1) for i in range(8)]
        # Checbox for devices
        CB1 = [sg.Checkbox(x, default = False, disabled= True, font='Any 10', key="CB1_{}".format(i)) for i, x in enumerate(self.devicesUSB)]
        CB2 = [sg.Checkbox(x, default = False, disabled= True, font='Any 10', key="CB2_{}".format(i)) for i, x in enumerate(y for y in self.devicesESP if '1' in y[-1])]
        CB3 = [sg.Checkbox(x, default = False, disabled= True, font='Any 10', key="CB3_{}".format(i)) for i, x in enumerate(y for y in self.devicesESP if '2' in y[-1])]
        CB4 = [sg.Checkbox(x, default = False, disabled= True, font='Any 10', key="CB4_{}".format(i)) for i, x in enumerate(self.devicesGrower)]
        
        top_banner = [[sg.Text(' '*31 + 'Grow Greens', font='Any 28')]]

        top  = [[sg.Text('ID: {}'.format(ID), font='Any 12', key='ID'), sg.Text(' '*27 + 'Estatus General', font='Any 20'), sg.Text(' '*33 + timestampStr, font='Any 12')],
                [sg.Text(' '*43 + 'Frío' + ' '*35 + 'Óptimo' + ' '*35 + 'Caliente' + ' '*38 + 'Seco' + ' '*35 + 'Óptimo' + ' '*35 + 'Húmedo', font='Any 8')],
                [sg.Text('Temp: {}°C'.format(temp), font='Any 12', key='temp'), sg.ProgressBar(100, size=(30, 10), key='tempBar', bar_color=("red", "white")),
                 sg.Text('HR: {}%'.format(hum), font='Any 12', key='hum'), sg.ProgressBar(100, size=(30, 10), key='humBar', bar_color=("blue", "white"))]]

        block_2 = [[sg.Text(' '*20 + 'Rutinas', font='Any 18')],
                    [sg.Text('Seleccione:', font='Any 12', key="rut"), sg.Combo(self.levels, font='Any 12'),
                     sg.Text('Estatus', font='Any 12'), sg.Input('No hay selección', size=(20,10), key="rut_status")],
                    [sg.Text(' '*25), sg.Button('Iniciar', key="data_start"), sg.Text(' '*10), sg.Button('Parar', key="data_stop")]]

        block_3 = [[sg.Text(' '*15 + 'Dispósitivos', font='Any 18')], CB1, CB2, CB3, CB4]

        block_4 = [[sg.Text(' '*18 + 'Comunicación', font='Any 18')],
                    [sg.Output(size=(60,10), key="stdout")],
                    [sg.Text('Último comando enviado:', font='Any 10'), sg.Text('{}'.format(lastCommand), font='Any 10', text_color='black', key="last_command")],
                    [sg.Text(' '*20 + 'Enviar a:', font='Any 10'), sg.Combo(self.devicesUSB, font='Any 12', key="select_device")],
                    [sg.Input(size=(50,10)), sg.Button('Enviar', key="send_data")]]

        self.layout = [[sg.Column(top_banner, size=(960, 60), pad=(0,0), background_color=self.black90)],
                  [sg.Column(top, size=(960, 120), pad=self.BPAD_TOP)],
                  [sg.Column([[sg.Column(block_2, size=(450,120), pad=self.BPAD_LEFT_INSIDE)],
                              [sg.Column(block_3, size=(450,180),  pad=self.BPAD_LEFT_INSIDE)]], pad=self.BPAD_LEFT, background_color=self.BORDER_COLOR),
                   sg.Column(block_4, size=(450, 320), pad=self.BPAD_RIGHT)]]
    
    def begin(self):
        self.window = sg.Window('Dashboard', self.layout, background_color=self.BORDER_COLOR, return_keyboard_events=True, finalize=True)
        self.window['CB1_0'].Update(value=True)
        self.temp_bar = self.window['tempBar']
        self.hum_bar = self.window['humBar']
        self.str2log('GUI started correctly', level = 1)
        
    def str2log(self, msg, level = 'DEBUG'):
        if self.log!=None:
            if (level==0 or level=='DEBUG'): self.log.debug(msg)
            elif (level==1 or level=='INFO'): self.log.info(msg)
            elif (level==2 or level=='WARNING'): self.log.warning(msg)
            elif (level==3 or level=='ERROR'): self.log.error(msg)
            elif (level==4 or level=='CRITICAL'): self.log.critical(msg)
        else:
            print(msg)
    
    # update to send serial messages to all devices
    def serialMsg(self, msg):
        if self.ser != None:
            self.ser.write(self.ser.generalControl, msg)
        else:
            self.str2log(msg, 3)
    
    def updateGeneral(self, temp, hum):
        self.window['temp'].Update(value='Temp: {}°C'.format(temp))
        self.window['hum'].Update(value='HR: {}%'.format(hum))
        
        minTemp = 10
        maxTemp = 40
        adjustTemp = (temp - minTemp)*100/(maxTemp-minTemp)
        
        minHum = 40
        maxHum = 100
        adjustHum = (hum - minHum)*100/(maxHum-minHum)
        
        self.temp_bar.UpdateBar(adjustTemp)
        self.hum_bar.UpdateBar(adjustHum)
 
    def updateDevicesConnected(self, values):
        connected = []
        for i, value in enumerate(self.devicesUSB):
            if(values['CB1_{}'.format(i)]): connected.append(value)
        for i, value in enumerate([x for x in self.devicesESP if '1' in x[-1]]):
            if(values['CB2_{}'.format(i)]): connected.append(value)
        for i, value in enumerate([x for x in self.devicesESP if '2' in x[-1]]):
            if(values['CB3_{}'.format(i)]): connected.append(value)
        for i, value in enumerate(self.devicesGrower):
            if(values['CB4_{}'.format(i)]): connected.append(value)
        if(len(connected)!=len(self.devicesConnected)): self.window['select_device'].Update(values=connected)
   
    # Needs to be called in an Event Loop
    def run(self):
        try:
            event, values = self.window.read(timeout=self.timeout, timeout_key='timeout')
            # Update connected devices every 15 seconds
            if time.time()-self.timer>15:
                self.timer = time.time()
                self.updateDevicesConnected(values)
            if event == sg.WIN_CLOSED:
                sg.popup('ERROR', 'No puedes hacer eso') # Handle error
                self.isOpen = False
            elif event == "Left:113": print("LEFT")
            elif event == "Right:114": print("RIGHT")
            
        except Exception as e:
            self.str2log("GUI Closed: {}".format(e), 2)
            self.isOpen = False
            #print(e)

# Debug
def main():
    gui = GUI()
    gui.begin()
    gui.updateGeneral(22, 70) # Set temperature and humidity data
    while gui.isOpen:
        gui.run()
        
if __name__ == '__main__':
    main()
