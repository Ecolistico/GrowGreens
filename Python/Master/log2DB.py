#!/usr/bin/env python3

# Import modules
import os
import json
from datetime import datetime

def readlastline(f):
    f.seek(-2, 2)              # Jump to the second last byte.
    while f.read(1) != b"\n":  # Until EOL is found ...
        f.seek(-2, 1)          # ... jump back, over the read byte plus one more.
    return f.read()            # Read all data from this point on.

# Get last line from conf file
if (os.path.exists('log2DB.conf')):
    with open('log2DB.conf') as f: lastLine = f.readline()
else: lastLine = None

# Get Datetime from line
if lastLine != None: 
    lastDatetime = datetime.strptime(lastLine[0:19], '%Y-%m-%d %H:%M:%S')
    print(lastLine[0:19])

# Control variable
checkFile = False

# Check if temp/temp/growMaster.log.2 dir exists
if (os.path.exists('temp/growMaster.log.2')): 
    # Get first and last line of file
    with open('temp/growMaster.log.2', "rb") as f:
        first = f.readline()
        last = readlastline(f)
        print(first)
        print(last)
    # Get dateTimes from lines
    firstDT = datetime.strptime(first[0:19], '%Y-%m-%d %H:%M:%S')
    lastDT = datetime.strptime(last[0:19], '%Y-%m-%d %H:%M:%S')
    # Check if last dateTime in file is between start and end dateTime of log
    if(firstDT<=lastDatetime and lastDT>=lastDatetime):
        print('We have to update database from log file 2')
        checkFile = True
        """""
        # Get lines to be checked
        with open('temp/growMaster.log.2', "rb") as f:
            lines = f.readlines()
            lines = lines[lines.index(last):]
        """""
    else: print('Our dateTime is not in log file 2')

# Check if temp/temp/growMaster.log.1 dir exists
elif(os.path.exists('temp/growMaster.log.1'):
    # Get first and last line of file
    with open('temp/growMaster.log.1', "rb") as f:
        first = f.readline()
        last = readlastline(f)
        print(first)
        print(last)
    # Get dateTimes from lines
    firstDT = datetime.strptime(first[0:19], '%Y-%m-%d %H:%M:%S')
    lastDT = datetime.strptime(last[0:19], '%Y-%m-%d %H:%M:%S')
    # Check if last dateTime in file is between start and end dateTime of log
    if((firstDT<=lastDatetime and lastDT>=lastDatetime) or checkFile):
        print('We have to update database from log file 1')
        checkFile = True
        """""
        # Get lines to be checked
        with open('temp/growMaster.log.1', "rb") as f:
            lines = f.readlines()
            lines = lines[lines.index(last):]
        """""
    else: print('Our dateTime is not in log file 1')

# Check if temp/temp/growMaster.log dir exists
elif(os.path.exists('temp/growMaster.log'):
    # Get first and last line of file
    with open('temp/growMaster.log', "rb") as f:
        first = f.readline()
        last = readlastline(f)
        print(first)
        print(last)
    # Get dateTimes from lines
    firstDT = datetime.strptime(first[0:19], '%Y-%m-%d %H:%M:%S')
    lastDT = datetime.strptime(last[0:19], '%Y-%m-%d %H:%M:%S')
    # Check if last dateTime in file is between start and end dateTime of log
    if((firstDT<=lastDatetime and lastDT>=lastDatetime) or checkFile):
        print('We have to update database from log file 0')
        checkFile = True
        """""
        # Get lines to be checked
        with open('temp/growMaster.log', "rb") as f:
            lines = f.readlines()
            lines = lines[lines.index(last):]
        """""
    else: print('Our dateTime is not in log file 0')
# Else nothing to check
else: print('No log file to check')