import sys
import os
actualDirectory = os.getcwd()
if(actualDirectory.endswith('Grower')): sys.path.insert(0, '../sysRasp/')
elif(actualDirectory.endswith('src')): sys.path.insert(0, '../../sysRasp/')
from sysRasp import getSerial
import shutil

SSID = "ssid=tocani-"+ getSerial() + "\n"

filename = "hostapd.conf.ap"
file = open(filename, "w")

file.write("interface=wlan0\n")
file.write("driver=nl80211\n")
file.write("hw_mode=g\n")
file.write("channel=6\n")
file.write("ieee80211n=1\n")
file.write("wmm_enabled=0\n")
file.write("macaddr_acl=0\n")
file.write("ignore_broadcast_ssid=0\n")
file.write("auth_algs=1\n")
file.write("wpa=2\n")
file.write("wpa_key_mgmt=WPA-PSK\n")
file.write("wpa_pairwise=TKIP\n")
file.write("rsn_pairwise=CCMP\n")
file.write(SSID)
file.write("wpa_passphrase=123tocani456\n")


file.close()

shutil.copyfile(filename, '/etc/hostapd/{}'.format(filename))