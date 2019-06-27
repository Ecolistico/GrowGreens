from Grower import Grower
from time import time

counter = time()
grower = Grower()
grower.disableStreaming()
grower.photoSequence(1)
grower.enableStreaming()
grower.close()
print(time()-counter)
print(grower.whatIsMyIP())
print(grower.coz.getData())