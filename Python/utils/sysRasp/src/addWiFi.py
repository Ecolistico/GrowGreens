import sys
sys.path.insert(0, '../')
from sysRasp import runShellCommand
import json

with open("/var/www/html/data/data.json", "r") as f:
    data = json.load(f)
    ssid = data["ssid"]
    password = data["password"]
    f.close()

if(runShellCommand('cp /etc/wpa_supplicant/wpa_supplicant.conf wpa_supplicant.conf')):
    print("Getting WiFi settings...")
else: print("Error: Cannot get WiFi settings")

with open("wpa_supplicant.conf", "a+") as f:
    f.write("\nnetwork={\n")
    f.write('ssid="{}"\n'.format(ssid))
    f.write('psk="{}"\n'.format(password))
    f.write("}")
    f.close

if(runShellCommand('sudo cp wpa_supplicant.conf /etc/wpa_supplicant/wpa_supplicant.conf')):
    print("Adding new WiFi settings...")
else: print("Error: Cannot add new WiFi settings")

if(runShellCommand('rm wpa_supplicant.conf')):
    print("Delating duplicated files...")
else: print("Error: Cannot delate duplicated files")

if(runShellCommand('python WiFiconfig.py')):
    print("Restarting WiFi...")
else: print("Error: Cannot restart WiFi")
