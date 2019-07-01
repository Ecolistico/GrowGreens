import sys
import os
actualDirectory = os.getcwd()
if(actualDirectory.endswith('Grower')): sys.path.insert(0, '../sysRasp/')
elif(actualDirectory.endswith('src')): sys.path.insert(0, '../../sysRasp/')
from sysRasp import runShellCommand

runShellCommand('sudo cp /etc/wpa_supplicant/wpa_supplicant.conf.orig /etc/wpa_supplicant/wpa_supplicant.conf')
