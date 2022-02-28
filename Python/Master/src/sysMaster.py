#!/usr/bin/env python3

# Import directories
import os
import sys
import select
import binascii
import subprocess
import urllib.request
import xml.etree.ElementTree as ET

def GetLine(Block=False):
  if Block or select.select([sys.stdin], [], [], 0) == ([sys.stdin], [], []):
      return input()

def clear():
    # for windows
    if os.name == 'nt':
        _ = os.system('cls')
    # for mac and linux(here, os.name is 'posix')
    else:
        _ = os.system('clear')

def ping():
    if (os.name == 'nt'): # for windows
        pg = "ping -n 1"
    else : # for mac and linux(here, os.name is 'posix')
        pg = "ping -c 1"
    return pg

def isWiFi():
    try:
        urllib.request.urlopen('http://216.58.192.142', timeout=1)
        return True
    except urllib.request.URLError as err:
        return False

def getIPaddr():
    wifi_ip = subprocess.check_output(['hostname', '-I'])
    return wifi_ip

def getIPaddr1():
    wifi_ip = subprocess.check_output(['hostname', '-I'])
    ip = wifi_ip.decode("utf-8").split(' ')[0]
    return ip

def getGatewayIPaddr():
    gw_ip = subprocess.check_output(['ip', 'route'])
    ip = gw_ip.decode("utf-8").split(' ')[2]
    return ip

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

def scan_for_hosts(ip_range):
    """Scan the given IP address range using Nmap and return the result
    in XML format.
    """
    nmap_args = ['sudo', 'nmap', '-n', '-sP', '-oX', '-', ip_range]
    return subprocess.check_output(nmap_args)

def find_ip_address_for_mac_address(xml, mac_address):
    """Parse Nmap's XML output, find the host element with the given
    MAC address, and return that host's IP address (or `None` if no
    match was found).
    """
    host_elems = ET.fromstring(xml).iter('host')
    host_elem = find_host_with_mac_address(host_elems, mac_address)
    if host_elem is not None:
        return find_ip_address(host_elem)

def find_host_with_mac_address(host_elems, mac_address):
    """Return the first host element that contains the MAC address."""
    for host_elem in host_elems:
        if host_has_mac_address(host_elem, mac_address):
            return host_elem

def host_has_mac_address(host_elem, wanted_mac_address):
    """Return true if the host has the given MAC address."""
    found_mac_address = find_mac_address(host_elem)
    return (
        found_mac_address is not None and
        found_mac_address.lower() == wanted_mac_address.lower()
    )

def find_mac_address(host_elem):
    """Return the host's MAC address."""
    return find_address_of_type(host_elem, 'mac')

def find_ip_address(host_elem):
    """Return the host's IP address."""
    return find_address_of_type(host_elem, 'ipv4')

def find_address_of_type(host_elem, type_):
    """Return the host's address of the given type, or `None` if there
    is no address element of that type.
    """
    address_elem = host_elem.find('./address[@addrtype="{}"]'.format(type_))
    if address_elem is not None:
        return address_elem.get('addr')

# For an array of bytes returns a list of hexadecimals values
def splitBytes2Hex(data):
    hex_var = binascii.hexlify(data)
    bytes_hex = [hex_var[i:i+2] for i in range(0, len(hex_var),2)]
    return bytes_hex

# Split byte into its bits, return a list of binary
def splitIntoBits(byte, inverted = False):
    bits = [byte >> i & 1 for i in range(8)]
    if inverted: bits.reverse()
    return bits

# Convert a list of bits into a single number
def listBytestoDecimal(listBytes):
    dec = 0
    for i in listBytes:
        dec = dec*256 + int(i, 16)
    return dec