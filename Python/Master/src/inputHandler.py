#!/usr/bin/env python3

# Import directories
import sys
import select
from time import time
from pynput import keyboard
import paho.mqtt.publish as publish

class inputHandler:
    def __init__(self, systemID, brokerIP, logger, serialController, mqttController, gui):
        # Publish require varaibles
        self.ID = systemID
        self.IP = brokerIP
        # Define logger
        self.log = logger
        # Define communication controllers
        self.serialControl = serialController
        self.mqttControl = mqttController
        # Define connection with gui
        self.gui = gui
        # Aux Variables
        self.exit = False
        self.timer = time()
        self.commands = []
        self.commandIndex = -1
        self.auxFlag = False
        
        # Non-blocking keyboard listener:
        listener = keyboard.Listener(on_press=self.on_press)
        listener.start()
    
    def on_press(self, key):
        try:
            if key.char: pass
        except AttributeError:
            if(key.name == "up") and len(self.commands)>0:
                self.timer = time()
                self.auxFlag = True
                self.commandIndex += 1
                if self.commandIndex == len(self.commands): self.commandIndex = 0
            elif(key.name == "down") and len(self.commands)>0:
                self.timer = time()
                self.auxFlag = True
                self.commandIndex -= 1
                if self.commandIndex <= -1: self.commandIndex = len(self.commands)-1
            
    # Get an Input Line it consumes between 20-30% of the processor capacity
    def getLine(self, Block=False):
      if Block or select.select([sys.stdin], [], [], 0) == ([sys.stdin], [], []):
          return input()
    
    def Msg2Log(self, mssg):
        if(self.log!=None):
            if(mssg.startswith("debug,")): self.log.debug(mssg.split(",", 1)[1])
            elif(mssg.startswith("info,")): self.log.info(mssg.split(",", 1)[1])
            elif(mssg.startswith("warning,")): self.log.warning(mssg.split(",", 1)[1])
            elif(mssg.startswith("error,")): self.log.error(mssg.split(",", 1)[1])
            elif(mssg.startswith("critical,")): self.log.critical(mssg.split(",", 1)[1])
            else: self.log.debug(mssg)
        else: print(mssg)
            
    def writeGC(self, mssg): # Write in generalControl
        if self.serialControl.gcIsConnected: self.serialControl.write(self.serialControl.generalControl, mssg)
        else: self.Msg2Log("error,Cannot write to serial device [generalControl]. It is disconnected.")
            
    def writeMG1(self, mssg): # Write in motorsGrower1
        if self.serialControl.mg1IsConnected: self.serialControl.write(self.serialControl.motorsGrower1, mssg)
        else: self.Msg2Log("error,Cannot write to serial device [motorsGrower1]. It is disconnected.")
            
    def writeMG2(self, mssg): # Write in motorsGrower1
        if self.serialControl.mg2IsConnected: self.serialControl.write(self.serialControl.motorsGrower2, mssg)
        else: self.Msg2Log("error,Cannot write to serial device [motorsGrower2]. It is disconnected.")
            
    def valInteger(self, integer):
        try:
            val = int(integer)
            return True
        except:
            self.Msg2Log("error,inputHandler- {} is not an integer".format(integer))
            return False
    
    def valGreater0(self, number):
        if(number>0): return True
        else:
            self.Msg2Log("error,inputHandler- {} is not greater than 0".format(number))
            return False
    
    def valLenList(self, myList, reqLen):
        if(len(myList)==reqLen): return True
        else:
            self.Msg2Log("error,inputHandler- {} does not have the correct length".format(myList))
            return False
    
    def valLenList1(self, myList, minLen):
        if(len(myList)>=minLen): return True
        else:
            self.Msg2Log("error,inputHandler- {} does not have the minimun length".format(myList))
            return False
        
    def valSplit(self, strLine):
        splitLine = strLine.split(",")
        if(len(splitLine)>1):
            return splitLine
        else: self.Msg2Log("error,inputHandler- {} needs more arguments".format(strLine))
    
    def handleGUI(self, command, parameters):
        if self.gui.isOpen:
            if(command.startswith("solenoid,setCycleTime,")):
                try:
                    val = parameters[-1]
                    self.gui.total = self.gui.getTotalIrrigationTime(self.gui.data)
                    if self.gui.total<=(int(val)*60):
                        self.gui.cycleTime = int(val)
                        self.gui.data[-1][1] = str(self.gui.cycleTime)
                        self.gui.window['cycleTime'].Update(value=self.gui.cycleTime)
                        self.gui.window['Actualizar Ciclo'].Update(button_color=self.gui.disable_color, disabled=True)
                        self.gui.rewriteCSV(self.gui.filename, self.gui.header_list, self.gui.data)
                        if self.gui.piso!='' and self.gui.lado!='' and self.gui.etapa!='' and self.gui.solucion!='':
                            evMin, evMax = self.gui.getEVlimits(self.gui.cycleTime, int(self.gui.etapa))
                            self.gui.window['evTime'].Update(range=(evMin, evMax))
                            self.gui.window['TiempoCiclo'].Update(self.gui.data[-1][1]+' min')
                    else: self.Msg2Log("error,inputHandler Error: GUI not allow changes")
                except Exception as e: self.Msg2Log("error,inputHandler Error: Connecction with GUI failed [{}]".format(e))
                        
            elif(command.startswith("solenoid,setTimeOn,") and len(parameters)>=8):
                try:
                    floor = int(parameters[4])+1
                    region = int(parameters[5])
                    if(region<4):
                        side = 'A'
                        region += 1
                    else:
                        side = 'B'
                        region -= 3
                    sol = int(parameters[6])
                    if sol == 0: solution = 'H2O'
                    else: solution = str(sol)
                    val = parameters[7]
                    resp = self.gui.changeEVvalue(floor, side, region, solution,
                                                  self.gui.data, self.gui.cycleTime, int(val), factor=1)
                    if(resp):
                        if(floor==int(self.gui.piso) and side==self.gui.lado and region==int(self.gui.etapa) and solution==self.gui.solucion):
                            self.gui.evTime = int(val)
                            self.gui.window['evTime'].Update(value=self.gui.evTime)
                        self.gui.window['Actualizar'].Update(button_color=self.gui.disable_color, disabled=True)
                        self.gui.total = self.gui.getTotalIrrigationTime(self.gui.data)
                        self.gui.window['cycleTime'].Update(range=(int(self.gui.total/30)+1, 20))
                        self.gui.rewriteCSV(self.gui.filename, self.gui.header_list, self.gui.data)
                        self.gui.updateCurrentTimeValues()
                    else: self.Msg2Log("error,inputHandler Error: GUI not allow changes")
                except Exception as e: self.Msg2Log("error,inputHandler Error: Connecction with GUI failed [{}]".format(e))
    
    def handleInput(self, line):      
        if(line.lower()=="exit"):
            self.Msg2Log("Exit command activated")
            self.exit = True
            
        elif(line.startswith("raw")):
            param = self.valSplit(line)
            if(param!=None):
                if(param[1]=="generalControl" and self.valLenList1(param, 3) and self.serialControl.gcIsConnected):
                    cmd = ",".join(param[2:])
                    self.writeGC(cmd)
                    self.Msg2Log("info,inputHandler-[generalControl] Raw Command={}".format(cmd))
                    self.handleGUI(cmd, param)
                elif(param[1]=="motorsGrower1" and self.valLenList1(param, 3) and self.serialControl.mg1IsConnected):
                    cmd = ",".join(param[2:])
                    self.writeMG1(cmd)
                    self.Msg2Log("info,inputHandler-[motorsGrower1] Raw Command={}".format(cmd)) 
                elif(param[1]=="motorsGrower2" and self.valLenList1(param, 3) and self.serialControl.mg2IsConnected):
                    cmd = ",".join(param[2:])
                    self.writeMG2(cmd)
                    self.Msg2Log("info,inputHandler-[motorsGrower2] Raw Command={}".format(cmd)) 
                elif( (param[1]=="esp32" or param[1]=="Grower") and self.valLenList1(param, 4)):
                    cmd = ",".join(param[3:])
                    topic = "{}/{}{}".format(self.mqttControl.ID, param[1], param[2])
                    try:
                        publish.single(topic, "{}".format(cmd), hostname = self.mqttControl.brokerIP)
                        self.Msg2Log("info,inputHandler-[mqtt] Raw Command Topic={} Message={}".format(
                            topic,cmd))
                    except Exception as e:
                        self.Msg2Log("error,LAN/WLAN not found- Impossible use publish() [{}]".format(e))
                else: self.Msg2Log("info,inputHandler- {} Command Unknown".format(line))
        
        elif(line.startswith("importEeprom")):
            self.Msg2Log("info,Importing eeprom to generalControl")
            self.serialControl.importEeprom = 0
            self.serialControl.writeEeprom()
        
        elif(line.startswith("\033[A")): pass # Key UP print("UP")
        elif(line.startswith("\033[B")): pass # Key DOWN print("DOWN")
        elif(line.startswith("\033[C")): pass # Key RIGHT print("RIGHT")
        elif(line.startswith("\033[D")): pass # Key LEFT print("LEFT")
        
        else: self.Msg2Log("error,inputHandler- {} Command Unknown".format(line))
    
    def loop(self):
        if time()-self.timer > 1 and self.commandIndex!=-1 and self.auxFlag:
            self.auxFlag = False
            self.Msg2Log("info,inputHandler- Click enter if you want to send [{}]".format(self.commands[self.commandIndex]))
        elif(time()-self.timer > 5 and self.commandIndex!=-1):
            self.commandIndex = -1
            self.Msg2Log("warning,inputHandler- Timeout limit reached to send command with the arrows keys")
        
        line = self.getLine()
        if line!=None:
            if self.commandIndex!=-1:
                line = self.commands[self.commandIndex]
                self.commandIndex = -1
            repeated = False
            specialKey = False
            for com in self.commands:
                if line == com:
                    repeated = True
            if line.startswith("\033[A") or line.startswith("\033[B") or line.startswith("\033[C") or line.startswith("\033[D"): specialKey = True
            if not repeated and not specialKey:
                self.commands.append(line)
                if len(self.commands)>10: self.commands.pop(0)
            self.handleInput(line)

def main():
    from gui import GUI
    
    myGui = GUI()
    myGui.begin()
    inputControl = inputHandler("99-999-999", "192.168.6.10", None, None, None, myGui)
        
    while True:
        inputControl.loop()
        if myGui.isOpen: myGui.run()
        
if __name__ == "__main__":
    main()