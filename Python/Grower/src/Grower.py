#!/usr/bin/env python3

import os
from time import time
from datetime import datetime
import RPi.GPIO as GPIO
import cozir
from sysGrower import getIPaddr
import time

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
                
        self.in1 = 17 #For Pins For 28BYJ-48 X
        self.in2 = 18
        self.in3 = 27
        self.in4 = 22
        
        self.iny1 = 8 #For Pins For 28BYJ-48X
        self.iny2 = 25
        self.iny3 = 24
        self.iny4 = 7
        
        self.limitSwitchX = 26
        self.limitSwitchY = 16
        
        
        self.step_sequence = [[1,0,0,1],
                             [1,0,0,0],
                             [1,1,0,0],
                             [0,1,0,0],
                             [0,1,1,0],
                             [0,0,1,0],
                             [0,0,1,1],
                             [0,0,0,1]]
        
        # careful lowering this, at some point you run into the mechanical limitation of how quick your motor can move
        self.step_sleep = 0.001

        self.step_count = 4096 # 5.625*(1/64) per step, 4096 steps is 360°
        
                
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
        
        GPIO.setup( self.in1, GPIO.OUT )
        GPIO.setup( self.in2, GPIO.OUT )
        GPIO.setup( self.in3, GPIO.OUT )
        GPIO.setup( self.in4, GPIO.OUT )
        
        GPIO.setup( self.iny1, GPIO.OUT )
        GPIO.setup( self.iny2, GPIO.OUT )
        GPIO.setup( self.iny3, GPIO.OUT )
        GPIO.setup( self.iny4, GPIO.OUT )
        
        GPIO.setup(self.limitSwitchX, GPIO.IN, pull_up_down=GPIO.PUD_UP)
        GPIO.setup(self.limitSwitchY, GPIO.IN, pull_up_down=GPIO.PUD_UP)

        GPIO.output(self.OUT1, GPIO.LOW)
        GPIO.output(self.OUT2, GPIO.LOW)
        GPIO.output(self.SDA, GPIO.LOW)
        GPIO.output(self.SCL, GPIO.LOW)
        
        # initializing
        GPIO.output( self.in1, GPIO.LOW )
        GPIO.output( self.in2, GPIO.LOW )
        GPIO.output( self.in3, GPIO.LOW )
        GPIO.output( self.in4, GPIO.LOW )
        
        
        self.motor_pins = [self.in1,self.in2,self.in3,self.in4]
        self.motor_step_counter = 0 ;
        
        
        GPIO.output( self.iny1, GPIO.LOW )
        GPIO.output( self.iny2, GPIO.LOW )
        GPIO.output( self.iny3, GPIO.LOW )
        GPIO.output( self.iny4, GPIO.LOW )
        
        
        self.motor_pins1 = [self.iny1,self.iny2,self.iny3,self.iny4]
        self.motor_step_counter1 = 0 ;

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
    
    def motorXCamera(self, direction):
        i = 0
        for i in range(self.step_count):
            if GPIO.input(self.limitSwitchX) == 0:
                print("HomeX")
                break
            for pin in range(0, len(self.motor_pins)):
                GPIO.output( self.motor_pins[pin], self.step_sequence[self.motor_step_counter][pin] )
            if direction==True:
                self.motor_step_counter = (self.motor_step_counter - 1) % 8
            elif direction==False:
                self.motor_step_counter = (self.motor_step_counter + 1) % 8
            else: # defensive programming
                print( "uh oh... direction should *always* be either True or False" )
                GPIO.output( self.in1, GPIO.LOW )
                GPIO.output( self.in2, GPIO.LOW )
                GPIO.output( self.in3, GPIO.LOW )
                GPIO.output( self.in4, GPIO.LOW )
                exit( 1 )
            time.sleep( self.step_sleep )
            GPIO.output( self.in1, GPIO.LOW )
            GPIO.output( self.in2, GPIO.LOW )
            GPIO.output( self.in3, GPIO.LOW )
            GPIO.output( self.in4, GPIO.LOW )
            
    def motorYCamera(self, direction):
        i = 0
        for i in range(self.step_count):
            if GPIO.input(self.limitSwitchY) == 0:
                print("HomeY")
                break
            for pin in range(0, len(self.motor_pins1)):
                GPIO.output( self.motor_pins1[pin], self.step_sequence[self.motor_step_counter1][pin] )
            if direction==True:
                self.motor_step_counter1 = (self.motor_step_counter1 - 1) % 8
            elif direction==False:
                self.motor_step_counter1 = (self.motor_step_counter1 + 1) % 8
            else: # defensive programming
                print( "uh oh... direction should *always* be either True or False" )
                GPIO.output( self.iny1, GPIO.LOW )
                GPIO.output( self.iny2, GPIO.LOW )
                GPIO.output( self.iny3, GPIO.LOW )
                GPIO.output( self.iny4, GPIO.LOW )
                exit( 1 )
            time.sleep( self.step_sleep )
            GPIO.output( self.iny1, GPIO.LOW )
            GPIO.output( self.iny2, GPIO.LOW )
            GPIO.output( self.iny3, GPIO.LOW )
            GPIO.output( self.iny4, GPIO.LOW )
        
    def moveHome(self):
        self.motorXCamera(True)
        self.motorYCamera(True)

    def motorMove(self, direction, XY, steps):
        i = 0
        if XY == "X":
            for i in range(steps):
                if GPIO.input(self.limitSwitchX) == 0:
                    print("HomeX")
                    break
                for pin in range(0, len(self.motor_pins)):
                    GPIO.output( self.motor_pins[pin], self.step_sequence[self.motor_step_counter][pin] )
                if direction==True:
                    self.motor_step_counter = (self.motor_step_counter - 1) % 8
                elif direction==False:
                    self.motor_step_counter = (self.motor_step_counter + 1) % 8
                else: # defensive programming
                    print( "uh oh... direction should *always* be either True or False" )
                    GPIO.output( self.in1, GPIO.LOW )
                    GPIO.output( self.in2, GPIO.LOW )
                    GPIO.output( self.in3, GPIO.LOW )
                    GPIO.output( self.in4, GPIO.LOW )
                    exit( 1 )
                time.sleep( self.step_sleep )
                GPIO.output( self.in1, GPIO.LOW )
                GPIO.output( self.in2, GPIO.LOW )
                GPIO.output( self.in3, GPIO.LOW )
                GPIO.output( self.in4, GPIO.LOW )
            
        if XY == "Y":
            for i in range(steps):
                if GPIO.input(self.limitSwitchY) == 0:
                    break
                for pin in range(0, len(self.motor_pins1)):
                    GPIO.output( self.motor_pins1[pin], self.step_sequence[self.motor_step_counter1][pin] )
                if direction==True:
                    self.motor_step_counter1 = (self.motor_step_counter1 - 1) % 8
                elif direction==False:
                    self.motor_step_counter1 = (self.motor_step_counter1 + 1) % 8
                else: # defensive programming
                    print( "uh oh... direction should *always* be either True or False" )
                    GPIO.output( self.iny1, GPIO.LOW )
                    GPIO.output( self.iny2, GPIO.LOW )
                    GPIO.output( self.iny3, GPIO.LOW )
                    GPIO.output( self.iny4, GPIO.LOW )
                    exit( 1 )
                time.sleep( self.step_sleep )
                GPIO.output( self.iny1, GPIO.LOW )
                GPIO.output( self.iny2, GPIO.LOW )
                GPIO.output( self.iny3, GPIO.LOW )
                GPIO.output( self.iny4, GPIO.LOW )
          

    def close(self):
        GPIO.cleanup() # Clean GPIO
        if(self.coz != None): self.coz.close() # Close serial Cozir port
                