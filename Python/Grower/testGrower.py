from Grower import Grower
from time import time
from pysftp import Connection

grower = Grower()
grower.disableStreaming()
grower.photoSequence(2)
grower.enableStreaming()
grower.close()
counter = time()
grower.sendPhotos('192.168.8.100', 'pi', 'Kale1elak.', floor = 4)
print(time()-counter)
print(grower.whatIsMyIP())
print(grower.coz.getData())