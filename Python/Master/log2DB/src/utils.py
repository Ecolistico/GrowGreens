#!/usr/bin/env python3

# Import modules
from datetime import datetime

def readlastline(f):
    f.seek(-2, 2)              # Jump to the second last byte.
    while f.read(1) != b"\n":  # Until EOL is found ...
        f.seek(-2, 1)          # ... jump back, over the read byte plus one more.
    return f.read()            # Read all data from this point on.

def getDateTime(line):
    return datetime.strptime(line[0:19], '%Y-%m-%d %H:%M:%S')

def getDevice(line):
    return line.split(" ")[2]

def getType(line):
    return line[36:44].strip()

def getMsg(line):
    return line[45:].strip()

def getInfo(line):
    return getDateTime(line), getDevice(line), getType(line), getMsg(line)
