#!/bin/bash
sleep 0
export DISPLAY=:0.0
lxterminal -t "GrowGreens" --working-directory=/home/pi/Documents/GrowGreens/Python/Master --command="/bin/bash -c 'python ./growMaster.py start'"
