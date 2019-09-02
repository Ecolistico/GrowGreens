#!/usr/bin/env python

# Import directories
import bme680 # From: https://github.com/pimoroni/bme680-python
from time import time
from numpy import log
from datetime import datetime

class BME680:
    def __init__(self):
        try:
            self.sensor = bme680.BME680(bme680.I2C_ADDR_PRIMARY)
        except IOError:
            self.sensor = bme680.BME680(bme680.I2C_ADDR_SECONDARY)
        
        self.temp = 0
        self.hum = 0
        self.pressure = 0
        self.altitude = 0
        self.actualTime = time()
        # Configuration
        
        # These oversampling settings can be tweaked to
        # change the balance between accuracy and noise in
        # the data.

        self.sensor.set_humidity_oversample(bme680.OS_2X)
        self.sensor.set_pressure_oversample(bme680.OS_4X)
        self.sensor.set_temperature_oversample(bme680.OS_8X)
        self.sensor.set_filter(bme680.FILTER_SIZE_3)
        self.sensor.set_gas_status(bme680.ENABLE_GAS_MEAS)
        
        self.sensor.set_gas_heater_temperature(320)
        self.sensor.set_gas_heater_duration(150)
        self.sensor.select_gas_heater_profile(0)
        
        # Up to 10 heater profiles can be configured, each
        # with their own temperature and duration.
        # sensor.set_gas_heater_profile(200, 150, nb_profile=1)
        # sensor.select_gas_heater_profile(1)
        
        self.read(True) # Get the firs reading
        
    def read(self, alt = False):
        self.actualTime = time()
        while(not self.sensor.get_sensor_data() and time()-self.actualTime<1): pass
        if time()-self.actualTime<1:
            self.temp = self.sensor.data.temperature
            self.hum = self.sensor.data.humidity
            self.pressure = self.sensor.data.pressure
            if alt: self.altitude = self.getAltitude()
            return True
        return False
    
    def getAltitude(self):
        u = 0.0289644 # kg/mol
        g = 9.80665 # m/s^2
        R = 8.31432 # N*m/(mol*K)
        T = self.sensor.data.temperature+273.15
        P = self.sensor.data.pressure/10
        P0 = 101.325 # kPa = 1atm
        h = log(P/P0)*(-R*T/(u*g))
        return h
    
    def printData(self):
        output = '{0:.2f} C,{1:.2f} %RH, {2:.2f} m'.format(self.temp, self.hum, self.altitude)
        print(output)
        
    def upload2DB(self, dbConnector):
        # Create cursor
        c = dbConnector.cursor()
        # Create table if not exist
        c.execute("CREATE TABLE IF NOT EXISTS extSens(datetime TIMESTAMP, Temp REAL, Hum REAL)")
        # Insert values
        c.execute("INSERT INTO extSens (datetime, Temp, Hum) VALUES (?, ?, ?)",
                  (datetime.now(), self.temp, self.hum))
        # Commit changes
        dbConnector.commit()
        # Erase cursor
        c.close()
        