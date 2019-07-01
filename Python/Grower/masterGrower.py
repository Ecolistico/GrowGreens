import sys
sys.path.insert(0, '../sysRasp/')
sys.path.insert(0, './src/')
from threading import Thread
from time import time
from Grower import Grower
from pysftp import Connection
import paho.mqtt.publish as publish
import paho.mqtt.client as mqtt
import sysRasp

# Global Variables
containerID, floor, brokerIP = sysRasp.getData_JSON(sysRasp.MQTT_PATH)
WiFiState = 0
print("Setting up Grower...")
grower = Grower()
print("Setting Up Streaming...")
growerStreaming = False
client = None

# On Conenct Callback for MQTT
def on_connect(client, userdata, flags, rc):
    Topic = "{}/Grower{}".format(containerID, floor)
    logTopic = "{}/Grower{}/log".format(containerID, floor)
    
    message = "MQTT"
    if(rc == 0):
        message += " Connection succesful"
        mssg = "Grower connected"
        client.subscribe(Topic)
        publish.single(logTopic, mssg, hostname = brokerIP)
        print(message)
        print("Subscribed topic: {}".format(Topic))
    else:
        message += " Connection refused"
        if(rc == 1): message += " - incorrect protocol version"
        elif(rc == 2): message += " - invalid client identifier"
        elif(rc == 3): message += " - server unavailable"
        elif(rc == 4): message += " - bad username or password"
        elif(rc == 5): message += " - not authorised"
        else: message += " - currently unused"

# On Message Callback for MQTT
def on_message(client, userdata, msg):
    global grower
    global growerStreaming
    
    logTopic = "{}/Grower{}/log".format(containerID, floor) # Output Topic
    top = msg.topic # Input Topic
    message = msg.payload.decode("utf-8") # Input message
    
    if(message == "OnLED"):
        grower.turnOn(grower.LED)
        publish.single(logTopic, "LED On", hostname = brokerIP)
        
    elif(message == "OnIR"):
        grower.turnOn(grower.IR)
        publish.single(logTopic, "IR On", hostname = brokerIP)
        
    elif(message == "OnXENON"):
        grower.turnOn(grower.XENON)
        publish.single(logTopic, "XENON On", hostname = brokerIP)
        
    elif(message == "OnIRCUT"):
        grower.turnOn_IRCUT(grower.IRCUT)
        publish.single(logTopic, "IRCUT On", hostname = brokerIP)
        
    elif(message == "OffLED"):
        grower.turnOff(grower.LED)
        publish.single(logTopic, "LED Off", hostname = brokerIP)
        
    elif(message == "OffIR"):
        grower.turnOff(grower.IR)
        publish.single(logTopic, "IR Off", hostname = brokerIP)
        
    elif(message == "OffXENON"):
        grower.turnOff(grower.XENON)
        publish.single(logTopic, "XENON Off", hostname = brokerIP)
        
    elif(message == "OffIRCUT"):
        grower.turnOff_IRCUT(grower.IRCUT)
        publish.single(logTopic, "IRCUT Off", hostname = brokerIP)
        
    elif(message == "EnableIRCUT"):
        grower.enable_IRCUT(grower.IRCUT)
        publish.single(logTopic, "IRCUT Enable", hostname = brokerIP)
        
    elif(message == "DisableIRCUT"):
        grower.disable_IRCUT(grower.IRCUT)
        publish.single(logTopic, "IRCUT Disable", hostname = brokerIP)
        
    elif(message.startswith("takePicture")):
        if growerStreaming:
            aux = True
            grower.disableStreaming()
            growerStreaming = False
        else: aux = False
        
        param = message.split(',')
        grower.takePicture(int(param[1]), param[2])
        mssg = "Picture taken - mode={}\tname={}".format(param[1], param[2])
        publish.single(logTopic, mssg, hostname = brokerIP)
        if aux:
            grower.enableStreaming()
            growerStreaming = True
    
    elif(message.startswith("thermalPhoto")):
        if growerStreaming:
            aux = True
            grower.disableStreaming()
            growerStreaming = False
        else: aux = False
        
        param = message.split(',')
        grower.thermalPhoto(param[1])
        mssg = "Thermal Photo taken - name={}".format(param[1])
        publish.single(logTopic, mssg, hostname = brokerIP)
        if aux:
            grower.enableStreaming()
            growerStreaming = True
            
    elif(message.startswith("PhotoSequence")):
        if growerStreaming:
            aux = True
            grower.disableStreaming()
            growerStreaming = False
        else: aux = False
        
        param = message.split(',')
        grower.photoSequence(param[1])
        mssg = "Photo Sequence taken - name={}".format(param[1])
        publish.single(logTopic, mssg, hostname = brokerIP)
        if aux:
            grower.enableStreaming()
            growerStreaming = True
    
    elif(message == "DisableStream"):
        grower.disableStreaming()
        growerStreaming = False
        publish.single(logTopic, "Stream Disable", hostname = brokerIP)
    
    elif(message == "EnableStream"):
        grower.enableStreaming()
        growerStreaming = True
        publish.single(logTopic, "Stream Enable", hostname = brokerIP)
    
    elif(message == "whatIsMyIP"):
        mssg = "IP={}".format(grower.whatIsMyIP())
        publish.single(logTopic, mssg, hostname = brokerIP)
    
    elif(message.startswith("sendPhotos")):
        param = message.split(',')
        grower.sendPhotos(param[1], param[2], param[3], int(floor))
        mssg = "Photos sended to {}@{}".format(param[2], param[1])
        publish.single(logTopic, mssg, hostname = brokerIP)
    
    elif(message == "cozirData"):
        hum, temp, co2 = grower.coz.getData()
        mssg = "{},{},{}".format(hum, temp, co2)
        publish.single(logTopic, mssg, hostname = brokerIP)
    
    elif(message == "reboot"):
        publish.single(logTopic, "Rebooting", hostname = brokerIP)
        sysRasp.runShellCommand('sudo reboot')
    
    elif(message == "forgetWiFi"):
        publish.single(logTopic, "Forgeting WiFi Credentials", hostname = brokerIP)
        sysRasp.runShellCommand('sudo python ./src/forgetWiFi.py')
        sysRasp.runShellCommand('sudo python ./src/APconfig.py')
        
    else:
        publish.single(logTopic, "MQTT command unknown", hostname = brokerIP)
        print("MQTT command unknown")
        
    print("Message enter: Topic = {}\tMessage = {}".format(top, message))
        
