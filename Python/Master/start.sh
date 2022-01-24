#!/bin/bash
sleep 1
export DISPLAY=:0.0
lxterminal -t "GrowGreens" --geometry=100x24 --working-directory=/home/pi/Documents/GrowGreens/Python/Master --command="/bin/bash -c 'python3 ./growMaster.py start'"
