from Grower import Grower
import time
from pysftp import Connection

grower = Grower()
#grower.disableStreaming()
#grower.photoSequence(2)
#grower.enableStreaming()
grower.close()
#counter = time()
#grower.sendPhotos('192.168.8.100', 'pi', 'Kale1elak.', floor = 4)
#print(time()-counter)
#print(grower.whatIsMyIP())
print(grower.coz.getData())
time.sleep(1)
print(grower.coz.getData()[0])
time.sleep(1)
print(grower.coz.getData()[1])
time.sleep(1)
print(grower.coz.getData()[2])
time.sleep(1)
print(grower.coz.getData())
