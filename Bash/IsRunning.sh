#!/bin/bash
program1="$(ps ef | grep '[p]ython Control_Sensor.py' | awk '{print $2}')"

if [ "${program1}" != "" ]
then
	echo "Control_Sensor is running" #> /home/pi/Documents/Serial_ArdiRasp/Log/IsRunning.out
else
	echo "Openning Control_Sensor" #> /home/pi/Documents/Serial_ArdiRasp/Log/IsRunning.out
	lxterminal -t WallG2 --command="/bin/bash -c ' /home/pi/Documents/Serial_ArdiRasp/Bash/Control_Sensor.sh; read'" #>> /home/pi/Documents/Serial_ArdiRasp/Log/IsRunning.log
fi
