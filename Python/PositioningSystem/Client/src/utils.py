import os

def signed(n, bits=16):
    n &= (1 << bits) - 1
    if n >> (bits - 1):
        n -= 1 << bits
    return n

def runShellCommand(myCmD):
    try:
        os.system(myCmD)
        return 1
    except Exception as e:
        print(e)
        return 0