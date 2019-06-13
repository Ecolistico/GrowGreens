from sysRasp import runShellCommand, getOutput_ShellCommand

# Check Alternatives
alt = getOutput_ShellCommand('update-alternatives --list python')    
if(alt == ''):
    runShellCommand("sudo update-alternatives --install /usr/bin/python python /usr/bin/python2.7 1")
    runShellCommand("sudo update-alternatives --install /usr/bin/python python /usr/bin/python3.5 2")
# Set Python Version
runShellCommand("sudo update-alternatives --set python /usr/bin/python2.7")

