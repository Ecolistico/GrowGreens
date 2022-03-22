#!/usr/bin/env python3

# Import directories
from time import time
from bluetoothManager import Client, bluetoothDevice
import paho.mqtt.publish as publish

class mqttController:
    def __init__(self, deviceManager, logger = None):
        # Define loggers
        self.log = logger.logger

        # Define deviceManager
        self.manager = deviceManager

        # Define aux variables
        self.clientConnected = False
        self.actualTime = time()
        
    def Msg2Log(self, mssg):
        if self.log != None:
            if(mssg.split(",")[1]=="debug"): self.log.debug(mssg.split(",")[0])
            elif(mssg.endswith(",debug")): self.log.debug(mssg.replace(",debug", ""))
            elif(mssg.split(",")[1]=="info"): self.log.info(mssg.split(",")[0])
            elif(mssg.endswith(",info")): self.log.info(mssg.replace(",info", ""))
            elif(mssg.split(",")[1]=="warning"): self.log.warning(mssg.split(",")[0])
            elif(mssg.endswith(",warning")): self.log.warning(mssg.replace(",warning", ""))
            elif(mssg.split(",")[1]=="error"): self.log.error(mssg.split(",")[0])
            elif(mssg.endswith(",error")): self.log.error(mssg.replace(",error", ""))
            elif(mssg.split(",")[1]=="critical"): self.log.critical(mssg.split(",")[0])
            elif(mssg.endswith(",critical")): self.log.critical(mssg.replace(",critical", ""))
            else: self.log.info(mssg)
        else: print(mssg)
    
    # Callback fires when conected to MQTT broker.
    def on_connect(self, client, userdata, flags, rc):
        Topic = "positioningSystem/#"
        message = "MQTT"
        if(rc == 0):
            message += " Connection succesful,info"
            client.subscribe(Topic)
            self.Msg2Log(message)
            self.Msg2Log("Subscribed topic= {},info".format(Topic))
        else:
            message += " Connection refused"
            if(rc == 1): message += " - incorrect protocol version,error"
            elif(rc == 2): message += " - invalid client identifier,error"
            elif(rc == 3): message += " - server unavailable,error"
            elif(rc == 4): message += " - bad username or password,error"
            elif(rc == 5): message += " - not authorised,error"
            else: message += " - currently unused,error"
            self.Msg2Log(message)

    # Callback fires when a published message is received.
    def on_message(self, client, userdata, msg):
        top = msg.topic # Input Topic
        message = msg.payload.decode("utf-8") # Input message
        device = top.split("/")[1] # Device where come
        
        topicList = top.split("/") # List topic

        if topicList[1] not in self.manager.clients: 
            self.Msg2Log("Client not recognized: {},info".format(topicList[1]))
            # Here we need to ask extra information to the client to update the client list
            #self.manager.clients[topicList[1]] = Client(x, y, z)
        
        macList = []
        for dev in self.manager.devices: macList.append(self.manager.devices[dev].mac)
        if topicList[2] not in macList:
            self.Msg2Log("Beacon not recognized: {},info".format(topicList[2]))
            # Here we need to ask extra information to the clients to update the beacons list
            #self.manager.devices[topicList[2]] = bluetoothDevice(dataDevice, clientData)

        if topicList[1] in self.manager.clients and topicList[2] in macList:
            for dev in self.manager.devices:
                if self.manager.devices[dev].mac == topicList[2]:
                    self.manager.devices[dev].updateDistance(topicList[1], float(message))
                    self.Msg2Log("Beacon {} distance updated to {},debug".format(topicList[2], message))
                    break
                
        # Get MQTT logs from all the devices
        if(top.endswith("updateDevices")):
            pass
        
        # Get MQTT errors from ESP32´s
        elif(top.endswith("deleteDevice")):
            pass
        
        elif(message.startswith('updateInfo')):
            pass

        elif(message.startswith('updateDevice')):
            pass
                
 
    def on_publish(self, client, userdata, mid):
        self.Msg2Log("MQTT Message delivered,info")

    def on_disconnect(self, client, userdata, rc):
        self.Msg2Log("Client MQTT Disconnected,warning")
        self.clientConnected = False
        self.actualTime = time()
