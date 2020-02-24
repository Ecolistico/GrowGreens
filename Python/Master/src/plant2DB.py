#!/usr/bin/env python

# Import directories
import os
import re
import sys
from datetime import datetime
from credentials import parse

IpRegex = "^(([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])$";
DigitRegex="[0-9]"

try:
    ip = sys.argv[1]
    update = sys.argv[2]
    val = re.match(IpRegex, ip)
    if val: pass
    else:
        ip = None
        print("\033[1;31;40mError: Provide a valid IP.\nExample: '192.168.1.150'")
    if len(update)==8:
        for digit in update:
            if re.match(DigitRegex, digit) == None:
                update = '0'
                print("\033[1;31;40mError: Provide a valid number to update database.\nExample: '01201231'")
                break
        if update == '00000000':
            update = '0'
            print("\033[1;31;40mError: There is not changes in DB.")
    else:
        print("\033[1;31;40mError: Provide a valid number to update database.\nExample: '01201231'")
        
except Exception as e:
    ip = None
    print("\033[1;31;40mError: Provide a valid IP.\nExample: '192.168.1.150'\n{}".format(e))
    
if ip != None and len(update)==8:
    uri = 'http://{}:1337/parse'.format(ip)
    
    # Define path to parse app
    os.environ["PARSE_API_ROOT"] = uri

    from parse_rest.datatypes import Function, Object, GeoPoint
    from parse_rest.connection import register
    from parse_rest.query import QueryResourceDoesNotExist
    from parse_rest.connection import ParseBatcher
    from parse_rest.core import ResourceRequestBadRequest, ParseError

    # App Variables
    register(parse['APPLICATION_ID'], parse['REST_API_KEY'], master_key=parse['MASTER_KEY'])
            
    # Database Structures
    plantKeys = ['piso', 'lado', 'linea', 'contador']

    # Create Planta Class to Query
    class Plant(Object):
        piso = 1
        lado = 'A'
        linea = 1
        contador = 0
        pass

    class Config(Object):
        pass

    def isDBAvailable():
        # Query Config to look make sure DB is available
        try:
            conf = Config.Query.all()
            if len(conf)>=1: return True
            else:
                print("Database unavailable")
                return False
        except:
            print("Database unavailable")
            return False

    def getPlantCounter(floor, side, line):
        # Query the last planta saved in the Planta class
        pl = Plant.Query.filter(piso=floor).filter(lado=side).filter(linea=line).order_by('createdAt', descending=True).limit(1)
        try:
            pl = pl[0]
            return pl
        except:
            print('Data required not found')
            return None

    # Function to save new object
    def newEntry(object, properties):
        newObject = Plant()
        for property in properties:
            setattr(newObject, property, getattr(object, property))
        newObject.save()
        
    def addNewPlant(floor, side):
        for i in range(1,13):
            lastPlant = getPlantCounter(floor, side, i)
            if lastPlant != None:
                lastPlant.contador = lastPlant.contador+1
            else:
                lastPlant = Plant()
                lastPlant.piso = floor
                lastPlant.lado = side
                lastPlant.linea = i
                lastPlant.contador = 1
            newEntry(lastPlant, plantKeys)

    if(isDBAvailable()):
        for i in range(int(update[0])): addNewPlant(1, 'A')
        for i in range(int(update[1])): addNewPlant(1, 'B')

        for i in range(int(update[2])): addNewPlant(2, 'A')
        for i in range(int(update[3])): addNewPlant(2, 'B')

        for i in range(int(update[4])): addNewPlant(3, 'A')
        for i in range(int(update[5])): addNewPlant(3, 'B')

        for i in range(int(update[6])): addNewPlant(4, 'A')
        for i in range(int(update[7])): addNewPlant(4, 'B')
