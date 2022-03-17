#!/usr/bin/env python3

# Import Directories
import os
from time import time
from sysCloud import getIPaddr
import paho.mqtt.publish as publish
from time import strftime, localtime

class mqttController:
    def __init__(self, data, logger = None):
        # Logging
        self.log = logger
        self.log.info("Setting up Data Server..." )
        # Data and MQTT variables
        self.containerID = data["ID"]
        self.brokerIP = data["IP"]
        self.port = data["port"]
        self.actualTime = time()
        # Routine variables
        self.inRoutine = 0
        self.routineStarted = False
        self.masterReady = False
        self.growerReady = False
        self.tucanReady = False
        self.lightsReady = False
        self.streamReady = False
        self.takePicture = False
        self.routineTimer = time()

    def sendLog(self, mssg, logType = 0):
        logTopic = "{}/Cloud/log".format(self.containerID)
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

    def isRoutineReady(self):
        if self.masterReady and self.growerReady and self.tucanReady:
            # Prepare all devices
            self.sendLog("All devices are ready. Sending configuration parameters", 1)
            ip = getIPaddr().split(" ")[0]
            msgs = [{"topic": "{}/Grower{}".format(self.containerID, self.inRoutine), "payload": "IrOn"},
                    {"topic": "{}/Tucan".format(self.containerID), "payload": "startStreaming,{},{}".format(ip, self.port)}]
            publish.multiple(msgs, hostname = self.brokerIP)

    def startRoutine(self):
        if self.masterReady and self.growerReady and self.tucanReady and self.lightsReady and self.streamReady: 
            self.log.info("Starting routine")
            publish.single("{}/Master".format(self.containerID), "StartRoutineNow,{}".format(self.inRoutine), hostname = self.brokerIP)
            self.routineStarted = True

    def finishRoutine(self):
        # Send messages to turn off lights and streaming
            ip = getIPaddr().split(" ")[0]
            msgs = [{"topic": "{}/Grower{}".format(self.containerID, self.inRoutine), "payload": "IrOff"},
                    {"topic": "{}/Tucan".format(self.containerID), "payload": "endStreaming"}]
            publish.multiple(msgs, hostname = self.brokerIP)
            self.inRoutine = 0
            self.routineStarted = False
            self.masterReady = False
            self.growerReady = False
            self.tucanReady = False
            self.lightsReady = False
            self.streamReady = False
            self.takePicture = False
            self.log.info("Routine finished")

    # On Connect Callback for MQTT
    def on_connect(self, client, userdata, flags, rc):
        Topic = "{}/Cloud".format(self.containerID)
        message = "MQTT"
        if(rc == 0):
            message += " Connection succesful"
            mssg = "Data Server connected"
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
        logTopic = "{}/Cloud/log".format(self.containerID) # Output Topic
        top = msg.topic # Input Topic
        device = top.split("/")[1] # Device name
        message = msg.payload.decode("utf-8") # Input message
        
        # Changes messages and communication
        if(message.startswith("StartRoutine")):
            if self.inRoutine == 0:
                self.inRoutine = int(message.split(",")[1])
                self.routineTimer = time()
                if self.inRoutine>0 and self.inRoutine<=8:
                    # Create new folder to save the pictures if it doesn't exist
                    if not os.path.exists('captures/{}/floor{}/{}/'.format(self.containerID, self.inRoutine, strftime("%Y-%m-%d", localtime()))):
                        os.makedirs('captures/{}/floor{}/{}/'.format(self.containerID, self.inRoutine, strftime("%Y-%m-%d", localtime())))
                    # Ask to all devices if they are ready
                    msgs = [{"topic": "{}/Master".format(self.containerID), "payload": message},
                            {"topic": "{}/Grower{}".format(self.containerID, self.inRoutine), "payload": message},
                            {"topic": "{}/Tucan".format(self.containerID), "payload": message}]
                    publish.multiple(msgs, hostname = self.brokerIP)
                    self.sendLog("Checking status to start routine in floor {}".format(self.inRoutine), 1)
                else:
                    self.inRoutine = 0
                    self.sendLog("Invalid floor number to start the routine", 3)
            else: pass

        elif(message.startswith("MasterReady")):
            # Master is ready when motors are ready available
            self.masterReady = True
            self.routineTimer = time()
            self.log.info("Master device confirm it is ready")
            self.isRoutineReady()
            
        elif(message.startswith("GrowerReady")):
            # Grower is ready when detects Tucan Module by bluetooth
            self.growerReady = True
            self.routineTimer = time()
            self.log.info("Grower device confirm it is ready")
            self.isRoutineReady()
            
        elif(message.startswith("TucanReady")):
            # Tucan is ready when detects Grower module by bluetooth and recognize
            # the routines runs in the same floor that the closest Grower
            self.tucanReady = True
            self.routineTimer = time()
            self.log.info("Tucan device confirm it is ready")
            self.isRoutineReady()
            
        elif(message.startswith("LightsReady")):
            # Light are ready when Grower module turns on both of them
            self.lightsReady = True
            self.routineTimer = time()
            self.log.info("Grower device confirm lights are ready")
            self.startRoutine()

        elif(message.startswith("StreamReady")):
            # Stream is ready when Tucan module stablish communication with Cloud
            self.streamReady = True
            self.routineTimer = time()
            self.log.info("Tucan device confirm stream connection is ready")
            self.startRoutine()
        
        elif(message.startswith("StreamNotReady")):
            # Stream failed
            self.streamReady = False
            self.sendLog("Tucan streaming communication failed", 4)
            self.finishRoutine()

        elif(message.startswith("takePicture")):
            # Set flag takePicture true
            self.routineTimer = time()
            self.takePicture = True

        elif(message.startswith("RoutineFinished")): self.finishRoutine()            

    def on_publish(self, client, userdata, mid):
        self.log.info("Message delivered")
        
    def on_disconnect(self, client, userdata, msg):
        self.log.warning("Client MQTT Disconnected")
        self.clientConnected = False
        self.actualTime = time()
        