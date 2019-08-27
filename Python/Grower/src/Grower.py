#!/usr/bin/env python3

import os
from time import time
from picamera import PiCamera
from datetime import datetime
from numpy import reshape, concatenate, savetxt
import RPi.GPIO as GPIO
from Adafruit_AMG88xx import Adafruit_AMG88xx
from pysftp import Connection
import cozir
from sysGrower import runShellCommand, getOutput_ShellCommand, getIPaddr

"""
Functions resume:
    * Grower()- Constructor for the class
    * getState() - returns the state of some gpio
    * turnOn() - turn On some gpio
    * turnOff() - turn Off some gpio
    * turnOn_IRCUT() - activate ircut
    * turnOff_IRCUT() - desactivate ircut
    * enable_IRCUT() - enable ircut
    * disable_IRCUT() - disable ircut
    * takePicture(mode, name) - Mode(0=thermal, 1=led, 2=xenon) and Name(the name of the picture)
    * photoSequence(name) - Run the 3 modes of the takePicture function giving the same name on each picture
    * thermalPhoto(name) - Gives an csv with thermal information of the two cameras
    * enableStreaming() - Close the cam in the local program to stream over internet
    * disableStreaming() - Close the cam streaming to open the cam in local program
    * whatIsMyIP() - Returns a string with the IP addres from this device
    * sendPhotos(host, name, password, floor) - sendPhotos to server
    * close() - Cleanup the GPIO´s
"""
class Grower:
    def __init__(self, logger, ir = 22, led = 23, xenon = 26, en1 = 4, en2 = 27, in1 = 24, in2 = 18, in3 = 17, in4 = 10, thermal1Addr = 0x69, thermal2Addr = 0x68, ircut = 0):
        self.log = logger
        
        self.day = 0
        self.month = 0
        self.year = 0
        self.getDateFormat()
        
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

        self.IRCUT = ircut # Default IRCUT output. 0 for outputs 1, 2 and 1 for outputs 3, 4

        # Setting Up Thermal Cams
        self.thermalCam1 = Adafruit_AMG88xx(address=thermal1Addr) # Set thermalCam1 on its i2c addres
        self.thermalCam2 = Adafruit_AMG88xx(address=thermal2Addr) # Set thermalCam2 on its i2c addres
        # Set Cam Disable by Default
        self.camEnable = False
        # Set stream as false by default
        self.stream = False
        # Setting Up Cozir
        self.coz = cozir.Cozir(self.log)
            # For now just stable in polling mode
        if(self.coz.opMode(self.coz.polling)):
            self.log.info("Cozir: Set Mode = K{0}".format(self.coz.act_OpMode))
            # Get hum, temp and co2_filter        
        if(self.coz.setData_output(self.coz.Hum + self.coz.Temp + self.coz.CO2_filt)):
            self.log.info("Cozir: Data Mode = M{}".format(self.coz.act_OutMode))
    
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

    def turnOff_IRCUT(self, ircut):
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

    def turnOn_IRCUT(self, ircut):
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

    def wait(self, timeout):
        actualTime = time()
        while(time()-actualTime<timeout): continue
        
    def checkDayDirectory(self):
        if os.path.exists('data/photos_{}-{}-{}'.format(self.day, self.month, self.year)): return True
        else: return False
        
    def createDayDirectory(self):
        os.makedirs('data/photos_{}-{}-{}'.format(self.day, self.month, self.year))
        os.makedirs('data/photos_{}-{}-{}/thermalPhotos'.format(self.day, self.month, self.year))
        os.makedirs('data/photos_{}-{}-{}/led'.format(self.day, self.month, self.year))
        os.makedirs('data/photos_{}-{}-{}/xenon'.format(self.day, self.month, self.year))
        os.makedirs('data/photos_{}-{}-{}/ir'.format(self.day, self.month, self.year))
        os.makedirs('data/photos_{}-{}-{}/manual'.format(self.day, self.month, self.year))
        
    
    def getDateFormat(self):
        now = datetime.now()
        if now.day<10: self.day = "0{}".format(now.day)
        else: self.day = "{}".format(now.day) 
        if now.month<10: self.month = "0{}".format(now.month)
        else: self.month = "{}".format(now.month)
        self.year = now.year
    
    def whatIsMyIP(self):
        return getIPaddr()
    
    def photoPath(self):
        return "data/photos_{}-{}-{}".format(self.day, self.month, self.year) # Return folder name
    
    def thermalPhoto(self, name):
        # Check if directory exist, if not create it
        if not self.checkDayDirectory(): self.createDayDirectory()
        
        # Get lecture
        thermalPixels1 = self.thermalCam1.readPixels()
        thermalPixels2 = self.thermalCam2.readPixels()
        
        # Process and join data
        thermalPixels1 = reshape(thermalPixels1, (8,8))
        thermalPixels2 = reshape(thermalPixels2, (8,8))
        thermalJoin = concatenate((thermalPixels1, thermalPixels2), axis=0)
        
        savetxt("data/photos_{}-{}-{}/thermalPhotos/{}.csv".format(self.day, self.month, self.year, name), thermalJoin, fmt="%.2f", delimiter=",")
    
    def cam_begin(self):
        self.cam = PiCamera()
        self.camEnable = True
        
    def cam_stop(self):
        self.cam.close()
        self.camEnable = False
        
    def takePicture(self, mode, name):
        # Check if directory exist, if not create it
        if not self.checkDayDirectory(): self.createDayDirectory()
                
        # Thermal Mode
        if(mode == 0):
            self.turnOn_IRCUT(self.IRCUT)
            self.turnOn(self.IR)
            self.turnOff(self.LED)
            self.turnOff(self.XENON)
            self.wait(0.45) # Wait 0.45 seconcds
            self.cam.capture("data/photos_{}-{}-{}/ir/{}.jpg".format(self.day, self.month, self.year, name)) # Take photo and give it a name
            self.thermalPhoto("{}".format(name)) #get thermal cam readings
            self.wait(0.1) # Wait 100ms
            self.turnOff(self.IR)
            self.turnOff_IRCUT(self.IRCUT)
            self.wait(0.05) # Wait 50ms

        # LED mode
        elif(mode == 1):
            self.turnOff_IRCUT(self.IRCUT)
            self.turnOn(self.LED)
            self.turnOff(self.IR)
            self.turnOff(self.XENON)   
            self.wait(0.45) # Wait 0.45 seconds
            self.cam.capture("data/photos_{}-{}-{}/led/{}.jpg".format(self.day, self.month, self.year, name)) # Take photo and give it a name
            self.wait(0.1) # Wait 100ms
            self.turnOff(self.LED)
            self.wait(0.05) # Wait 50ms

        # XENON mode
        elif(mode == 2):
            self.turnOff_IRCUT(self.IRCUT)
            self.turnOn(self.XENON)
            self.turnOff(self.IR)
            self.turnOff(self.LED)
            self.wait(0.45) # Wait 0.45 seconds
            self.cam.capture("data/photos_{}-{}-{}/xenon/{}.jpg".format(self.day, self.month, self.year, name)) # Take photo and give it a name
            self.wait(0.1) # Wait 100ms
            self.turnOff(self.XENON)
            self.wait(0.05) # Wait 50ms
        
        elif(mode == 3):
            self.wait(0.45) # Wait 0.45 seconds
            self.cam.capture("data/photos_{}-{}-{}/manual/{}.jpg".format(self.day, self.month, self.year, name)) # Take photo and give it a name
            self.wait(0.1) # Wait 100ms
            
    def photoSequence(self, name):
        # Check if directory exist, if not create it
        if not self.checkDayDirectory(): self.createDayDirectory()

        #self.cam.start_preview()
        self.turnOff_IRCUT(self.IRCUT)
        self.turnOn(self.LED)
        self.turnOff(self.IR)
        self.turnOff(self.XENON)
        self.wait(0.45) # Wait 0.45 seconds
        self.cam.capture("data/photos_{}-{}-{}/led/{}.jpg".format(self.day, self.month, self.year, name)) # Take photo and give it a name
        self.wait(0.1) # Wait 100ms
        self.turnOff(self.LED)
        self.turnOn(self.XENON)
        self.wait(0.45) # Wait 0.45 seconds
        self.cam.capture("data/photos_{}-{}-{}/xenon/{}.jpg".format(self.day, self.month, self.year, name)) # Take photo and give it a name
        self.wait(0.1) # Wait 100ms
        self.turnOff(self.XENON)
        self.turnOn_IRCUT(self.IRCUT)
        self.turnOn(self.IR)
        self.wait(0.45) # Wait 0.45 seconds        
        self.cam.capture("data/photos_{}-{}-{}/ir/{}.jpg".format(self.day, self.month, self.year, name)) # Take photo and give it a name
        self.thermalPhoto("{}".format(name)) #get thermal cam readings
        self.wait(0.1) # Wait 100ms
        self.turnOff(self.IR)
        self.turnOff_IRCUT(self.IRCUT)
        self.wait(0.05) # Wait 50ms
        #self.cam.stop_preview()
            
    def enableStreaming(self):
        if not self.stream:
            self.stream = True
            # Disconnecting cam from this program
            if self.camEnable: self.cam_stop()
            
            # Create camera1 in motionEye
            runShellCommand('sudo cp ../sysRasp/configFiles_MotionEye/camera-1.conf /etc/motioneye/camera-1.conf')
            
            # Change to python 2
                # Check Alternatives
            alt = getOutput_ShellCommand('update-alternatives --list python')    
            if(alt == ''):
                runShellCommand("sudo update-alternatives --install /usr/bin/python python /usr/bin/python2.7 1")
                runShellCommand("sudo update-alternatives --install /usr/bin/python python /usr/bin/python3.5 2")
                # Set Python Version
            runShellCommand("sudo update-alternatives --set python /usr/bin/python2.7")
            
            # Restart motionEye
            runShellCommand('sudo systemctl restart motioneye')
            
            # Change to python 3
                # Check Alternatives
            alt = getOutput_ShellCommand('update-alternatives --list python')    
            if(alt == ''):
                runShellCommand("sudo update-alternatives --install /usr/bin/python python /usr/bin/python2.7 1")
                runShellCommand("sudo update-alternatives --install /usr/bin/python python /usr/bin/python3.5 2")
                # Set Python Version
            runShellCommand("sudo update-alternatives --set python /usr/bin/python3.5")
        
    def disableStreaming(self):
        if self.stream:
            self.stream = False
            # Remove camera1 in motionEye
            runShellCommand('sudo rm /etc/motioneye/camera-1.conf')
            
            # Change to python 2
                # Check Alternatives
            alt = getOutput_ShellCommand('update-alternatives --list python')    
            if(alt == ''):
                runShellCommand("sudo update-alternatives --install /usr/bin/python python /usr/bin/python2.7 1")
                runShellCommand("sudo update-alternatives --install /usr/bin/python python /usr/bin/python3.5 2")
                # Set Python Version
            runShellCommand("sudo update-alternatives --set python /usr/bin/python2.7")
            
            # Restart motionEye
            runShellCommand('sudo systemctl restart motioneye')
            
            # Change to python 3
                # Check Alternatives
            alt = getOutput_ShellCommand('update-alternatives --list python')    
            if(alt == ''):
                runShellCommand("sudo update-alternatives --install /usr/bin/python python /usr/bin/python2.7 1")
                runShellCommand("sudo update-alternatives --install /usr/bin/python python /usr/bin/python3.5 2")
                # Set Python Version
            runShellCommand("sudo update-alternatives --set python /usr/bin/python3.5")
            
            # Connecting camara to this program
            if not self.camEnable: self.cam_begin()
    
    def sendPhotos(self, host, name, pskw, floor = 0):
        try:
            with Connection(host, username=name, password=pskw) as sftp:
                if(sftp.isdir('/home/pi/Documents/Master/photos')):
                    sftp.chdir('/home/pi/Documents/Master/photos')
                    if not sftp.isdir("Grower{}".format(floor)):
                        sftp.makedirs("Grower{}".format(floor))
                    sftp.chdir("Grower{}".format(floor))
                    sftp.makedirs(self.photoPath())
                    sftp.put_r(self.photoPath(), '/home/pi/Documents/Master/photos/Grower{}/{}'.format(floor, self.photoPath()), preserve_mtime=False)
                    return True
                else: return False
        except:
            return False
        
    def close(self):
        GPIO.cleanup() # Clean GPIO
        if self.camEnable: self.cam_stop() # Disconnecting cam from this program
