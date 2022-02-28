#!/usr/bin/env python3

# Import directories
import paho.mqtt.client as mqtt
import paho.mqtt.publish as publish


class mqttClient():
    
    def __init__(self):
        
        #MQTT
        self.client = mqtt.Client()
        self.server_ip = '192.168.6.10'
        self.cloud_ip = '192.168.6.87'
        
        self.Topic = "cloud/tucan"
        self.pub = "tucan/cloud"
        
        self.message = ''
        
        ##################################
# MQTT communication
    def mqttInit(self):
        # Reconnect client
        
        self.client.on_connect = self.on_connect # Specify on_connect callback
        self.client.on_message = self.on_message # Specify on_message callback
        self.client.on_publish = self.on_publish # Specify on_publish callback
        self.client.on_disconnect = self.on_disconnect # Specify on_disconnect callback
        
        # Connect to MQTT Broker Parameters (IP, Port, Seconds Alive)
        if(self.client.connect(self.server_ip, 1883, 60)==0):
            pass
                    
    # Callback fires when conected to MQTT broker.
    def on_connect(self, client, userdata, flags, rc):
        
        self.messag = "MQTT"
        
        if(rc == 0):
            self.messag += " connection to: " + self.server_ip + " broker, succesful."
            self.client.subscribe(self.Topic)
            print(self.messag)
            print("Subscribed to topic = {}".format(self.Topic))
            
        else:
            self.messag += " Connection to topic: {} is refused".format(self.Topic)
            if(rc == 1): self.messag += " - incorrect protocol version"
            elif(rc == 2): self.messag += " - invalid client identifier"
            elif(rc == 3): self.messag += " - server unavailable"
            elif(rc == 4): self.messag += " - bad username or password"
            elif(rc == 5): self.messag += " - not authorised"
            else: self.messag += " - currently unused"
            
            print(self.messag)

    # Callback fires when a published message is received.
    def on_message(self, client, userdata, msg):
        
        self.top = msg.topic # Input Topic
        self.device = self.top.split("/")[1] # Device where come
        
        self.message = msg.payload.decode("utf-8") # Input message
        print(self.device + ' has published to ' +
              self.top + ': ' + self.message)
        
        if self.message=="Ready to go!":
            print('starting scan...')
            
        elif self.message=="closing mqtt":
            print("closing mqtt connection.")
            
        else:
            print(self.message)
        
                
    def on_publish(self, client, userdata, mid):
        print("Message delivered")

    def on_disconnect(self, client, userdata, rc):
        print("Client MQTT Disconnected")

def main():
    
    cloud = mqttClient()
    # Initialize MQTT connection
    cloud.mqttInit()
    
    publish.single(cloud.pub,
                    'begin',
                    hostname=cloud.server_ip)
    
    publish.single(cloud.pub,
                    'close',
                    hostname=cloud.server_ip)
    
    while True:
        if cloud.message == 'closing mqtt':
            break
        cloud.client.loop()
            


if __name__ == '__main__':
    main()
    