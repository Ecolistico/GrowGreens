import os
import time
from picamera import PiCamera
from datetime import datetime
import numpy as np
import RPi.GPIO as GPIO
from Adafruit_AMG88xx import Adafruit_AMG88xx

class Grower:
    def __init__(self, ir = 22, led = 23, xenon = 26, en1 = 4, en2 = 27, in1 = 24, in2 = 18, in3 = 17, in4 = 10, thermal1Addr = 0x69, thermal2Addr = 0x68, ircut = 0):
        self.now = datetime.now()
        
        self.MODE_THERMAL = 0 # Define thermal mode.
        self.MODE_LED = 1 # Define led mode
        self.MODE_XENON = 2 # Define xenon mode

        self.IR = ir     # GPIO to activate IR
        self.LED = led    # GPIO to activate LED
        self.XENON = xenon  # GPIO to activate XENON
        self.En1 = en1     # GPIO to enable motor/IRCUT 1
        self.En2 = en2    # GPIO to enable motor/IRCUT 2
        self.In1 = in1    # GPIO to input 1 -> motor/IRCUT 1
        self.In2 = in2    # GPIO to input 2 -> motor/IRCUT 1
        self.In3 = in3    # GPIO to input 3 -> motor/IRCUT 2
        self.In4 = in4    # GPIO to input 4 -> motor/IRCUT 2

        self.IrCut = ircut # Default IRCUT output. 0 for outputs 1, 2 and 1 for outputs 3, 4

        self.thermalCam1 = Adafruit_AMG88xx(address=thermal1Addr) # Set thermalCam1 on its i2c addres
        self.thermalCam2 = Adafruit_AMG88xx(address=thermal2Addr) # Set thermalCam2 on its i2c addres
        self.cam = PiCamera()

        GPIO.setmode(GPIO.BCM)

        GPIO.setup(self.IR, GPIO.OUT)
        GPIO.setup(self.LED, GPIO.OUT)
        GPIO.setup(self.XENON, GPIO.OUT)
        GPIO.setup(self.En1, GPIO.OUT)
        GPIO.setup(self.En2, GPIO.OUT)
        GPIO.setup(self.In1, GPIO.OUT)
        GPIO.setup(self.In2, GPIO.OUT)
        GPIO.setup(self.In3, GPIO.OUT)
        GPIO.setup(self.In4, GPIO.OUT)

        GPIO.output(self.IR, GPIO.LOW)
        GPIO.output(self.LED, GPIO.LOW)
        GPIO.output(self.XENON, GPIO.LOW)
        GPIO.output(self.En1, GPIO.LOW)
        GPIO.output(self.En2, GPIO.LOW)
        GPIO.output(self.In1, GPIO.LOW)
        GPIO.output(self.In2, GPIO.LOW)
        GPIO.output(self.In3, GPIO.LOW)
        GPIO.output(self.In4, GPIO.LOW)

    def getState(self, gpio):
        return GPIO.input(gpio)

    def turnOn(self, gpio):
        GPIO.output(gpio, GPIO.HIGH)

    def turnOff(self, gpio):
        GPIO.output(gpio, GPIO.LOW)

    def enable_IRCUT(self, ircut):
        if(ircut == 0): GPIO.output(self.En1,GPIO.HIGH)
        else: GPIO.output(self.En2,GPIO.HIGH)

    def disable_IRCUT(self, ircut):
        if(ircut == 0): GPIO.output(self.En1,GPIO.LOW)
        else: GPIO.output(self.En2,GPIO.LOW)

    def turnOn_IRCUT(self, ircut):
        if(ircut == 0):
            if not GPIO.input(self.En1): self.enable_IRCUT(ircut)
            #GPIO.output(self.En1, GPIO.HIGH)
            GPIO.output(self.In1, GPIO.HIGH)
            GPIO.output(self.In2, GPIO.LOW)
        else:
            if not GPIO.input(self.En2): self.enable_IRCUT(ircut)
            #GPIO.output(self.En2, GPIO.HIGH)
            GPIO.output(self.In3, GPIO.LOW)
            GPIO.output(self.In4, GPIO.HIGH)

    def turnOff_IRCUT(self, ircut):
        if(ircut == 0):
            if not GPIO.input(self.En1): self.enable_IRCUT(ircut)
            #GPIO.output(self.En1, GPIO.HIGH)
            GPIO.output(self.In1, GPIO.LOW)
            GPIO.output(self.In2, GPIO.HIGH)
        else:
            if not GPIO.input(self.En2): self.enable_IRCUT(ircut)
            #GPIO.output(self.En2, GPIO.HIGH)
            GPIO.output(self.In3, GPIO.HIGH)
            GPIO.output(self.In4, GPIO.LOW)

    def takePicture(self, mode, name):
        # Check if directory exist, if not create it
        if not self.checkDayDirectory(): self.createDayDirectory()
        
        day, month, year = self.getDateFormat()
        # Thermal Mode
        if(mode == 0):
            self.turnOff_IRCUT(self.IrCut)
            self.turnOn(self.IR)
            self.wait(0.45) # Wait 0.45 seconds
            self.cam.capture("photos_{}-{}-{}/ir/{}.jpg".format(day, month, year, name)) # Take photo and give it a name
            self.thermalPhoto("photos_{}-{}-{}/thermalPhotos/{}".format(day, month, year, name)) #get thermal cam readings
            self.wait(0.1) # Wait 100ms
            self.turnOff(self.IR)
            self.wait(0.05) # Wait 50ms

        # LED mode
        elif(mode == 1):
            self.turnOn_IRCUT(self.IrCut)
            self.turnOn(self.LED)
            self.wait(0.45) # Wait 0.45 seconds
            self.cam.capture("photos_{}-{}-{}/led/{}.jpg".format(day, month, year, name)) # Take photo and give it a name
            self.wait(0.1) # Wait 100ms
            self.turnOff(self.LED)
            self.turnOff_IRCUT(self.IrCut)
            self.wait(0.05) # Wait 50ms

        # XENON mode
        elif(mode == 2):
            self.turnOn_IRCUT(self.IrCut)
            self.turnOn(self.XENON)
            self.wait(0.45) # Wait 0.45 seconds
            self.cam.capture("photos_{}-{}-{}/xenon/{}.jpg".format(day, month, year, name)) # Take photo and give it a name
            self.wait(0.1) # Wait 100ms
            self.turnOff(self.XENON)
            self.turnOff_IRCUT(self.IrCut)
            self.wait(0.05) # Wait 50ms
            
    def photoSequence(self, name):
        # Check if directory exist, if not create it
        if not self.checkDayDirectory(): self.createDayDirectory()
        day, month, year = self.getDateFormat()
        self.cam.start_preview()
        self.turnOn_IRCUT(self.IrCut)
        self.turnOn(self.LED)
        self.wait(0.45) # Wait 0.45 seconds
        self.cam.capture("photos_{}-{}-{}/led/{}.jpg".format(day, month, year, name)) # Take photo and give it a name
        self.wait(0.1) # Wait 100ms
        self.turnOff(self.LED)
        self.turnOn(self.XENON)
        self.wait(0.45) # Wait 0.45 seconds
        self.cam.capture("photos_{}-{}-{}/xenon/{}.jpg".format(day, month, year, name)) # Take photo and give it a name
        self.wait(0.1) # Wait 100ms
        self.turnOff(self.XENON)
        self.turnOff_IRCUT(self.IrCut)
        self.turnOn(self.IR)
        self.wait(0.45) # Wait 0.45 seconds        
        self.cam.capture("photos_{}-{}-{}/ir/{}.jpg".format(day, month, year, name)) # Take photo and give it a name
        self.thermalPhoto("photos_{}-{}-{}/thermalPhotos/{}".format(day, month, year, name)) #get thermal cam readings
        self.wait(0.1) # Wait 100ms
        self.turnOff(self.IR)
        self.wait(0.05) # Wait 50ms
        self.cam.stop_preview()
        
    def thermalPhoto(self, name):
        # Get lecture
        thermalPixels1 = self.thermalCam1.readPixels()
        thermalPixels2 = self.thermalCam2.readPixels()
        
        # Process and join data
        thermalPixels1 = np.reshape(thermalPixels1, (8,8))
        thermalPixels2 = np.reshape(thermalPixels2, (8,8))
        thermalJoin = np.concatenate((thermalPixels1, thermalPixels2), axis=0)
        
        np.savetxt("{}.csv".format(name), thermalJoin, fmt="%.2f", delimiter=",")
    
    def checkDayDirectory(self):
        day, month, year = self.getDateFormat()
        if os.path.exists('photos_{}-{}-{}'.format(day, month, year)): return True
        else: return False
        
    def createDayDirectory(self):
        day, month, year = self.getDateFormat()
        os.makedirs('photos_{}-{}-{}'.format(day, month, year))
        os.makedirs('photos_{}-{}-{}/thermalPhotos'.format(day, month, year))
        os.makedirs('photos_{}-{}-{}/led'.format(day, month, year))
        os.makedirs('photos_{}-{}-{}/xenon'.format(day, month, year))
        os.makedirs('photos_{}-{}-{}/ir'.format(day, month, year))
    
    def getDateFormat(self):
        self.now = datetime.now()
        
        if self.now.day<10: day = "0{}".format(self.now.day)
        else: day = "{}".format(self.now.day) 
        if self.now.month<10: month = "0{}".format(self.now.month)
        else: month = "{}".format(self.now.month)
        year = self.now.year
        
        return day, month, year
    
    def wait(self, timeout):
        actualTime = time.time()
        while(time.time()-actualTime<timeout): continue
            
    def close(self):
        GPIO.cleanup() # Clean GPIO
        self.cam.close()

counter = time.time()
print(counter)
grower = Grower()
grower.photoSequence(1)
grower.close()
print(time.time()-counter)