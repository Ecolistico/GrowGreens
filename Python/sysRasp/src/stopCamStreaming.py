import sys
sys.path.insert(0, '../')
from sysRasp import runShellCommand

# Remove camera1 in motionEye
runShellCommand('sudo rm /etc/motioneye/camera-1.conf')
# Change to python 2
runShellCommand('sudo python python3to2.py')
# Restart motionEye
runShellCommand('sudo systemctl restart motioneye')
# Change to python 3
runShellCommand('sudo python python2to3.py')