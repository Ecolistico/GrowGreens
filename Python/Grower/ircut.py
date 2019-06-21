import os
import RPi.GPIO as GPIO
import time
from time import sleep

start = time.time()

try:
    GPIO.setmode(GPIO.BCM)

    Motor1A = 23
    Motor1B = 24
    Motor1E = 25

    GPIO.setup(Motor1A,GPIO.OUT)
    GPIO.setup(Motor1B,GPIO.OUT)
    GPIO.setup(Motor1E,GPIO.OUT)

    #print('Taking first picture')
    os.system('sudo raspistill -o clsd.jpg -t 1')

    #print('Opening shutter')
    GPIO.output(Motor1A,GPIO.HIGH)
    GPIO.output(Motor1B,GPIO.LOW)
    GPIO.output(Motor1E,GPIO.HIGH)

    sleep(1.5)

    #print('Taking second picture')
    os.system('sudo raspistill -o opnd.jpg -t 1')

    #print('Closing shutter')
    GPIO.output(Motor1A,GPIO.LOW)
    GPIO.output(Motor1B,GPIO.HIGH)
    GPIO.output(Motor1E,GPIO.HIGH)

    sleep(0.5)

    #print('stop')
    GPIO.output(Motor1E,GPIO.LOW)

    GPIO.cleanup()
    end = time.time()
    print(end - start)

except: KeyboardInterrupt
