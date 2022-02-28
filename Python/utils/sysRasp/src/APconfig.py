import sys
sys.path.insert(0, '../')
from sysRasp import runShellCommand, existFile

# Stop hostapd
if(runShellCommand('sudo systemctl stop hostapd')):
    print("Service: hostapd stopped")
else: print("Error: stopping hostpad service")

# Stop dnsmasq
if(runShellCommand('sudo systemctl stop dnsmasq')):
    print("Service: dnsmasq stoped")
else: print("Error: stopping hostpad service")

# Save dhcpcd.conf
if(runShellCommand('sudo cp /etc/dhcpcd.conf.ap /etc/dhcpcd.conf')):
    print("File: dhcpcd.conf saved")
else: print("Error: saving dhcpcd.conf")

# Restart dhcpcd
if(runShellCommand('sudo systemctl restart dhcpcd')):
    print("Service: dhcpcd restarted")
else: print("Error: restarting hostpad service")

# Check if hostapd.conf exist
if(existFile('/etc/hostapd/hostapd.conf.ap')==False):
    # Create file
    if(runShellCommand('sudo python hostapdConfig.py')):
        print("File: hostapd.conf created")
    else: print("Error: creating hostpad.conf")
    
# Save hostapd.conf
if(runShellCommand('sudo cp /etc/hostapd/hostapd.conf.ap /etc/hostapd/hostapd.conf')):
    print("File: hostapd.conf saved")
else: print("Error: saving hostpad.conf")

# Save hostapd settings
if(runShellCommand('sudo cp /etc/default/hostapd.ap /etc/default/hostapd')):
    print("File: hostapd-set saved")
else: print("Error: saving hostpad-set")

# Saved hostapd init
if(runShellCommand('sudo cp /etc/init.d/hostapd.ap /etc/init.d/hostapd')):
    print("File: hostapd-init saved")
else: print("Error: saving hostpad-init")

# Saved dnsmasq.conf
if(runShellCommand('sudo cp /etc/dnsmasq.conf.ap /etc/dnsmasq.conf')):
    print("File: dnsmasq.conf saved")
else: print("Error: saving dnsmasq.conf")

# Saved sysctl.conf
if(runShellCommand('sudo cp /etc/sysctl.conf.ap /etc/sysctl.conf')):
    print("File: sysctl.conf saved")
else: print("Error: saving sysctl.conf")

# Restart sysctl
if(runShellCommand('sudo sh -c "echo 1 > /proc/sys/net/ipv4/ip_forward"')):
    print("Service: sysctl restarted")
else: print("Error: restarting sysctl")

# IPtables rules
# Create Rule
if(runShellCommand('sudo iptables -t nat -A POSTROUTING -o eth0 -j MASQUERADE')):
    print("IPTABLES: rule created")
else: print("Error: creating iptables rule")

# Save Rule
if(runShellCommand('sudo sh -c "iptables-save > /etc/iptables.ipv4.nat"')):
    print("IPTABLES: rule saved")
else: print("Error: saving iptables rule")

# Save rc.local
if(runShellCommand('sudo cp /etc/rc.local.ap /etc/rc.local')):
    print("File: rc.local saved")
else: print("Error: saving rc.local")

# Unmask hostapd
if(runShellCommand('sudo systemctl unmask hostapd')):
    print("Service: hostapd unmasked")
else: print("Error: unmasking hostapd service")

# Enable hostapd
if(runShellCommand('sudo systemctl enable hostapd')):
    print("Service: hostapd enabled")
else: print("Error: enabled hostapd service")

# Start hostapd
if(runShellCommand('sudo systemctl start hostapd')):
    print("Service: hostapd started")
else: print("Error: starting hostapd service")

# Restart dnsmasq
if(runShellCommand('sudo systemctl restart dnsmasq')):
    print("Service: dnsmasq started")
else: print("Error: starting dnsmasq service")
    
