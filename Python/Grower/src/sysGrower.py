import os
import json
import psutil
import ipaddress
import subprocess
from uuid import getnode
from datetime import datetime
    
MQTT_PATH = '/var/www/html/data/data.json'

def getIPaddr():
    wifi_ip = subprocess.check_output(['hostname', '-I'])
    ip_str = str(wifi_ip,'utf-8')
    return ip_str[:-2]

def getSerial():
    # Extract serial from cpuinfo file
    cpuserial = "0000000000000000"
    try:
        f = open('/proc/cpuinfo','r')
        for line in f:
            if line[0:6]=='Serial':
                cpuserial = line[10:26]
        f.close()
    except:
        cpuserial = "ERROR000000000"
            
    return cpuserial

def runShellCommand(myCmD):
    try:
        os.system(myCmD)
        return 1
    except:
        return 0
    
def getOutput_ShellCommand(myCmD):
    CmD = myCmD.split()
    output = subprocess.Popen(CmD, stdout=subprocess.PIPE).communicate()[0]
    out = output.decode("utf-8")
    return out

def existFile(path):
    exist = os.path.isfile(path)
    return exist

def scanWiFi():
    ssid = []
    out = subprocess.check_output(["iwlist","wlan0","scan"])
    for line in out.split():
        line = str(line,'utf-8')
        if line.startswith('ESSID'):
            ssid.append(line.split('"')[1])
    return ssid

# Internet Check
def isWiFi():
    internet = False
    wifi_list = scanWiFi()
    wifi_connect = getOutput_ShellCommand("iwgetid -r")
    wifi_connect = wifi_connect[0:len(wifi_connect)-1]
    for i in wifi_list:
        if(i == wifi_connect):
            internet = True
            return internet
    return internet
        
# AP Check
def isAP():
    IP = getIPaddr()
    IP_AP = "192.168.1.1" # Predefined AP IPaddress
    
    AP = False
    if IP_AP in IP:
        AP = True
        return AP
    else:
        return AP

def getData_JSON(path):
    if(os.path.isfile(path)):
        with open(path) as json_file:
            data = json.load(json_file)
            return data['containerID'], data['floor'], data['mqttIP']
    else: return "", "", ""
    
def checkInterface(interface):
    inter = psutil.net_if_addrs().get(interface)[0][2]
    try:
        ip = ipaddress.ip_address(inter)
        return True
    except:
        return False
    
"""
def photoPath():
    """"""
    wifi_ip = subprocess.check_output(['hostname', '-I'])
    ip_str = str(wifi_ip,'utf-8')
    growy_name = 'Growy'
    now = datetime.now()
    if now.day<10: day = "0{}".format(now.day)
    else: day = "{}".format(now.day) 
    if now.month<10: month = "0{}".format(now.month)
    else: month = "{}".format(now.month)
    year = now.year
    date = "{}-{}-{}".format(day, month, year)
    path = "//{}/{}/{}/sequence".format(ip_str[:-2], growy_name, date) 
    return path
"""  
def getMacAddr(): return getnode()