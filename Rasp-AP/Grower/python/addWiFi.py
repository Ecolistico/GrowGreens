from sysRasp import runShellCommand
import json

with open("../data/data.json", "r") as f:
    data = json.load(f)
    ssid = data["ssid"]
    password = data["password"]
    f.close()

if(runShellCommand('cp /etc/wpa_supplicant/wpa_supplicant.conf wpa_supplicant.conf')):
    print("Getting WiFi settings...")
else: print("Error: Cannot get WiFi settings")

with open("wpa_supplicant.conf", "a+") as f:
    f.write("\nnetwork={\n")
    f.write('\tssid="{}"\n'.format(ssid))
    f.write('\tpsk="{}"\n'.format(password))
    f.write('\tkey_mgmt=WPA-PSK\n')
    f.write("}")
    f.close

if(runShellCommand('sudo cp wpa_supplicant.conf /etc/wpa_supplicant/wpa_supplicant.conf')):
    print("Adding new WiFi settings...")
else: print("Error: Cannot add new WiFi settings")

if(runShellCommand('rm wpa_supplicant.conf')):
    print("Delating duplicated files...")
else: print("Error: Cannot delate duplicated files")
