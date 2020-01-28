#!/usr/bin/env python
import os
import csv
import time
import PySimpleGUI as sg

# Pendiente integrar serialController to manage connection between devices
class GUI:
    def __init__(self, logger = None):
        # Logger
        self.log = logger
        # Colors
        self.black90 = '#3c3c3b'
        self.green1 = '#00783e'
        self.green2 = '#368f3f'
        self.green3 = '#00b44f'
        self.green4 = '#82bc00'

        # Tables variables
        self.filename = 'config.csv'
        self.data = []
        self.header_list = []

        # UI Variables
        self.cycleTime = 15
        self.evTime = 5
        self.piso = ''
        self.lado = ''
        self.etapa = ''
        self.solucion = ''

        # Aux variables
        self.total = 0

        # Main Window
        self.window = None

        # Add your new theme colors and settings
        sg.LOOK_AND_FEEL_TABLE['GrowGreens'] = {'BACKGROUND': self.black90,
                                                'TEXT': self.green4,
                                                'INPUT': '#d2d2d2',
                                                'TEXT_INPUT': 'black',
                                                'SCROLL': '#d2d2d2',
                                                'BUTTON': ('black', self.green1),
                                                'PROGRESS': ('white', self.green2),
                                                'BORDER': 3, 'SLIDER_DEPTH': 3, 'PROGRESS_DEPTH': 3,
                                                }
        # Switch to use your newly created theme
        sg.theme('GrowGreens')

    def str2log(self, msg, level = 'DEBUG'):
        if self.log!=None:
            if (level==0 or level=='DEBUG'): self.log.debug(msg)
            elif (level==1 or level=='INFO'): self.log.info(msg)
            elif (level==2 or level=='WARNING'): self.log.warning(msg)
            elif (level==3 or level=='ERROR'): self.log.error(msg)
            elif (level==4 or level=='CRITICAL'): self.log.critical(msg)
        else:
            print(msg)

    def getEVlimits(self, cycle, etapa):
        evNumber = 32
        minSec = 3
        factor = 0.0833*etapa+0.1567
        maxSec = int((cycle*60/evNumber)*factor)
        if maxSec < minSec: maxSec = minSec
        return minSec, maxSec

    def getTotalIrrigationTIme(self, dataSource):
        sum = [0, 0, 0, 0, 0]
        for i in range(len(dataSource)-1):
            for j in range(len(sum)):
                sum[j] += int(dataSource[i][j+1])
        return max(sum)

    def getEVvalue(self, piso, lado, etapa, solucion, dataSource):
        if(lado=='A'): ladoInt = 0
        else: ladoInt = 4
        if(solucion=='H2O'): sol = 1
        else: sol = int(solucion) + 1
        columna = ((piso-1)*8) + (etapa+ladoInt) - 1
        return dataSource[columna][sol]

    def changeEVvalue(self, piso, lado, etapa, solucion, dataSource, cycleTime, value):
        if(lado=='A'): ladoInt = 0
        else: ladoInt = 4
        if(solucion=='H2O'): sol = 1
        else: sol = int(solucion) + 1
        columna = ((piso-1)*8) + (etapa+ladoInt) - 1
        oldValue = dataSource[columna][sol]
        dataSource[columna][sol] = value
        if (self.getTotalIrrigationTIme(dataSource)<=(int(cycleTime)*30)):
            return True
        else:
            dataSource[columna][sol] = oldValue
            return False

    def rewriteCSV(self, file, header, dataSource):
        with open(file, mode='w', newline='') as outfile:
            writer = csv.writer(outfile)
            writer.writerow(header)
            writer.writerows(dataSource)

    def checkEV(self):
        if self.piso!='' and self.lado!='' and self.etapa!='' and self.solucion!='':
            evMin, evMax = self.getEVlimits(self.cycleTime, int(self.etapa))
            val = self.getEVvalue(int(self.piso), self.lado, int(self.etapa), self.solucion, self.data)
            self.window['evTime'].Update(range=(evMin, evMax))
            self.window['evTime'].Update(value=val)
        else:
            self.window['evTime'].Update(value=5, range=(5, 5))
            self.window['Actualizar'].Update(disabled=True)

    def begin(self):
         if self.filename is not None:
             with open(self.filename, "r") as infile:
                 reader = csv.reader(infile)
                 self.header_list = next(reader)
                 try:
                     self.data = list(reader)  # read everything else into a list of rows
                 except:
                     sg.popup_error('Error reading file')
                     return

         self.total = self.getTotalIrrigationTIme(self.data)
         self.cycleTime = self.data[-1][1]

         layout = [
             [sg.Text('Grow Greens', size=(
                 18, 1), justification='center', font=("Helvetica 30 bold"), relief=sg.RELIEF_RIDGE, border_width=5)],
             [sg.Text('_' * 66)],
             [sg.Frame(layout=[
                 [sg.Text(' '*50, key='Error1', text_color='red')],
                 [sg.Table(values=self.data,
                           key='Table',
                           headings=self.header_list,
                           max_col_width=25,
                           auto_size_columns=True,
                           justification='center',
                           num_rows=min(len(self.data), 8))],
                 [ sg.Text('Ciclo de riego (min):', size=(14, 1)),
                 sg.Slider(range=(int(self.total/30)+1, 20), key='cycleTime', orientation='h', size=(15, 20), default_value=self.cycleTime),
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
                  sg.Slider(range=(5, 5), orientation='h', key='evTime', size=(10, 10), default_value=self.evTime)],
                 [sg.Text(' '*80),
                  sg.Button('Actualizar', size=(8, 1), key="Actualizar", pad=(10,10), disabled=True)],
                 ], title='Configuración de riego', relief=sg.RELIEF_SUNKEN)],
             [sg.Text('_' * 66)],
         ]

         self.window = sg.Window('Main', layout,
                            auto_size_text=True, finalize=True)

    def run(self):
        event, values = self.window.read(timeout=0, timeout_key='timeout')

        if int(values['cycleTime']) != self.cycleTime:
            self.cycleTime = int(values['cycleTime'])
            if(self.data[-1][1] == str(self.cycleTime)): self.window['Actualizar Ciclo'].Update(disabled=True)
            else: self.window['Actualizar Ciclo'].Update(disabled=False)
            if self.piso!='' and self.lado!='' and self.etapa!='' and self.solucion!='':
                evMin, evMax = self.getEVlimits(self.cycleTime, int(self.etapa))
            else: self.window['evTime'].Update(range=(5, 5))

        if int(values['evTime']) != self.evTime:
            self.evTime = int(values['evTime'])
            if self.piso!='' and self.lado!='' and self.etapa!='' and self.solucion!='':
                ev = self.getEVvalue(int(self.piso), self.lado, int(self.etapa), self.solucion, self.data)
                if (int(ev) == self.evTime): self.window['Actualizar'].Update(disabled=True)
                else: self.window['Actualizar'].Update(disabled=False)

        if values['Piso'] != self.piso:
            self.piso = values['Piso']
            self.checkEV()

        if values['Lado'] != self.lado:
            self.lado = values['Lado']
            self.checkEV()

        if values['Etapa'] != self.etapa:
            self.etapa = values['Etapa']
            self.checkEV()

        if values['Sol'] != self.solucion:
            self.solucion = values['Sol']
            self.checkEV()

        if event == 'Actualizar':
            resp = self.changeEVvalue(int(self.piso), self.lado, int(self.etapa), self.solucion, self.data, self.cycleTime, self.evTime)
            if(resp):
                self.window['Table'].Update(values=self.data)
                self.window['Actualizar'].Update(disabled=True)
                self.total = self.getTotalIrrigationTIme(self.data)
                self.window['cycleTime'].Update(range=(int(self.total/30)+1, 20))
                self.rewriteCSV(self.filename, self.header_list, self.data)
                self.str2log("Update Success")
            else: self.str2log("Update Failed")

        if event == 'Actualizar Ciclo':
            self.total = self.getTotalIrrigationTIme(self.data)
            if self.total<=(int(self.cycleTime)*30):
                self.data[-1][1] = str(self.cycleTime)
                self.window['Table'].Update(values=self.data)
                self.window['Actualizar Ciclo'].Update(disabled=True)
                self.rewriteCSV(self.filename, self.header_list, self.data)
                if self.piso!='' and self.lado!='' and self.etapa!='' and self.solucion!='':
                    val = self.getEVvalue(int(self.piso), self.lado, int(self.etapa), self.solucion, self.data)
                    self.window['evTime'].Update(range=(evMin, evMax))
                self.str2log("Update Cycle Success")
            else:
                self.str2log("Update Cycle Failed")

        if event in ('Quit', None):
            self.str2log("QUIT")
            self.window.close()


def main():
    gui = GUI()
    gui.begin()
    while True:
        gui.run()

if __name__ == '__main__':
    main()
