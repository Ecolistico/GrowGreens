#!/usr/bin/env python3

# Import Directories
from time import time
import paho.mqtt.publish as publish
import sysGrower
from Grower import Grower

class mqttController:
    def __init__(self, logger):
        # Log variables
        self.log = logger
        self.containerID, self.floor, self.brokerIP = sysGrower.getData_JSON(sysGrower.MQTT_PATH)
        self.log.info("Setting up Grower..." )

        # Grower Variables
        self.grower = Grower(self.log)        
        
        # Mqtt Variables
        self.clientConnected = False
        self.actualTime = time()

        # Aux variables
        self.light = 0
        self.ir = False
        self.lightTimer = time()

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
    
    # Turn off all lights after 3 hours
    def turnOff_TIMEOUT(self):
        if((self.ir or self.light!=0) and time()-self.lightTimer > 180*60):
            self.grower.turnOff(self.grower.OUT1)
            self.grower.turnOff(self.grower.OUT2)
            self.grower.turnOff(self.grower.SCL)
            self.grower.turnOff(self.grower.SDA)
            self.light = 0
            self.ir = False

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
        
        
        if(message == "OnOut1"):
            self.grower.turnOn(self.grower.OUT1)
            self.sendLog("Out1 On")
            if self.light == 0 or self.light ==2:
                self.light += 1
            if self.light == 3: publish.single("{}/Cloud".format(self.containerID), "LightsReady", hostname = self.brokerIP)
            self.lightTimer = time()

        elif(message == "OnOut2"):
            self.grower.turnOn(self.grower.OUT2)
            self.sendLog("Out2 On")
            if self.light == 0 or self.light ==1:
                self.light += 2
            if self.light == 3: publish.single("{}/Cloud".format(self.containerID), "LightsReady", hostname = self.brokerIP)
            self.lightTimer = time()
        
        elif(message == "OffOut1"):
            self.grower.turnOff(self.grower.OUT1)
            self.sendLog("Out1 Off")
            if self.light == 3 or self.light == 1:
                self.light -= 1
         
        elif(message == "OffOut2"):
            self.grower.turnOff(self.grower.OUT2)
            self.sendLog("Out2 Off")
            if self.light == 3 or self.light == 2:
                self.light -= 2
        
        elif(message == "IrOn"):
            self.grower.turnOff(self.grower.SCL)
            self.grower.turnOff(self.grower.SDA)
            self.sendLog("Ir On")
            #publish.single("{}/Cloud".format(self.containerID), "LightsReady", hostname = self.brokerIP)
            self.lightTimer = time()

        elif(message == "IrOff"):
            self.grower.turnOn(self.grower.SCL)
            self.grower.turnOn(self.grower.SDA)
            self.sendLog("Ir Off")
            
        elif(message == "moveCameraX+"):
            self.grower.motorXCamera(True)
            self.sendLog("MovingCameraX+")
            
        elif(message == "moveCameraX-"):
            self.grower.motorXCamera(False)          
            self.sendLog("MovingCameraX-")
            
        elif(message == "moveCameraY+"):
            self.grower.motorYCamera(True)
            self.sendLog("MovingCameraY+")
            
        elif(message == "moveCameraY-"):
            self.grower.motorYCamera(False)          
            self.sendLog("MovingCameraY-")
                           
        elif(message == "moveHome"):
            self.grower.moveHome()

        elif(message.startswith("motorMove")):
            if message.split(",")[1] == "1":
                direction = True
            elif message.split(",")[1] == "0":
                direction = False
            steps = int(message.split(",")[3])
            self.grower.motorMove(direction, message.split(",")[2], steps)          
            self.sendLog("Moving {} Camera to {}, {} steps".format(message.split(",")[2], direction, steps))
 
        elif(message.startswith("servoMove")):
            cycle = float(message.split(",")[2])
            self.grower.moveServo(message.split(",")[1], cycle)          
            self.sendLog("Moving servo {}, {} cycles".format(message.split(",")[1], cycle))
  
        elif(message == "IrOff"):
            self.grower.turnOn(self.grower.SCL)
            self.grower.turnOn(self.grower.SDA)
            self.sendLog("Ir Off")            
            

        elif(message == "whatIsMyIP"):
            mssg = "IP={}".format(self.grower.whatIsMyIP())
            self.sendLog(mssg, 1)
            
        elif(message == "cozirData"):
            if(self.grower.coz != None):
                hum, temp, co2 = self.grower.coz.getData()
                mssg = "cozir,{},{},{}".format(hum, temp, co2)
                self.sendLog(mssg)
            else: self.sendLog("Cozir disconnected: ignore data request", 3)
        
        elif(message.startswith("StartRoutine")):
            # Here goes the bluetooth validation with Tucan for now just ignore it
            publish.single("{}/Cloud".format(self.containerID), "GrowerReady", hostname = self.brokerIP)

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
        
        elif(message == "EnableAP"):
            self.sendLog("Configuring AP...", 1)
            sysGrower.runShellCommand('sudo python ./src/APconfig.py')

    def on_publish(self, client, userdata, mid):
        self.log.info("Message delivered")
        
    def on_disconnect(self, client, userdata, msg):
        self.log.warning("Client MQTT Disconnected")
        self.clientConnected = False
        self.actualTime = time()
        