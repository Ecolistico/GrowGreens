import RPi.GPIO as GPIO
import time

IR = 22
LED = 23
XENON = 26

GPIO.setmode(GPIO.BCM)
GPIO.setup(IR, GPIO.OUT)
GPIO.setup(LED, GPIO.OUT)
GPIO.setup(XENON, GPIO.OUT)

def testGrower():
    print("Testing grower...")
    GPIO.output(IR, True) # Turn On IR
    GPIO.output(LED, False) # Turn off LED
    GPIO.output(XENON, False) # Turn off XENON
    print("IR: Turn On")
    time.sleep(5) # Wait 5 seconds
    GPIO.output(IR, False) # Turn off IR
    GPIO.output(LED, True) # Turn on LED
    GPIO.output(XENON, False) # Turn off XENON
    print("IR: Turn Off")
    print("LED: Turn On")
    time.sleep(5) # Wait 5 seconds
    GPIO.output(IR, False) # Turn off IR
    GPIO.output(LED, False) # Turn off LED
    GPIO.output(XENON, True) # Turn on XENON
    print("LED: Turn Off")
    print("XENON: Turn On")
    time.sleep(5) # Wait 5 seconds
    GPIO.output(IR, False) # Turn off IR
    GPIO.output(LED, False) # Turn off LED
    GPIO.output(XENON, False) # Turn off XENON
    print("XENON: Turn Off")
    GPIO.cleanup()

testGrower()
