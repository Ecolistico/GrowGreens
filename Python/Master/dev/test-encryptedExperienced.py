'''
In general the structure of the expierence is the next one:
    HHMMCommandParameters

Where:
    HH - Hour
    MM - Minuto
    Command - Character that indicates some parameter change. See list 1 of reserved characters
    Parameters - Values codificated by this library

If more than two parameters change at the same time the structure is:
    HHMMCommand1Parameters1Command2Parameters2

And so on for 3 or more parameters changed.


List 1. Reserved characters for differents parameters or functions:
    * ',' - Separate one experience from the next one
    * '@' - Separate one day from the next one
    * '<' - Indicator that the lights were turned off
    * '>' - Indicator that the lights were turned on
    * '+' - Indicator that the next two values correspond to the temperature
    * '-' - Indicator that the next two values correspond to the humidity
    * '_' - Indicator that the next two values correspond to the CO2
    * '/' - Indicator that the next x values correspond to the nutrition
'''
import time

class bidict(dict):
    def __init__(self, *args, **kwargs):
        super(bidict, self).__init__(*args, **kwargs)
        self.inverse = {}
        for key, value in self.items():
            self.inverse.setdefault(value,[]).append(key)

    def __setitem__(self, key, value):
        if key in self:
            self.inverse[self[key]].remove(key)
        super(bidict, self).__setitem__(key, value)
        self.inverse.setdefault(value,[]).append(key)

    def __delitem__(self, key):
        self.inverse.setdefault(self[key],[]).remove(key)
        if self[key] in self.inverse and not self.inverse[self[key]]:
            del self.inverse[self[key]]
        super(bidict, self).__delitem__(key)

encryptationCode = bidict({ 'a':0, 'b':1, 'c':2, 'd':3, 'e':4, 'f':5, 'g':6, 'h':7, 'i':8, 'j':9, 'k':10, 'l':11, 'm':12, 'n':13, 'o':14,
                     'p':15, 'q':16, 'r':17, 's':18, 't':19, 'u':20, 'v':21,'w':22, 'x':23, 'y':24, 'z':25, 'A':26, 'B':27, 'C':28,
                     'D':29, 'E':30, 'F':31, 'G':32, 'H':33, 'I':34, 'J':35, 'K':36, 'L':37, 'M':38, 'N':39, 'O':40, 'P':41, 'Q':42,
                     'R':43, 'S':44, 'T':45, 'U':46, 'V':47,'W':48, 'X':49, 'Y':50, 'Z':51, '0':52, '1':53, '2':54, '3':55, '4':56,
                     '5':57, '6':58, '7':59, '8':60, '9':61, '#':62, '*':63
                    })


def encryptedTemp(temp, range):
    '''
    Encryption Rules:
        temp  [0-31.5] C
        range [0-31.5] C
        step = 0.5 C
    '''
    maxTemp = 31.5
    minTemp = 0

    encryptedValue = []
    if(temp>maxTemp):
        encryptedValue = encryptationCode.inverse[int(maxTemp*2)]

    elif(temp<minTemp):
        encryptedValue = encryptationCode.inverse[int(minTemp*2)]

    else:
        rdTemp = round(temp,1)
        intTemp = int(temp)
        decTemp = rdTemp - intTemp
        if(decTemp>=0.3 and decTemp<=0.7): decTemp = 0.5
        else: decTemp = 0

        encryptedValue = encryptationCode.inverse[int((intTemp+decTemp)*2)]

    rdRange = round(range,1)
    intRange = int(range)
    decRange = rdRange - intRange
    if(decRange>=0.3 and decRange<=0.7): decRange = 0.5
    else: decRange = 0

    encryptedValue += encryptationCode.inverse[int((intRange+decRange)*2)]
    print(len(encryptedValue))
    return ''.join(encryptedValue)

def encryptedHum(hum, range): # Encryption Rules are the same for range and value
    '''
    Encryption Rules:
        hum  [38-100] %
        range [0-63] %
        step = 1 %
    '''
    maxHum = 100
    minHum = 38

    encryptedValue = []
    if(hum>maxHum):
        encryptedValue = encryptationCode.inverse[int(maxHum)-38]

    elif(hum<minHum):
        encryptedValue = encryptationCode.inverse[int(minHum)-38]

    else:
        encryptedValue = encryptationCode.inverse[int(hum)-38]

    encryptedValue += encryptationCode.inverse[int(range)]
    print(len(encryptedValue))
    return "".join(encryptedValue)

def encryptedCO2(CO2, range):
    '''
    Encryption Rules:
        co2  [0-945] ppm
        step = 15 ppm
        range [0-315] ppm
        step = 5 ppm
    '''

    maxCO2 = 945
    minCO2 = 0

    encryptedValue = []
    if(CO2>maxCO2):
        encryptedValue = encryptationCode.inverse[int(maxCO2/15)]

    elif(CO2<minCO2):
        encryptedValue = encryptationCode.inverse[int(minCO2/15)]

    else:
        encryptedValue = encryptationCode.inverse[int(CO2/15)]

    encryptedValue += encryptationCode.inverse[int(range/5)]
    print(len(encryptedValue))
    return "".join(encryptedValue)

def decryptTemp(x):
    temp = float(encryptationCode[x[0]])/2
    range = float(encryptationCode[x[1]])/2
    return temp, range

def decryptHum(x):
    hum = encryptationCode[x[0]]+38
    range = encryptationCode[x[1]]
    return hum, range

def decryptCO2(x):
    CO2 = encryptationCode[x[0]]*15
    range = float(encryptationCode[x[1]])*5
    return CO2, range

# Problemas corriendo 2 funciones
# El resultado anterior a veces se concatene con el posterior dando lugar a malos
#a = decryptTemp(encryptedTemp(40,0))
#b = decryptHum(encryptedHum(101,5))
#c =  decryptCO2(encryptedCO2(-1.2,200))
c = encryptedCO2(-1.2,200)
a = encryptedTemp(40,0)
b = encryptedHum(101,5)



print(a)
print(b)
print(c)
