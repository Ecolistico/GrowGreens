#!/usr/bin/env python
import csv
import time
import PySimpleGUI as sg
from datetime import datetime

# Colors
black90 = '#3c3c3b'
green1 = '#00783e'
green2 = '#368f3f'
green3 = '#00b44f'
green4 = '#82bc00'

# Add your new theme colors and settings
sg.LOOK_AND_FEEL_TABLE['GrowGreens'] = {'BACKGROUND': black90,
                                        'TEXT': green4,
                                        'INPUT': '#d2d2d2',
                                        'TEXT_INPUT': 'black',
                                        'SCROLL': '#d2d2d2',
                                        'BUTTON': ('black', green1),
                                        'PROGRESS': ('white', green2),
                                        'BORDER': 3, 'SLIDER_DEPTH': 3, 'PROGRESS_DEPTH': 3,
                                        }
# Switch to use your newly created theme
sg.theme('GrowGreens')

def getEVlimits(cycle, etapa):
    evNumber = 32
    minSec = 3
    factor = 0.0833*etapa+0.1567
    maxSec = int((cycle*60/evNumber)*factor)
    if maxSec < minSec: maxSec = minSec
    return minSec, maxSec

def getTotalIrrigationTIme(data):
    sum = [0, 0, 0, 0, 0]
    for i in range(len(data)-1):
        for j in range(len(sum)):
            sum[j] += int(data[i][j+1])
    return max(sum)

def getEVvalue(piso, lado, etapa, solucion, data):
    if(lado=='A'): ladoInt = 0
    else: ladoInt = 4
    if(solucion=='H2O'): sol = 1
    else: sol = int(solucion) + 1
    columna = ((piso-1)*8) + (etapa+ladoInt) - 1
    return data[columna][sol]

def changeEVvalue(piso, lado, etapa, solucion, data, cycleTime, value):
    if(lado=='A'): ladoInt = 0
    else: ladoInt = 4
    if(solucion=='H2O'): sol = 1
    else: sol = int(solucion) + 1
    columna = ((piso-1)*8) + (etapa+ladoInt) - 1
    oldValue = data[columna][sol]
    data[columna][sol] = value
    if (getTotalIrrigationTIme(data)<=(int(cycleTime)*30)):
        return True
    else:
        data[columna][sol] = oldValue
        return False

