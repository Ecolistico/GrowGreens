import sys
import os
actualDirectory = os.getcwd()
if not actualDirectory.endswith('src'): sys.path.insert(0, './src/')
from sysGrower import getSerial
import shutil

SSID = "ssid=tocani-"+ getSerial() + "\n"

filename = "hostapd.conf.ap"
with open(filename, "w") as f:
    f.write("interface=wlan0\n")
    f.write("driver=nl80211\n")
    f.write("hw_mode=g\n")
    f.write("channel=6\n")
    f.write("ieee80211n=1\n")
    f.write("wmm_enabled=0\n")
    f.write("macaddr_acl=0\n")
    f.write("ignore_broadcast_ssid=0\n")
    f.write("auth_algs=1\n")
    f.write("wpa=2\n")
    f.write("wpa_key_mgmt=WPA-PSK\n")
    f.write("wpa_pairwise=TKIP\n")
    f.write("rsn_pairwise=CCMP\n")
    f.write(SSID)
    f.write("wpa_passphrase=123tocani456\n")

shutil.copyfile(filename, '/etc/hostapd/{}'.format(filename))