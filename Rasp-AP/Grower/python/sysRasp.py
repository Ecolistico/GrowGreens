import os
import subprocess
import urllib.request

def isWiFi():
    try:
        urllib.request.urlopen('http://216.58.192.142', timeout=1)
        return True
    except urllib.request.URLError as err:
        return False

def getIPaddr():
    wifi_ip = subprocess.check_output(['hostname', '-I'])
    return wifi_ip

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
    if(out[-1] == "\n"):
        out = out[:-1]
    return out

def existFile(path):
    exist = os.path.isfile(path)
    return exist