def GUI():
     # UI Variables
     cycleTime = 15
     evTime = 5
     piso = ''
     lado = ''
     etapa = ''
     solucion = ''

     # Tables variables
     filename = r'C:\Users\Chema\Documents\Libro1.csv'
     data = []
     header_list = []
     button = 'Yes'

     if filename is not None:
         with open(filename, "r") as infile:
             reader = csv.reader(infile)
             if button == 'Yes':
                 header_list = next(reader)
             try:
                 data = list(reader)  # read everything else into a list of rows
                 if button == 'No':
                     header_list = ['column' + str(x) for x in range(len(data[0]))]
             except:
                 sg.popup_error('Error reading file')
                 return

     total = getTotalIrrigationTIme(data)

     layout = [
         [sg.Text('Grow Greens', size=(
             18, 1), justification='center', font=("Helvetica 30 bold"), relief=sg.RELIEF_RIDGE, border_width=5)],
         [sg.Text('_' * 66)],
         [sg.Frame(layout=[
             [sg.Text(' '*50, key='Error1', text_color='red')],
             [sg.Table(values=data,
                       key='Table',
                       headings=header_list,
                       max_col_width=25,
                       auto_size_columns=True,
                       justification='center',
                       num_rows=min(len(data), 8))],
             [ sg.Text('Ciclo de riego (min):', size=(14, 1)),
             sg.Slider(range=(int(total/30)+1, 20), key='cycleTime', orientation='h', size=(15, 20), default_value=cycleTime),
             sg.Button('Actualizar Ciclo', size=(8, 2), key="Actualizar Ciclo", pad=(10,10), disabled=True)],
             [sg.Text('Piso:'),
              sg.Text('Lado:'),
              sg.Text('Etapa:'),
              sg.Text('Solución:'),
              sg.Text(' '*40, key='Error2', text_color='red')],
             [sg.Combo(('', '1', '2', '3', '4'), key="Piso", default_value='', size=(4, 1)),
              sg.Combo(('', 'A', 'B'), key="Lado", default_value='', size=(4, 1)),
              sg.Combo(('', '1', '2', '3', '4'), key="Etapa", default_value='', size=(4, 1)),
              sg.Combo(('', 'H2O', '1', '2', '3', '4'), key="Sol", default_value='', size=(4, 1)),
              sg.Text('Tiempo (s)'),
              sg.Slider(range=(5, 5), orientation='h', key='evTime', size=(10, 10), default_value=evTime)],
             [sg.Text(' '*80),
              sg.Button('Actualizar', size=(8, 1), key="Actualizar", pad=(10,10), disabled=True)],
             ], title='Configuración de riego', relief=sg.RELIEF_SUNKEN)],
         [sg.Text('_' * 66)],
     ]

     window = sg.Window('Main', layout,
                        auto_size_text=True, finalize=True)

     #
     # Some place later in your code...
     # You need to perform a ReadNonBlocking on your window every now and then or
     # else it won't refresh.
     #
     # your program's main loop
     second = 0
     while True:
         now = datetime.now()
         # This is the code that reads and updates your window
         event, values = window.read(timeout=0, timeout_key='timeout')

         if int(values['cycleTime']) != cycleTime:
             cycleTime = int(values['cycleTime'])
             if(data[-1][1] == str(cycleTime)): window['Actualizar Ciclo'].Update(disabled=True)
             else: window['Actualizar Ciclo'].Update(disabled=False)
             if piso!='' and lado!='' and etapa!='' and solucion!='':
                 evMin, evMax = getEVlimits(cycleTime, int(etapa))
             else: window['evTime'].Update(range=(5, 5))

         if int(values['evTime']) != evTime:
             evTime = int(values['evTime'])
             if piso!='' and lado!='' and etapa!='' and solucion!='':
                 ev = getEVvalue(int(piso), lado, int(etapa), solucion, data)
                 if (int(ev) == evTime): window['Actualizar'].Update(disabled=True)
                 else: window['Actualizar'].Update(disabled=False)

         if values['Piso'] != piso:
             piso = values['Piso']
             if piso!='' and lado!='' and etapa!='' and solucion!='':
                 evMin, evMax = getEVlimits(cycleTime, int(etapa))
                 val = getEVvalue(int(piso), lado, int(etapa), solucion, data)
                 window['evTime'].Update(range=(evMin, evMax))
                 window['evTime'].Update(value=val)
             else:
                 window['evTime'].Update(value=5, range=(5, 5))
                 window['Actualizar'].Update(disabled=True)

         if values['Lado'] != lado:
             lado = values['Lado']
             if piso!='' and lado!='' and etapa!='' and solucion!='':
                 evMin, evMax = getEVlimits(cycleTime, int(etapa))
                 val = getEVvalue(int(piso), lado, int(etapa), solucion, data)
                 window['evTime'].Update(range=(evMin, evMax))
                 window['evTime'].Update(value=val)
             else:
                 window['evTime'].Update(value=5, range=(5, 5))
                 window['Actualizar'].Update(disabled=True)

         if values['Etapa'] != etapa:
             etapa = values['Etapa']
             if piso!='' and lado!='' and etapa!='' and solucion!='':
                evMin, evMax = getEVlimits(cycleTime, int(etapa))
                val = getEVvalue(int(piso), lado, int(etapa), solucion, data)
                window['evTime'].Update(range=(evMin, evMax))
                window['evTime'].Update(value=val)
             else:
                 window['evTime'].Update(value=5, range=(5, 5))
                 window['Actualizar'].Update(disabled=True)

         if values['Sol'] != solucion:
             solucion = values['Sol']
             if piso!='' and lado!='' and etapa!='' and solucion!='':
                 evMin, evMax = getEVlimits(cycleTime, int(etapa))
                 val = getEVvalue(int(piso), lado, int(etapa), solucion, data)
                 window['evTime'].Update(range=(evMin, evMax))
                 window['evTime'].Update(value=val)
             else:
                 window['evTime'].Update(value=5, range=(5, 5))
                 window['Actualizar'].Update(disabled=True)

         if event == 'Actualizar':
             resp = changeEVvalue(int(piso), lado, int(etapa), solucion, data, cycleTime, evTime)
             if(resp):
                 window['Table'].Update(values=data)
                 window['Actualizar'].Update(disabled=True)
                 total = getTotalIrrigationTIme(data)
                 window['cycleTime'].Update(range=(int(total/30)+1, 20))
                 print("Update Success")
             else: print("Update Failed")

         if event == 'Actualizar Ciclo':
             total = getTotalIrrigationTIme(data)
             if total<=(int(cycleTime)*30):
                 data[-1][1] = str(cycleTime)
                 window['Table'].Update(values=data)
                 window['Actualizar Ciclo'].Update(disabled=True)
                 if piso!='' and lado!='' and etapa!='' and solucion!='':
                     val = getEVvalue(int(piso), lado, int(etapa), solucion, data)
                     window['evTime'].Update(range=(evMin, evMax))
                 print("Update Cycle Succes")
             else:
                 print("Update Cycle Failed")
         if event != 'timeout':
             print(event, values)
         if event in ('Quit', None):
             break
         """
         DEBUG
         if(second!=now.second):
             second = now.second
             print('Another second')
             print(values['cycleTime'])
             print(cycleTime)
         """
     window.close()

def main():
    GUI()
    #sg.popup('Grow Greens finished')

if __name__ == '__main__':
    main()
