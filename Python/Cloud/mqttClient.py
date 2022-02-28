#!/usr/bin/env python3

# Import Directories
import paho.mqtt.client as mqtt
import paho.mqtt.publish as publish

class mqttClient:
    def __init__(self, data, stream = None):
        self.client = mqtt.Client()
        self.ip = data["ip"]
        self.id = data["id"] 
        self.stream = stream
        self.running = True
        
        # Reconnect client
        self.client.on_connect = self.on_connect # Specify on_connect callback
        self.client.on_message = self.on_message # Specify on_message callback
        #client.on_publish = on_publish # Specify on_publish callback
        self.client.on_disconnect = self.on_disconnect # Specify on_disconnect callback
        
        # Connect to MQTT Broker Parameters (IP, Port, Seconds Alive)
        if(self.client.connect(self.ip, 1883, 60)==0): pass
        
    # Callback fires when conected to MQTT broker.
    def on_connect(self, client, userdata, flags, rc):
        message = "MQTT"
        topic = "{}/tucan".format(self.id) # subscribed to...
        
        if(rc == 0):
            message += " connection to: " + self.ip + " broker, succesful."
            self.client.subscribe(topic)
            print(message)
            print("Subscribed to topic = {}".format(topic))
            
        else:
            message += "Connection to topic: {} is refused.".format(topic)
            if(rc == 1): message += " - incorrect protocol version"
            elif(rc == 2): message += " - invalid client identifier"
            elif(rc == 3): message += " - server unavailable"
            elif(rc == 4): message += " - bad username or password"
            elif(rc == 5): message += " - not authorised"
            else: message += " - currently unused"
            print(message)
            
    # Callback fires when a published message is received.
    def on_message(self, client, userdata, msg):
        top = msg.topic # Input Topic
        device = top.split("/")[1] # Device where come
        
        messg = msg.payload.decode("utf-8") # Input message
        print(device + ' has pubished to ' + top + ': ' + messg)
            
        if messg == "begin":
            print('starting...')               
            if self.stream != None:
                self.stream.clientConnect()
                publish.single("{}/cloud".format(self.id), "Ready to go!", hostname=self.ip)
            else: publish.single("{}/cloud".format(self.id), "Stream not configured", hostname=self.ip) 
        elif messg == "takePicture":
            if self.stream != None:
                self.stream.capture()
            else: print("Stream not configured")
        elif messg == "activate cameras":
            pass
        elif messg == "close":
            print('closing...')
            publish.single("{}/cloud".format(self.id), "closing mqtt", hostname=self.ip)
            self.running = False
        else:
            publish.single("{}/cloud".format(self.id), "I dont understand you...", hostname=self.ip)
            
    def on_publish(self, client, userdata, mid):
        print("Message delivered")
        
    def on_disconnect(self, client, userdata, rc):
        print("Client MQTT Disconnected")

def main():
    data = {"ip": "192.168.6.10", "id": "23-009-006"}
    
    tucan = mqttClient(data)
    
    while tucan.running:
        tucan.client.loop()
        
if __name__ == '__main__':
    main()