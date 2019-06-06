from sysRasp import runShellCommand

# Stop hostapd
if(runShellCommand('sudo systemctl stop hostapd')):
    print("Service: hostapd stopped")
else: print("Error: stopping hostpad service")

# Stop dnsmasq
if(runShellCommand('sudo systemctl stop dnsmasq')):
    print("Service: dnsmasq stoped")
else: print("Error: stopping hostpad service")

# Save dhcpcd.conf
if(runShellCommand('sudo cp /etc/dhcpcd.conf.orig /etc/dhcpcd.conf')):
    print("File: dhcpcd.conf saved")
else: print("Error: saving dhcpcd.conf")

# Restart dhcpcd
if(runShellCommand('sudo systemctl restart dhcpcd')):
    print("Service: dhcpcd restarted")
else: print("Error: restarting hostpad service")

# Save hostapd.conf
if(runShellCommand('sudo cp /etc/hostapd/hostapd.conf.orig /etc/hostapd/hostapd.conf')):
    print("File: hostapd.conf saved")
else: print("Error: saving hostpad.conf")

# Save hostapd settings
if(runShellCommand('sudo cp /etc/default/hostapd.orig /etc/default/hostapd')):
    print("File: hostapd-set saved")
else: print("Error: saving hostpad-set")

# Saved hostapd init
if(runShellCommand('sudo cp /etc/init.d/hostapd.orig /etc/init.d/hostapd')):
    print("File: hostapd-init saved")
else: print("Error: saving hostpad-init")

# Saved dnsmasq.conf
if(runShellCommand('sudo cp /etc/dnsmasq.conf.orig /etc/dnsmasq.conf')):
    print("File: dnsmasq.conf saved")
else: print("Error: saving dnsmasq.conf")

# Saved sysctl.conf
if(runShellCommand('sudo cp /etc/sysctl.conf.orig /etc/sysctl.conf')):
    print("File: sysctl.conf saved")
else: print("Error: saving sysctl.conf") 

# Save rc.local
if(runShellCommand('sudo cp /etc/rc.local.orig /etc/rc.local')):
    print("File: rc.local saved")
else: print("Error: saving rc.local")

# Restart dnsmasq
if(runShellCommand('sudo systemctl restart dnsmasq')):
    print("Service: dnsmasq started")
else: print("Error: starting dnsmasq service")