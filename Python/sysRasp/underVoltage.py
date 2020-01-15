#!/usr/bin/env python3

import subprocess
from time import sleep
from datetime import datetime

while True:
    now = datetime.now()
    CmD = "/opt/vc/bin/vcgencmd get_throttled".split()
    output = subprocess.Popen(CmD, stdout=subprocess.PIPE).communicate()[0]
    out = output.decode("utf-8")
    if len(out.split("=")[1])>5:
        if out.split("=")[1][6] == "5": print("{} Under Voltage Detected".format(now.strftime("%d-%b-%Y %H:%M:%S.%f")))
    sleep(0.2)