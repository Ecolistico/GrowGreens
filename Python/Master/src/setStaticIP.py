import sys

file = sys.argv[1]
deviceIP = sys.argv[2]
routerIP = sys.argv[3]

with open(file, "a+") as f:
    f.write("interface eth0\n")
    f.write('static ip_address={}/24\n'.format(deviceIP))
    f.write('static routers={}\n'.format(routerIP))
    f.write('static domain_name_servers={}\n'.format(routerIP))
    
    f.write("\ninterface wlan0\n")
    f.write('static ip_address={}/24\n'.format(deviceIP))
    f.write('static routers={}\n'.format(routerIP))
    f.write('static domain_name_servers={}\n'.format(routerIP))
    f.close
