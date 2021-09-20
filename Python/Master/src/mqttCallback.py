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
                 loggerMain,
                 loggerGr1,
                 loggerGr2,
                 loggerGr3,
                 loggerGr4,
                 loggerFront1,
                 loggerCenter1,
                 loggerBack1,
                 loggerFront2,
                 loggerCenter2,
                 loggerBack2):
        # Define container parameters
        self.ID = ID
        self.brokerIP = brokerIP
        self.conn = connector
        # Define loggers
        self.logMain = loggerMain
        self.logGr1 = loggerGr1
        self.logGr2 = loggerGr2
        self.logGr3 = loggerGr3
        self.logGr4 = loggerGr4
        self.logFront1 = loggerFront1
        self.logCenter1 = loggerCenter1
        self.logBack1 = loggerBack1
        self.logFront2 = loggerFront2
        self.logCenter2 = loggerCenter2
        self.logBack2 = loggerBack2
        # Define ESP32´s object
        self.ESP32 = ESPdata.multiESP(self.logFront1, self.logCenter1, self.logBack1, self.logFront2, self.logCenter2, self.logBack2)
        # Define multiGrower´s object
        self.mGrower = multiGrower
        # Define aux variables
        self.clientConnected = False
        self.actualTime = time()
        # Communication with MongoDB-Parse Server
        self.serverIP = ''
        
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
    
    def onGrowerMsg(self, grower, message):
        grower.connected = True
        if(message.startswith("Photo Sequence taken") and grower.inRoutine):
            grower.mqttReq("")
            grower.serialReq("continueSequence,{}".format(grower.floor))
            grower.actualTime = time()-120   
        elif(message.startswith("cozir")): grower.str2array(message)
        
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
            if(device == "Grower1"):
                self.Msg2Log(self.logGr1, message)
                self.requestGr1 = False
                self.onGrowerMsg(self.mGrower.Gr1, message)
            elif(device == "Grower2"):
                self.Msg2Log(self.logGr2, message)
                self.requestGr2 = False
                self.onGrowerMsg(self.mGrower.Gr2, message)
            elif(device == "Grower3"):
                self.Msg2Log(self.logGr3, message)
                self.requestGr3 = False
                self.onGrowerMsg(self.mGrower.Gr3, message)
            elif(device == "Grower4"):
                self.Msg2Log(self.logGr4, message)
                self.requestGr4 = False
                self.onGrowerMsg(self.mGrower.Gr4, message)
                    
            elif(device == "esp32front1"): self.logFront1.debug(message)
            elif(device == "esp32center1"): self.logCenter1.debug(message)
            elif(device == "esp32back1"): self.logBack1.debug(message)
            elif(device == "esp32front2"): self.logFront2.debug(message)
            elif(device == "esp32center2"): self.logCenter2.debug(message)
            elif(device == "esp32back2"): self.logBack2.debug(message)
            else: self.logMain.warning("Unknown mqtt log recieve - device={}, message={}".format(device, message))
        
        # Get MQTT errors from ESP32´s
        elif(top.endswith("error")):
            if(device == "esp32front1"): self.logFront1.error(message)
            elif(device == "esp32center1"): self.logCenter1.error(message)
            elif(device == "esp32back1"): self.logBack1.error(message)
            if(device == "esp32front2"): self.logFront2.error(message)
            elif(device == "esp32center2"): self.logCenter2.error(message)
            elif(device == "esp32back2"): self.logBack2.error(message)
            else: self.logMain.error("Unknown mqtt log recieve - device={}, message={}".format(device, message))
            
        # Get data from ESP32 front, center and back
        elif(top.endswith("esp32front1") and message!="sendData"):
            self.ESP32.front1.str2array(message)
            self.ESP32.front1.connected = True
            # Ask again for the data if not complete
        elif(top.endswith("esp32center1") and message!="sendData"):
            self.ESP32.center1.str2array(message)
            self.ESP32.center1.connected = True
            # Ask again for the data if not complete
        elif(top.endswith("esp32back1") and message!="sendData"):
            self.ESP32.back1.str2array(message)
            self.ESP32.back1.connected = True
            # Ask again for the data if not complete
        elif(top.endswith("esp32front2") and message!="sendData"):
            self.ESP32.front2.str2array(message)
            self.ESP32.front2.connected = True
            # Ask again for the data if not complete
        elif(top.endswith("esp32center2") and message!="sendData"):
            self.ESP32.center2.str2array(message)
            self.ESP32.center2.connected = True
            # Ask again for the data if not complete
        elif(top.endswith("esp32back2") and message!="sendData"):
            self.ESP32.back2.str2array(message)
            self.ESP32.back2.connected = True
            # Ask again for the data if not complete
        
        # Communication withb MongoDB-Parse Server
        if(device == 'Server' and not message.startswith('whatIsMyIP')):
            self.serverIP = message
            
    def on_publish(client, userdata, mid):
        self.logMain.debug("Message delivered")

    def on_disconnect(client, userdata, rc):
        self.logMain.warning("Client MQTT Disconnected")
        self.clientConnected = False
        self.actualTime = time()
