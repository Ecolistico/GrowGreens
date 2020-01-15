import sys
import json
from collections import OrderedDict

# Functions that generates de unique ID for each container
def IDgenerator(file, state, city, num):
    genID= ""
    stateExist = False
    cityExist = False
    
    with open(file, 'r') as f:
        data = json.load(f, object_pairs_hook=OrderedDict)

    countState = 1
    countCity = 1

    for st in data:
        if(st==state):
            stateExist = True
            for ct in data[state]:
                if(ct==city):
                    cityExist = True
                    break
                else:
                    countCity += 1
            break
        else:
            countState += 1
    
    if(stateExist and cityExist):
        if(countState<10):
            genID += "0{}-".format(countState)
        else:
            genID += "{}-".format(countState)
        if(countCity<10):
            genID += "00{}-".format(countCity)
        elif(countCity<100):
            genID += "0{}-".format(countCity)
        else:
            genID += "{}-".format(countCity)
        if(int(num)<10):
            genID += "00{}".format(num)
        elif(int(num)<100):
            genID += "0{}".format(num)
        else:
            genID += "{}".format(num)
        return genID
    
    else: return None

file = sys.argv[1] # Where ID will save
jsonFile = sys.argv[2] # Info of states and cities in JSON
staticIP = sys.argv[3]

# Generate unique ID
with open(file, 'r') as f:
    data = json.load(f)
    state = data['state']
    city = data['city']
    num = data['num']
    genID = IDgenerator(jsonFile, state, city, num)                

with open(file, 'w') as f:
    if(genID!=None):
        if(len(staticIP)>3):
            data['staticIP'] = staticIP
        data['ID'] = genID
        print("Se generó el ID del sistema={}".format(genID))
        json.dump(data, f)
    else:
        print("\033[1;31;40mError: El ID no sé pudo generar, por favor contacte a su supervisor e informe el error")
        input("Presione enter para finalizar")
