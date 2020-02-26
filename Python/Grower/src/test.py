import paho.mqtt.publish as publish


pub = str(input("What do you want to publish?"))
publish.single("15-113-001/Grower3", pub, hostname = "192.168.1.100")
print('OK')