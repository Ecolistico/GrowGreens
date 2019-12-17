#!/usr/bin/env python3

# Import Directories
from time import time
import paho.mqtt.publish as publish
import sysGrower
from Grower import Grower

class mqttController:
    def __init__(self, logger):
        self.log = logger
        self.containerID, self.floor, self.brokerIP = sysGrower.getData_JSON(sysGrower.MQTT_PATH)
        self.log.info("Setting up Grower..." )
        self.grower = Grower(self.log)
        self.log.info("Setting Up Streaming..." )
        self.grower.enableStreaming() # Enable streaming
        self.growerStream = True
        self.clientConnected = False
        self.actualTime = time()

    def update(self):
        self.containerID, self.floor, self.brokerIP = sysGrower.getData_JSON(sysGrower.MQTT_PATH)
        self.log.warning("Parameters updated - ID={}, Floor={}, brokerIP={}".format(self.containerID, self.floor, self.brokerIP))
    
    def sendLog(self, mssg, logType = 0):
        logTopic = "{}/Grower{}/log".format(self.containerID, self.floor)
        # Debug
        if(logType==0):
            self.log.debug(mssg)
            mssg += ",debug"
        # Info
        elif(logType==1):
            self.log.info(mssg)
            mssg += ",info"
        # Warning
        elif(logType==2):
            self.log.warning(mssg)
            mssg += ",warning"
        # Error
        elif(logType==3):
            self.log.error(mssg)
            mssg += ",error"
        # Critical
        elif(logType==4):
            self.log.critical(mssg)
            mssg += ",critical"
        # Any other case
        else:
            self.log.debug(mssg)
            mssg += ",debug"
            
        publish.single(logTopic, mssg, hostname = self.brokerIP)
    # On Conenct Callback for MQTT
    def on_connect(self, client, userdata, flags, rc):
        Topic = "{}/Grower{}".format(self.containerID, self.floor)
        message = "MQTT"
        if(rc == 0):
            message += " Connection succesful"
            mssg = "Grower connected"
            client.subscribe(Topic)
            self.sendLog(mssg, 1)
            self.log.info(message)
            self.log.info("Subscribed topic= {}".format(Topic))
        else:
            message += " Connection refused"
            if(rc == 1): message += " - incorrect protocol version"
            elif(rc == 2): message += " - invalid client identifier"
            elif(rc == 3): message += " - server unavailable"
            elif(rc == 4): message += " - bad username or password"
            elif(rc == 5): message += " - not authorised"
            else: message += " - currently unused"
            self.log.warning(message)

    # On Message Callback for MQTT
    def on_message(self, client, userdata, msg):        
        logTopic = "{}/Grower{}/log".format(self.containerID, self.floor) # Output Topic
        top = msg.topic # Input Topic
        message = msg.payload.decode("utf-8") # Input message

        if(message == "OnLED"):
            self.grower.turnOn(self.grower.LED)
            self.sendLog("Led On")
            
        elif(message == "OnIR"):
            self.grower.turnOn(self.grower.IR)
            self.sendLog("IR On")
            
        elif(message == "OnXENON"):
            self.grower.turnOn(self.grower.XENON)
            self.sendLog("Xenon On")
            
        elif(message == "OnIRCUT"):
            self.grower.turnOn_IRCUT(self.grower.IRCUT)
            self.sendLog("IRCUT On")
            
        elif(message == "OffLED"):
            self.grower.turnOff(self.grower.LED)
            self.sendLog("Led Off")
            
        elif(message == "OffIR"):
            self.grower.turnOff(self.grower.IR)
            self.sendLog("IR Off")
            
        elif(message == "OffXENON"):
            self.grower.turnOff(self.grower.XENON)
            self.sendLog("Xenon Off")
            
        elif(message == "OffIRCUT"):
            self.grower.turnOff_IRCUT(self.grower.IRCUT)
            self.sendLog("IRCUT Off")
            
        elif(message == "EnableIRCUT"):
            self.grower.enable_IRCUT(self.grower.IRCUT)
            self.sendLog("IRCUT Enable", 2)
            
        elif(message == "DisableIRCUT"):
            self.grower.disable_IRCUT(self.grower.IRCUT)
            self.sendLog("IRCUT Disable", 2)
            
        elif(message.startswith("takePicture")):
            param = message.split(',')
            picMode = int(param[1])
            picName = param[2]
            
            if(picMode>=0 and picMode<=3 and picName!=""):
                if self.growerStream: self.grower.disableStreaming()
                self.grower.takePicture(picMode, picName)
                mssg = "Picture taken - mode={}\tname={}".format(str(picMode), picName)
                if self.growerStream: self.grower.enableStreaming()
                self.sendLog(mssg, 1)
            else:
                mssg = "Error in takePicture(): Parameter incorrect"
                self.sendLog(mssg, 3)
        
        elif(message.startswith("thermalPhoto")):
            param = message.split(',')
            picName = param[1]
            
            if(picName!=""):
                if self.growerStream: self.grower.disableStreaming()
                param = message.split(',')
                self.grower.thermalPhoto(picName)
                mssg = "Thermal Photo taken - name={}".format(picName)
                if self.growerStream: self.grower.enableStreaming()
                self.sendLog(mssg, 1)
            else:
                mssg = "Error in thermalPhoto function: Parameter incorrect"
                self.sendLog(mssg, 3)
                
        elif(message.startswith("photoSequence")):
            param = message.split(',')
            picName = param[1]
            
            if(picName!=""):
                if self.growerStream: self.grower.disableStreaming()
                param = message.split(',')
                self.grower.photoSequence(picName)
                mssg = "Photo Sequence taken - name={}".format(picName)
                if self.growerStream: self.grower.enableStreaming()
                self.sendLog(mssg, 1)
            else:
                mssg = "Error in photoSequence function: Parameter incorrect"
                self.sendLog(mssg, 3)
        
        elif(message == "DisableStream"):
            self.growerStream = False
            self.grower.disableStreaming()
            self.sendLog("Stream Disable", 2)
        
        elif(message == "EnableStream"):
            self.growerStream = True
            self.grower.enableStreaming()
            self.sendLog("Stream Enable", 2)
        
        elif(message == "whatIsMyIP"):
            mssg = "IP={}".format(self.grower.whatIsMyIP())
            self.sendLog(mssg, 1)
        
        elif(message.startswith("sendPhotos")):
            param = message.split(',')
            host = param[1]
            hostName = param[2]
            hostPassword = param[3]
            if(host!="" and hostName!="" and hostPassword!=""):
                if self.grower.sendPhotos(host, hostName, hostPassword, int(self.floor)):
                    mssg = "Photos sended to {}@{}".format(hostName, host)
                    self.sendLog(mssg, 1)
                else:
                    mssg = "Error in sendPhotos(): Cannot be executed"
                    self.sendLog(mssg, 3)
            else:
                mssg = "Error in sendPhotos(): Parameter incorrect"
                self.sendLog(mssg, 3)
        
        elif(message == "cozirData"):
            hum, temp, co2 = self.grower.coz.getData()
            mssg = "cozir,{},{},{}".format(hum, temp, co2)
            self.sendLog(mssg)
        
        elif(message == "updateGrowerDate"):
            self.grower.getDateFormat()
            self.sendLog("Updating Date Format", 1)
            
        elif(message == "reboot"):
            self.sendLog("Rebooting", 2)
            sysGrower.runShellCommand('sudo reboot')
        
        elif(message == "forgetWiFi"):
            self.sendLog("Forgeting WiFi Credentials", 2)
            sysGrower.runShellCommand('sudo python ./src/forgetWiFi.py')
            sysGrower.runShellCommand('sudo python ./src/APconfig.py')
            
        else:
            self.sendLog("MQTT command unknown", 3)

    def on_publish(self, client, userdata, mid):
        self.log.info("Message delivered")
        
    def on_disconnect(self, client, userdata, msg):
        self.log.warning("Client MQTT Disconnected")
        self.clientConnected = False
        self.actualTime = time()
        