import time
import os
import RPi.GPIO as GPIO
from Adafruit_AMG88xx import Adafruit_AMG88xx

class Grower:
    def __init__(self, ir = 22, led = 23, xenon = 26, en1 = 4, en2 = 27, in1 = 24, in2 = 18, in3 = 17, in4 = 10, thermal1Addr = 0x69, thermal2Addr = 0x68, ircut = 0):
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
            #if not GPIO.input(self.En1): self.enable_IRCUT(ircut)
            GPIO.output(self.En1, GPIO.HIGH)
            GPIO.output(self.In1, GPIO.HIGH)
            GPIO.output(self.In2, GPIO.LOW)
        else:
            #if not GPIO.input(self.En2): self.enable_IRCUT(ircut)
            GPIO.output(self.En2, GPIO.HIGH)
            GPIO.output(self.In3, GPIO.HIGH)
            GPIO.output(self.In4, GPIO.LOW)
        time.sleep(2)

    def turnOff_IRCUT(self, ircut):
        if(ircut == 0):
            #if not GPIO.input(self.En1): self.enable_IRCUT(ircut)
            GPIO.output(self.En1, GPIO.HIGH)
            GPIO.output(self.In1, GPIO.LOW)
            GPIO.output(self.In2, GPIO.HIGH)
        else:
            #if not GPIO.input(self.En2): self.enable_IRCUT(ircut)
            GPIO.output(self.En2, GPIO.HIGH)
            GPIO.output(self.In3, GPIO.LOW)
            GPIO.output(self.In4, GPIO.HIGH)
        time.sleep(2)

    def takePicture(self, mode):
        # Thermal Mode
        if(mode == 0):
            print("Picture: Thermal Mode")
            print("IRCUT: Turn Off")
            self.turnOff_IRCUT(self.IrCut)
            print("IR: Turn On")
            self.turnOn(self.IR)
            time.sleep(2) #Wait 2 seconds
            #os.system('sudo raspistill -o {}}.jpg -t 1'.format()) # Take picture and give it a name
            #get thermal cam readings
            print("IR: Turn Off")
            self.turnOff(self.IR)
            print("IRCUT: Disable")
            self.disable_IRCUT(self.IrCut)
            print()
            time.sleep(2) # Wait 2 seconds

        # LED mode
        elif(mode == 1):
            print("Picture: LED Mode")
            print("IRCUT: Turn On")
            self.turnOn_IRCUT(self.IrCut)
            print("LED: Turn On")
            self.turnOn(self.LED)
            time.sleep(2) #Wait 2 seconds
            #os.system('sudo raspistill -o {}}.jpg -t 1'.format()) # Take picture and give it a name
            print("LED: Turn Off")
            self.turnOff(self.LED)
            print("IRCUT: Turn Off and Disabled")
            self.turnOff_IRCUT(self.IrCut)
            self.disable_IRCUT(self.IrCut)
            print()
            time.sleep(2) # Sleep 2 seconds

        # XENON mode
        elif(mode == 2):
            print("Picture: XENON Mode")
            print("IRCUT: Turn On")
            self.turnOn_IRCUT(self.IrCut)
            print("XENON: Turn On")
            self.turnOn(self.XENON)
            time.sleep(2) #Wait 2 seconds
            #os.system('sudo raspistill -o {}}.jpg -t 1'.format()) # Take picture and give it a name
            print("XENON: Turn Off")
            self.turnOff(self.XENON)
            print("IRCUT: Turn Off and disabled")
            self.turnOff_IRCUT(self.IrCut)
            self.disable_IRCUT(self.IrCut)
            print()
            time.sleep(2) # Sleep 2 seconds

    def close(self):
        GPIO.cleanup() # Clean GPIO

grower1 = Grower()
grower1.turnOn_IRCUT(0)
#time.sleep(5)
grower1.turnOff_IRCUT(0)
#time.sleep(5)
#grower1.takePicture(0)
#grower1.takePicture(1)
#grower1.takePicture(2)
grower1.close()
