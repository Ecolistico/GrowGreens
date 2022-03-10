#!/usr/bin/env python3

# Import directories
from time import time
import paho.mqtt.publish as publish
import ESPdata

class mqttController:
    def __init__(self,
                 ID,
                 brokerIP,
                 connector,
                 multiGrower,
                 logger):
        # Define container parameters
        self.ID = ID
        self.brokerIP = brokerIP
        self.conn = connector
        # Define loggers
        self.logMain = logger.logger
        self.logGr = []
        for i in range(len(logger.logger_grower)):  self.logGr.append(logger.logger_grower[i])
        self.logger_esp32 = []
        for i in range(len(logger.logger_esp32)): self.logger_esp32.append(logger.logger_esp32[i])
        self.logAirPrincipal = logger.logger_AirPrincipal
        self.logAirReturn = logger.logger_AirReturn
        # Define ESP32´s object
        self.ESP32 = ESPdata.multiESP(self.logger_esp32)
        # Define multiGrower´s object
        self.mGrower = multiGrower
        # Define AirPrincipal and AirReturn connected variables
        self.AirConnected = {'Principal': {'status': False, 'counter': 0}, 'Return': {'status': False, 'counter': 0}}
        # Define aux variables
        self.clientConnected = False
        self.actualTime = time()
        
    def update(self, ID, brokerIP, connector):
        self.ID = ID
        self.brokerIP = brokerIP
        self.conn = connector
    
    def Msg2Log(self, logger, mssg):
        if(mssg.split(",")[1]=="debug"): logger.debug(mssg.split(",")[0])
        elif(mssg.endswith(",debug")): logger.debug(mssg.replace(",debug", ""))
        elif(mssg.split(",")[1]=="info"): logger.info(mssg.split(",")[0])
        elif(mssg.endswith(",info")): logger.info(mssg.replace(",info", ""))
        elif(mssg.split(",")[1]=="warning"): logger.warning(mssg.split(",")[0])
        elif(mssg.endswith(",warning")): logger.warning(mssg.replace(",warning", ""))
        elif(mssg.split(",")[1]=="error"): logger.error(mssg.split(",")[0])
        elif(mssg.endswith(",error")): logger.error(mssg.replace(",error", ""))
        elif(mssg.split(",")[1]=="critical"): logger.critical(mssg.split(",")[0])
        elif(mssg.endswith(",critical")): logger.critical(mssg.replace(",critical", ""))
        else: logger.debug(mssg)
        
    # Callback fires when conected to MQTT broker.
    def on_connect(self, client, userdata, flags, rc):
        Topic = "{}/#".format(self.ID)
        message = "MQTT"
        if(rc == 0):
            message += " Connection succesful"
            mssg = "Master connected"
            client.subscribe(Topic)
            self.logMain.info(message)
            self.logMain.debug("Subscribed topic= {}".format(Topic))
        else:
            message += " Connection refused"
            if(rc == 1): message += " - incorrect protocol version"
            elif(rc == 2): message += " - invalid client identifier"
            elif(rc == 3): message += " - server unavailable"
            elif(rc == 4): message += " - bad username or password"
            elif(rc == 5): message += " - not authorised"
            else: message += " - currently unused"
            self.logMain.warning(message)

    # Callback fires when a published message is received.
    def on_message(self, client, userdata, msg):
        top = msg.topic # Input Topic
        message = msg.payload.decode("utf-8") # Input message
        device = top.split("/")[1] # Device where come
        
        # Get MQTT logs from all the devices
        if(top.endswith("log")):
            if(device.startswith("Grower")):
                level = int(device[-1]) - 1
                self.Msg2Log(self.logGr[level], message)
                self.mGrower.Gr[level].connected = True  
                if(message.startswith("cozir")): self.mGrower.Gr[level].str2array(message)
            elif(device == "esp32AirPrincipal"): 
                self.logAirPrincipal.debug(message)
                self.AirConnected['Principal']['status'] = True
            elif(device == "esp32AirReturn"): 
                self.logAirReturn.debug(message)
                self.AirConnected['Return']['status'] = True
            elif(device.startswith("esp32")):
                level = int(device[-1]) - 1
                pos = device[5:-1]
                if pos == "front": self.logger_esp32[level][0].debug(message)
                elif pos == "center": self.logger_esp32[level][1].debug(message)
                elif pos == "back": self.logger_esp32[level][2].debug(message)
            else: self.logMain.warning("Unknown mqtt log recieve - device={}, message={}".format(device, message))
        
        # Get MQTT errors from ESP32´s
        elif(top.endswith("error")):
            if(device == "esp32AirPrincipal"): self.logAirPrincipal.error(message)
            elif(device == "esp32AirReturn"): self.logAirReturn.error(message)
            elif(device.startswith("esp32")):
                level = int(device[-1]) - 1
                pos = device[5:-1]
                if pos == "front": self.logger_esp32[level][0].error(message)
                elif pos == "center": self.logger_esp32[level][1].error(message)
                elif pos == "back": self.logger_esp32[level][2].error(message)
            else: self.logMain.error("Unknown mqtt error recieve - device={}, message={}".format(device, message))
            
        # Get data from ESP32 front, center and back
        elif("esp32" in top and not "AirPrincipal" in top and not "AirReturn" in top and message != "sendData"):
            level = int(device[-1]) - 1
            pos = device[5:-1]
            if pos == "front": 
                self.ESP32.esp32[level][0].str2array(message)
                self.ESP32.esp32[level][0].connected = True
            elif pos == "center": 
                self.ESP32.esp32[level][1].str2array(message)
                self.ESP32.esp32[level][1].connected = True
            elif pos == "back": 
                self.ESP32.esp32[level][2].str2array(message)
                self.ESP32.esp32[level][2].connected = True
            # Ask again for the data if not complete
        
        # Get messages directed to Master
        elif(device.startswith('Master')):
            # Check if we can start the routine or not
            if(message.startswith('StartRoutine')):
                fl = int(message.split(",")[1]) # Floor
                serialFloor = self.mGrower.data[str(fl)]
                if fl>0 and fl<=len(self.mGrower.Gr) and serialFloor!="disconnected":
                    serialFloor = int(serialFloor)
                    serialDevice = int((serialFloor-1)/4)
                    if(message.startswith("StartRoutineNow")):
                        floor = fl - serialDevice*4
                        self.mGrower.Gr[fl-1].serialReq("sequence_n,{},41,10".format(floor))
                        self.mGrower.Gr[fl-1].mqttReq("")
                        self.mGrower.Gr[fl-1].actualTime = time()-120 
                        self.logMain.info("Starting Grower{} sequence".format(fl))
                    else:                    
                        self.mGrower.Gr[fl-1].time2Move(serialFloor)
                        self.logMain.info("Checking Grower{} status to start sequence".format(fl))
                else: self.logMain.error("Cannot start sequence. Parameters (floor or serialFloor are wrong).")
            elif(message.startswith('continueSequence')):
                fl = int(message.split(",")[1]) # Floor
                serialFloor = self.mGrower.data[str(fl)]
                if fl>0 and fl<=len(self.mGrower.Gr) and serialFloor!="disconnected":
                    serialFloor = int(serialFloor)
                    serialDevice = int((serialFloor-1)/4)
                    floor = fl - serialDevice*4
                    self.mGrower.Gr[fl-1].serialReq("continueSequence,{}".format(floor))
                    self.mGrower.Gr[fl-1].mqttReq("")
                    self.mGrower.Gr[fl-1].actualTime = time()-120 
                    self.logMain.info("Grower{} continue sequence".format(fl))
                else: self.logMain.error("Cannot continue sequence. Parameters (floor or serialFloor are wrong).")

            else: self.logMain.warning("Master MQTT request unknown. Message={}".format(message))
        # DEBUG mqtt messages
        #else: self.logMain.warning("Unknown mqtt device={}, message={}".format(device, message))
            
    def on_publish(self, client, userdata, mid):
        self.logMain.debug("Message delivered")

    def on_disconnect(self, client, userdata, rc):
        self.logMain.warning("Client MQTT Disconnected")
        self.clientConnected = False
        self.actualTime = time()
