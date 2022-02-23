#This is a single student implementation of the BLEATS project student initial beacon.
from bluetooth.ble import BeaconService
import time

service = BeaconService()

service.start_advertising("00000000-2300-0900-0600-000000000000",1, 1, 1, 11)
time.sleep(1)
service.stop_advertising()

print("Done.")