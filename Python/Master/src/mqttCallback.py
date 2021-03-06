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
                 loggerFront,
                 loggerCenter,
                 loggerBack):
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
        self.logFront = loggerFront
        self.logCenter = loggerCenter
        self.logBack = loggerBack
        # Define ESP32´s object
        self.ESP32 = ESPdata.multiESP(self.logFront, self.logCenter, self.logBack)
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
                    
            elif(device == "esp32front"): self.logFront.debug(message)
            elif(device == "esp32center"): self.logCenter.debug(message)
            elif(device == "esp32back"): self.logBack.debug(message)
            else: self.logMain.warning("Unknown mqtt log recieve - device={}, message={}".format(device, message))
        
        # Get MQTT errors from ESP32´s
        elif(top.endswith("error")):
            if(device == "esp32front"): self.logFront.error(message)
            elif(device == "esp32center"): self.logCenter.error(message)
            elif(device == "esp32back"): self.logBack.error(message)
            else: self.logMain.error("Unknown mqtt log recieve - device={}, message={}".format(device, message))
            
        # Get data from ESP32 front, center and back
        elif(top.endswith("esp32front") and message!="sendData"):
            self.ESP32.front.str2array(message)
            self.ESP32.front.connected = True
            # Ask again for the data if not complete
        elif(top.endswith("esp32center") and message!="sendData"):
            self.ESP32.center.str2array(message)
            self.ESP32.center.connected = True
            # Ask again for the data if not complete
        elif(top.endswith("esp32back") and message!="sendData"):
            self.ESP32.back.str2array(message)
            self.ESP32.back.connected = True
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
