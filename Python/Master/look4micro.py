#!/usr/bin/env python3
import sys
sys.path.insert(0, './src/')
from sysMaster import runShellCommand

try:
    name = sys.argv[1]

    runShellCommand("udevadm info --name='/dev/{}' > out.log".format(name))

    for line in open("out.log"):
        if "DEVNAME" in line:
            print(line.split("=")[1][0:-1])
            break

    runShellCommand("sudo rm out.log")

except:
    print("\033[1;31;40mError: Proporcioné el nombre del microcontrolador que esta " +
          "buscando como parámetro.\nEjemplo: ./look4micro.py generalControl")