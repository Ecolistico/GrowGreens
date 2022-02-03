#!/usr/bin/env python3

import numpy as np

# Calculation of Vapor Pressure Deficit (VPD) with the equation of Penman-Montheith
def DVP(temperature, humidity):
    PVsat= 0.61078*np.exp((17.27*temperature)/(237.3+temperature))
    dvp = PVsat * (1-humidity/100)
    return dvp

def MideaControl(value):
    # Turn on AC
    if value > 0: 
        msgs = [{"device": "esp32AirPrincipal", "payload": "AcOn,20,0"},
                {"device": "esp32AirReturn", "payload": "AcOn,20,0"}]
    # Turn off AC
    elif value < 0: 
        msgs = [{"device": "esp32AirPrincipal", "payload": "AcOff"}, 
                {"device": "esp32AirReturn", "payload": "AcOff"}]
    # Do nothing
    else: msgs = []
    return msgs

class EnvControl:
    def __init__(self, configData, esp32Data = None):
        self.enable = True # For future use if we want to disable Air Control
        self.config = configData
        self.configError = False
        self.esp32Data = esp32Data
        self.AC = None

    def update(self):
        if self.enable and not self.configError:
            if self.config['controlType'].lower() == 'onoff':
                if self.esp32Data != None:
                    temp = self.esp32Data.averageTemp()
                    hum = self.esp32Data.averageHum()
                else: # Default values for testing purposes
                    temp = 23
                    hum = 75
                if self.config['controlMode'].lower() == 'temperature': actual = temp
                elif self.config['controlMode'].lower() == 'humidity': actual = hum
                elif self.config['controlMode'].lower() == 'dvp': actual = DVP(temp, hum)
                else: 
                    if not self.configError:
                        self.configError = True
                        print("Configuration Error: controlMode is not implemented yet")
                    actual = (float(self.config['max']) + float(self.config['min']))/2
                
                if self.config['controller'].lower() == 'midea':
                    out = self.On_Off(actual, float(self.config['max']), float(self.config['min']))
                    return MideaControl(out)
                else: # Do nothing
                    if not self.configError:
                        self.configError = True
                        print("Configuration Error: controller is not implemented yet")
                    return [] 
            else: # Do nothing
                if not self.configError:
                    self.configError = True
                    print("Configuration Error: controlType is not implemented yet")
                return [] 
        else: return [] # Do nothing and wait for next update

    def On_Off(self, actual, max, min):
        # Turn On
        if actual>max and self.AC!=True:
            self.AC = True
            return 1
        # Turn Off
        elif actual<min and self.AC!=False:
            self.AC = False
            return -1
        # Do nothing
        return 0

# Debug
def main():
    from time import sleep
    data = {'controlType': 'OnOff', 'controlMode': 'temperature', 'max': '22', 'min': '17', 'controller': 'midea'}
    env = EnvControl(data, None)
    while True: 
        env_msgs = env.update()
        new_msgs = []
        for msg in env_msgs:
            new_msgs.append({"topic": "{}/{}".format("23-009-006", msg["device"]), "payload": msg["payload"]})
        if len(new_msgs) > 0: print(new_msgs)
        sleep(3)
    
if __name__ == '__main__':
    main()