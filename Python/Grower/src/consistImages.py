from time import sleep
from picamera import PiCamera

name = str(input('Filter name: '))
camera = PiCamera(resolution=(1920, 1080), framerate=30)

# Set ISO to the desired value
camera.iso = 200

# Wait for the automatic gain control to settle
sleep(3)

# Now fix the values
camera.shutter_speed = camera.exposure_speed
camera.exposure_mode = 'off'

redAWB = 1.4
blueAWB = 0.5
customGains = (redAWB, blueAWB)
camera.awb_mode = "off"
camera.awb_gains = customGains

# Finally, take several photos with the fixed settings
camera.capture_sequence([name + '%02d.png' % i for i in range(5)])
