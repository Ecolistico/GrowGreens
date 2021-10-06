#!/usr/bin/env python3

import os
from time import time
from datetime import datetime
import RPi.GPIO as GPIO
import cozir
from sysGrower import getIPaddr

"""
Functions resume:
    * Grower()- Constructor for the class
    * getState() - returns the state of some gpio
    * turnOn() - turn On some gpio
    * turnOff() - turn Off some gpio
    * whatIsMyIP() - Returns a string with the IP addres from this device
    * close() - Cleanup the GPIO´s
"""

class Grower:
    def __init__(self, logger, out1 = 24, out2 = 12):
        self.log = logger

        self.day = 0
        self.month = 0
        self.year = 0
        self.getDateFormat()

        self.OUT1 = out1     # GPIO to activate IR
        self.OUT2 = out2    # GPIO to activate LED
        self.SDA = 2
        self.SCL = 3

        # Setting Up Cozir
        try:
            self.coz = cozir.Cozir(self.log)
            checkCozir = 0
            # For now just stable in polling mode
            if(self.coz.opMode(self.coz.polling)):
                self.log.info("Cozir: Set Mode = K{0}".format(self.coz.act_OpMode))
            else: checkCozir +=1
            # Get hum, temp and co2_filter
            if(self.coz.setData_output(self.coz.Hum + self.coz.Temp + self.coz.CO2_filt)):
                self.log.info("Cozir: Data Mode = M{}".format(self.coz.act_OutMode))
            else: checkCozir +=1
            if checkCozir == 2:
                self.coz.close()
                self.coz = None
                self.log.critical("Cozir not found: sensor disconnected")
        except Exception as e:
            self.coz = None
            self.log.critical("Cozir not found: sensor disconnected. {}".format(e))

        # Setting Up GPIO
        GPIO.setmode(GPIO.BCM)

        GPIO.setup(self.OUT1, GPIO.OUT)
        GPIO.setup(self.OUT2, GPIO.OUT)
        GPIO.setup(self.SDA, GPIO.OUT)
        GPIO.setup(self.SCL, GPIO.OUT)

        GPIO.output(self.OUT1, GPIO.LOW)
        GPIO.output(self.OUT2, GPIO.LOW)
        GPIO.output(self.SDA, GPIO.LOW)
        GPIO.output(self.SCL, GPIO.LOW)

    def getState(self, gpio):
        return GPIO.input(gpio)

    def turnOn(self, gpio):
        GPIO.output(gpio, GPIO.HIGH)

    def turnOff(self, gpio):
        GPIO.output(gpio, GPIO.LOW)

    def wait(self, timeout):
        actualTime = time()
        while(time()-actualTime<timeout): continue

    def checkDayDirectory(self):
        if os.path.exists('data/{}-{}-{}'.format(self.day, self.month, self.year)): return True
        else: return False

    def createDayDirectory(self):
        os.makedirs('data/{}-{}-{}'.format(self.day, self.month, self.year))
        os.makedirs('data/{}-{}-{}/sequence'.format(self.day, self.month, self.year))
        os.makedirs('data/{}-{}-{}/thermal'.format(self.day, self.month, self.year))
        os.makedirs('data/{}-{}-{}/manual'.format(self.day, self.month, self.year))

    def getDateFormat(self):
        now = datetime.now()
        if now.day<10: self.day = "0{}".format(now.day)
        else: self.day = "{}".format(now.day)
        if now.month<10: self.month = "0{}".format(now.month)
        else: self.month = "{}".format(now.month)
        self.year = now.year

    def whatIsMyIP(self):
        return getIPaddr()

    def close(self):
        GPIO.cleanup() # Clean GPIO
        if(self.coz != None): self.coz.close() # Close serial Cozir port
