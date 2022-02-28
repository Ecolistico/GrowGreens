#!/usr/bin/env python3

# Import Directories
import paho.mqtt.client as mqtt
import paho.mqtt.publish as publish

class mqttClient:
    
    def __init__(self):
        
        self.client = mqtt.Client()
        self.server_ip = "192.168.6.10"
        self.cloud_ip = "192.168.6.87"
        
        self.Topic = "tucan/cloud" # subscribed to...
        self.pub = "cloud/tucan"
        
        self.messg = ''
        
############################################
#### MQTT communication
    def mqttInit(self):
        
        # Reconnect client
        self.client.on_connect = self.on_connect # Specify on_connect callback
        self.client.on_message = self.on_message # Specify on_message callback
        #client.on_publish = on_publish # Specify on_publish callback
        self.client.on_disconnect = self.on_disconnect # Specify on_disconnect callback
        
        # Connect to MQTT Broker Parameters (IP, Port, Seconds Alive)
        if(self.client.connect(self.server_ip, 1883, 60)==0):
            pass
        
    # Callback fires when conected to MQTT broker.
    def on_connect(self, client, userdata, flags, rc):

        self.message = "MQTT"
        
        if(rc == 0):
            self.message += " connection to: " + self.server_ip + " broker, succesful."
            self.client.subscribe(self.Topic)
            print(self.message)
            print("Subscribed to topic = {}".format(self.Topic))
            
        else:
            self.message += "Connection to topic: {} is refused.".format(self.Topic)
            if(rc == 1): self.message += " - incorrect protocol version"
            elif(rc == 2): self.message += " - invalid client identifier"
            elif(rc == 3): self.message += " - server unavailable"
            elif(rc == 4): self.message += " - bad username or password"
            elif(rc == 5): self.message += " - not authorised"
            else: self.message += " - currently unused"
        
            print(self.message)
            

    # Callback fires when a published message is received.
    def on_message(self, client, userdata, msg):
        
        self.top = msg.topic # Input Topic
        self.device = self.top.split("/")[1] # Device where come
        
        self.messg = msg.payload.decode("utf-8") # Input message
        print(self.device + ' has pubished to ' +
              self.top + ': ' + self.messg)

        self.gatillo = self.messg
                
            
        if self.messg == "begin":
            print('starting...')
            publish.single(self.pub,
                           "Ready to go!", hostname=self.server_ip)
            
        elif self.messg == "activate cameras":
            pass
            
        elif self.messg == "close":
            pass
            
        else:
            publish.single(self.pub,
                           "I dont understand you...", hostname=self.server_ip)
            
           


    def on_publish(self, client, userdata, mid):
        print("Message delivered")
        
    def on_disconnect(self, client, userdata, rc):
        print("Client MQTT Disconnected")

def main():
    
    tucan = mqttClient()
    tucan.mqttInit()
    
    while True:
        if tucan.messg == 'close':
            print('closing...')
            publish.single(tucan.pub,
                           "closing mqtt",
                           hostname=tucan.server_ip)
            break
            
        tucan.client.loop()
        
    
if __name__ == '__main__':
    main()