def on_disconnect(client, userdata, msg):
    print("Client MQTT Disconnected")
    
def check_WiFi():
    global containerID
    global floor
    global brokerIP
    global WiFiState
    
    WiFiTime = time()- 20
    WiFiCount = 0
    prevWiFiState = 0
    
    while True:
        if(time()- WiFiTime > 20):
            if(sysRasp.isWiFi()):
                if(prevWiFiState == 2):
                     ID, fl, IP = sysRasp.getData_JSON(sysRasp.MQTT_PATH)
                     if(ID!= containerID and ID!=""):
                         containerID = ID
                     if(fl!= floor and fl!=""):
                         floor = fl
                     if(IP!=brokerIP and IP!=""):
                         brokerIP = IP
                prevWiFiState = WiFiState
                WiFiState = 1 # Conected to WiFi
                WiFiCount = 0
            elif(sysRasp.isAP()):
                prevWiFiState = WiFiState
                WiFiState = 2 # Access Point created
                WiFiCount = 0
            else:
                prevWiFiState = WiFiState
                WiFiState = 0 # Not accesPoint or WiFi
                WiFiCount += 1
            WiFiTime = time()
        
        if(WiFiCount>=5):
            WiFiCount = 0
            print("Configuring AP...")
            sysRasp.runShellCommand('sudo python ./src/APconfig.py')
            
def MQTT():
    #global brokerIP
    global WiFiState
    global growerStreaming
    
    prevWiFiState = 0
    
    grower.enableStreaming()
    growerStreaming = True
    
    while True:
        global client
        
        if(WiFiState!=prevWiFiState):
           if(WiFiState == 1 and brokerIP!=""):
               client = mqtt.Client()
               client.on_connect = on_connect # Specify on_connect callback
               client.on_message = on_message # Specify on_message callback
               #client.on_publish = on_publish # Specify on_publish callback
               client.on_disconnect # Specify on_disconnect callback
               client.connect(brokerIP, 1883, 60) # Connect to MQTT Broker Parameters (IP, Port, Seconds Alive)
           prevWiFiState = WiFiState
        
        if(WiFiState==1 and client!=None): client.loop()
            
if __name__ == "__main__":
    try:
        while True:
            thread1 = Thread(target=check_WiFi, args=())
            thread2 = Thread(target=MQTT, args=())
            
            thread1.start()
            thread2.start()

            thread1.join()
            thread2.join()
            
            while True:
                pass
            
    except:
        grower.close() # Clean GPIO
        if(client!=None):
            client.disconnect() # Disconnect MQTT