import sys
import os
actualDirectory = os.getcwd()
if not actualDirectory.endswith('src'): sys.path.insert(0, './src/')
from sysGrower import runShellCommand

runShellCommand('sudo cp /etc/wpa_supplicant/wpa_supplicant.conf.orig /etc/wpa_supplicant/wpa_supplicant.conf')
