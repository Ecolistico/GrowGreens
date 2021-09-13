import PySimpleGUI as sg
from datetime import datetime
import random
import time

# To add at class
datetimeObj = datetime.now()
timestampStr = datetimeObj.strftime("%d-%b-%Y %H:%M")
hum = 65
temp = 28.6
ID = "99-999-999"
lastCommand = "master,reboot"
devices = ['master', 'generalControl', 'motorsGrower1', 'motorsGrower2']

# Colors
black90 = '#3c3c3b'
green1 = '#00783e'
green2 = '#368f3f'
green3 = '#00b44f'
green4 = '#82bc00'
disable_color = (black90, black90)


theme_dict = {'BACKGROUND': black90,
                'TEXT': 'white',
                'INPUT': '#d2d2d2',
                'TEXT_INPUT': 'black',
                'SCROLL': '#d2d2d2',
                'BUTTON': ('black', green1),
                'PROGRESS': ('white', green2),
                'BORDER': 3,'SLIDER_DEPTH': 3, 'PROGRESS_DEPTH': 3}
                #'BORDER': 1,'SLIDER_DEPTH': 0, 'PROGRESS_DEPTH': 0}

# sg.theme_add_new('Dashboard', theme_dict)     # if using 4.20.0.1+
sg.LOOK_AND_FEEL_TABLE['Dashboard'] = theme_dict
sg.theme('Dashboard')

BORDER_COLOR = '#C7D5E0'
DARK_HEADER_COLOR = '#1B2838'
BPAD_TOP = ((0,0), (10, 10))
BPAD_LEFT = ((20,10), (0, 10))
BPAD_LEFT_INSIDE = (0, 10)
BPAD_RIGHT = ((10,20), (10, 20))

top_banner = [[sg.Text(' '*31 + 'Grow Greens', font='Any 28')]]

top  = [[sg.Text('ID: {}'.format(ID), font='Any 12'), sg.Text(' '*27 + 'Estatus General', font='Any 20'), sg.Text(' '*33 + timestampStr, font='Any 12')],
        [sg.Text(' '*43 + 'Frío' + ' '*35 + 'Óptimo' + ' '*35 + 'Caliente' + ' '*38 + 'Seco' + ' '*35 + 'Óptimo' + ' '*35 + 'Húmedo', font='Any 8')],
        [sg.Text('Temp: {}°C'.format(temp), font='Any 12'), sg.ProgressBar(100, size=(30, 10), key='tempBar', bar_color=("red", "white")),
         sg.Text('HR: {}%'.format(hum), font='Any 12'), sg.ProgressBar(100, size=(30, 10), key='humBar', bar_color=("blue", "white"))]]

connected = sg.Frame(layout=[
            [sg.Checkbox('Master', default=True)],
            [sg.Checkbox('generalControl', default=True)],
            [sg.Checkbox('motorsGrower1', default=True)],
            [sg.Checkbox('motorsGrower2', default=True)]], title='Conectados')

disconnected = sg.Frame(layout=[
            [sg.Checkbox('Master', default=True)],
            [sg.Checkbox('generalControl', default=False)],
            [sg.Checkbox('motorsGrower1', default=False)],
            [sg.Checkbox('motorsGrower2', default=False)]], title='Desconectados')

block_2 = [[sg.Text(' '*15 + 'Dispósitivos', font='Any 18')],
            [connected, disconnected]]

block_3 = [[sg.Text('Block 3', font='Any 18')],
            [sg.Input(), sg.Text('Some Text')],
            [sg.Button('Go'), sg.Button('Exit')]]


block_4 = [[sg.Text(' '*22 + 'Serial', font='Any 18')],
            [sg.Output(size=(60,10))],
            [sg.Text('Último comando enviado:', font='Any 10'), sg.Text('{}'.format(lastCommand), font='Any 10', text_color='black')],
            [sg.Text('Enviar a:', font='Any 10'), sg.Combo(devices, font='Any 12')],
            [sg.Input(size=(40,10)), sg.Button('Enviar'), sg.Button('Salir')]]


layout = [[sg.Column(top_banner, size=(960, 60), pad=(0,0), background_color=black90)],
          [sg.Column(top, size=(960, 120), pad=BPAD_TOP)],
          [sg.Column([[sg.Column(block_2, size=(450,150), pad=BPAD_LEFT_INSIDE)],
                      [sg.Column(block_3, size=(450,150),  pad=BPAD_LEFT_INSIDE)]], pad=BPAD_LEFT, background_color=BORDER_COLOR),
           sg.Column(block_4, size=(450, 320), pad=BPAD_RIGHT)]]

window = sg.Window('Dashboard', layout, background_color=BORDER_COLOR)
temp_bar = window['tempBar']
hum_bar = window['humBar']

while True:             # Event Loop
    event, values = window.read(timeout=10)
    if event == sg.WIN_CLOSED or event == 'Exit':
        #sg.popup('ERROR', 'No puedes hacer eso') # Handle error
        #time.sleep(10)
        break
    # update bar with loop value +1 so that bar eventually reaches the maximum
    temp_bar.UpdateBar(temp)
    hum_bar.UpdateBar(hum)
    
window.close()
