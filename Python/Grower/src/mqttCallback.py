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
        self.log.info("Parameters updated - ID={}, Floor={}, brokerIP={}".format(self.containerID, self.floor, self.brokerIP))
    
    def sendLog(self, mssg, logType = 0):
        logTopic = "{}/Grower{}/log".format(self.containerID, self.floor)
        # Debug
        if(logType==0):
            self.log.info(mssg)
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
            self.log.info(mssg)
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
            self.log.error(message)

    # On Message Callback for MQTT
    def on_message(self, client, userdata, msg):        
        logTopic = "{}/Grower{}/log".format(self.containerID, self.floor) # Output Topic
        top = msg.topic # Input Topic
        message = msg.payload.decode("utf-8") # Input message
        
        """    
        elif(message == "OnIRCUT"):
            self.grower.turnOn_IRCUT(self.grower.IRCUT)
            self.sendLog("IRCUT On")
  
        elif(message == "OffIRCUT"):
            self.grower.turnOff_IRCUT(self.grower.IRCUT)
            self.sendLog("IRCUT Off")
            
        elif(message == "EnableIRCUT"):
            self.grower.enable_IRCUT(self.grower.IRCUT)
            self.sendLog("IRCUT Enable", 2)
            
        elif(message == "DisableIRCUT"):
            self.grower.disable_IRCUT(self.grower.IRCUT)
            self.sendLog("IRCUT Disable", 2)
        """    
        
        if(message == "OnLED1"):
            self.grower.turnOn(self.grower.LED)
            self.sendLog("Led1 On")
            
        elif(message == "OnIR"):
            self.grower.turnOn(self.grower.IR)
            self.sendLog("IR On")
            
        elif(message == "OnLED2"):
            self.grower.turnOn(self.grower.XENON)
            self.sendLog("Led2 On")
        
        elif(message == "OffLED1"):
            self.grower.turnOff(self.grower.LED)
            self.sendLog("Led1 Off")
            
        elif(message == "OffIR"):
            self.grower.turnOff(self.grower.IR)
            self.sendLog("IR Off")
            
        elif(message == "OffLED2"):
            self.grower.turnOff(self.grower.XENON)
            self.sendLog("Led2 Off")
        
        elif(message.startswith("takePicture")):
            param = message.split(',')            
            
            if self.growerStream: self.grower.disableStreaming()
            if(len(param)>6):
                Name = param[1]
                ledMode = int(param[2])
                irMode = int(param[3])
                Photo = int(param[4])
                Thermal = int(param[5])
                Cozir = int(param[6])

                if not Photo and not Thermal:
                    Photo = True
                    Thermal = True
                    self.sendLog("Taking both photo and thermal (Next time you must specify at least 1)", 2) # Warning
                check = self.grower.takePicture(Name, ledMode, irMode, Photo, Thermal, Cozir)
            else: check = self.grower.takePicture()
            if(check):
                if(len(param)>6): mssg = "Picture taken - name={}".format(Name)
                else: mssg = "Picture taken"
                mssgLevel = 0
            else:
                mssg = "Photo cannot be taken"
                mssgLevel = 3
            if self.growerStream: self.grower.enableStreaming()
            self.sendLog(mssg, mssgLevel)
        
        elif(message.startswith("thermalPhoto")):
            param = message.split(',')
            if(len(param)>1):
                Name = param[1]            
                check = self.grower.thermalPhoto(Name)
            else: check = self.grower.thermalPhoto()
            if(check):
                if(len(param)>1): mssg = "Thermal Photo taken - name={}".format(Name)
                else: mssg = "Thermal Photo taken"
                mssgLevel = 0
            else:
                mssg = "Thermal Photo cannot be taken"
                mssgLevel = 3
            if self.growerStream: self.grower.enableStreaming()
            self.sendLog(mssg, mssgLevel)
                
        elif(message.startswith("photoSequence")):
            param = message.split(',')
            
            if self.growerStream: self.grower.disableStreaming()
            
            if(len(param)>1):
                Name = param[1]
                check = self.grower.photoSequence(Name)
            else: check = self.grower.photoSequence()
            
            if(check==0):
                mssg = "Photo Sequence cannot be taken"
                mssgLevel = 3
            elif(check==1):
                mssg = "Thermal Photo taken... Normal Photo cannot be taken"
                mssgLevel = 2
            elif(check==2):
                mssg = "Normal Photo taken... Thermal Photo cannot be taken"
                mssgLevel = 2
            else:    
                if(len(param)>1): mssg = "Photo Sequence taken - name={}".format(Name)
                else: mssg = "Photo Sequence taken"
                mssgLevel = 0
            if self.growerStream: self.grower.enableStreaming()
            self.sendLog(mssg, mssgLevel)
        
        elif(message == "DisableStream"):
            self.growerStream = False
            self.grower.disableStreaming()
            if(self.grower.cam == None): self.sendLog("Camera Unavailable", 3)
            else: self.sendLog("Stream Disable", 2)
        
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
            if(self.grower.coz != None):
                hum, temp, co2 = self.grower.coz.getData()
                mssg = "cozir,{},{},{}".format(hum, temp, co2)
                self.sendLog(mssg)
            else: self.sendLog("Cozir disconnected: ignore data request", 3)
        
        elif(message == "updateGrowerDate"):
            self.grower.getDateFormat()
            self.sendLog("Updating Date Format")
        
        elif(message == "forgetWiFi"):
            self.sendLog("Forgeting WiFi Credentials", 2)
            sysGrower.runShellCommand('sudo python ./src/forgetWiFi.py')
            sysGrower.runShellCommand('sudo python ./src/APconfig.py')
        
        elif(message == "get_throttled"):
            mssg = sysGrower.getOutput_ShellCommand("/opt/vc/bin/vcgencmd get_throttled")
            self.sendLog(mssg[:-1], 1)
            
        elif(message == "reboot"):
            self.sendLog("Rebooting", 2)
            sysGrower.runShellCommand('reboot')
        
        elif(message == "shutdown"):
            self.sendLog("Shutting down", 2)
            sysGrower.runShellCommand('shutdown -h now')
            
        else:
            self.sendLog("MQTT command unknown", 3)

    def on_publish(self, client, userdata, mid):
        self.log.info("Message delivered")
        
    def on_disconnect(self, client, userdata, msg):
        self.log.warning("Client MQTT Disconnected")
        self.clientConnected = False
        self.actualTime = time()
        