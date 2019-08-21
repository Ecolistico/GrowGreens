# Import modules
import os
import csv
import time
import serial
from datetime import datetime

# Init Arduino object, serial port and Baud Rate
#arduino = serial.Serial('COM9', 115200, timeout=1) # Windows
#arduino = serial.Serial('/dev/ttyMega1', 115200, timeout=1) # Linux
arduino = serial.Serial('/dev/MegaControl', 115200, timeout=1) # Linux
arduino.close()
tocani = serial.Serial('/dev/MegaTocani', 9600, timeout=1) # Linux
tocani.close()

day=0
data=[]
fuzzyData=[]
header=["Hora","Minuto","Segundo","T_GB","H_GB","T_1B","H_1B","T_2B","H_2B",
        "T_GC","H_GC","T_1C","H_1C","T_2C","H_2C","T_EXT","H_EXT","Presion","Volumen"]

pos_data=[]
header_tocani=["Arm","Forarm","Shoulder","X","Y","Z"]
filename_tocani = "Tocani/tocani_pos.csv"

action_state=-1 # Variable para ejecutar acciones de guardado, recepción o envío de información
count=0 # Contador
action_state_tocani=-1 # Variable para ejecutar acciones de guardado, recepción o envío de información
count_tocani=0 # Contador

print("Preparando dispositivos...")
arduino.open()
tocani.open()
print("Dispositivos preparados")
print("\n\n***** Bienvenido a Growngreens *****\n\nMediante esta ventana usted podrá monitorear y controlar su contenedor,"+
      " para ello debe haber acreditado satisfactoriamente su capacitación. En caso de no haber acreditado su capacitación"+
      " por favor comúniqueselo a su encargado directo, ya que puede causar daños irreversibles.\n")
start = input("¿Desea continuar? s/n \n")
if(start=="s" or start=="S" or start=="y" or start=="Y"):
    x=0
    print("\nA continuación se le darán las instrucciones precisas de como operar mediante este programa.\n\n"+
          "El programa tiene 2 modos de funcionamiento:\n\t 1) Tocani (Default Mode). Este modo le permite maniobrar al robot,"+
          " para acceder a el tiene que escribir el comando (T) y presionar enter. Si usted entra en este modo puede"+
          " utilizar los comandos vistos en su capacitación en la sección 'Tocani'.\n\t 2) Control. Este modo le "+
          " permite el control del aire acondicionado así como de los paramétros del control climático,"+
          " para acceder a el tiene que escribir el comando (C) y presionar enter. Si usted entra en este modo puede"+
          " utilizar los comandos vistos en su capacitación en la sección 'Control'.\n\nAdemás, en este ventana podrá observar"+
          " la información enviada por los diferentes dispositivos que controlan su contenedor.")
    time.sleep(5)
else:
    x=1
    
serial_device = 0 # Por default se inicia modo de operación Tocani 
  
#try:
while x==0:
    now = datetime.now()
    if(now.day<10):
        dia="0{0}".format(now.day)
    else:
        dia="{0}".format(now.day)

    if(now.month<10):
        mes="0{0}".format(now.month)
    else:
        mes="{0}".format(now.month)

    if day!=now.day:
        day=now.day
        if (now.day<10):
            dia="0{0}".format(now.day)
        else:
            dia="{0}".format(now.day)

        if (now.month<10):
            mes="0{0}".format(now.month)
        else:
            mes="{0}".format(now.month)

        filename="Data/{0}-{1}-{2}.csv".format(now.year,mes,dia)
        file=open(filename,'a')
        with file:
            writer = csv.writer(file,lineterminator='\r\n')
            writer.writerow(header)
        file.close()

    # Define Temp and Hum setpoints
    if(now.hour>=9 and now.hour<19):
        setpoint_temp=22
        setpoint_hum=65

    elif( (now.hour>=7 and now.hour<9) or (now.hour>=19 and now.hour<21) ):
        setpoint_temp=19
        setpoint_hum=70

    elif(now.hour>=21 or now.hour<7):
        setpoint_temp=15
        setpoint_hum=75

    key_line = GetLine()
    
    if(key_line=='T'):
        serial_device=0
        print("Modo de operación: Tocani")
    elif(key_line=='C'):
        serial_device=1
        print("Modo de operación: Control")  
    
    elif(key_line=="Cozir" and serial_device==0):
        tocani.write(bytes("C\n",'utf-8'))
    elif(key_line is not None and serial_device==0):
        tocani.write(bytes("{}\n".format(key_line),'utf-8'))
        
    elif(key_line=='c' and serial_device==1):
        ir_Code = 1
        arduino.write(bytes("c\r\n",'utf-8'))
        arduino.write(bytes(str(ir_Code)+"\r\n",'utf-8'))
    elif(key_line=='e' and serial_device==1):
        ir_Code = 2
        arduino.write(bytes("c\r\n",'utf-8'))
        arduino.write(bytes(str(ir_Code)+"\r\n",'utf-8'))
    elif(key_line=='v' and serial_device==1):
        ir_Code = 3
        arduino.write(bytes("c\r\n",'utf-8'))
        arduino.write(bytes(str(ir_Code)+"\r\n",'utf-8'))
    elif(key_line=='a' and serial_device==1):
        ir_Code = 4
        arduino.write(bytes("c\r\n",'utf-8'))
        arduino.write(bytes(str(ir_Code)+"\r\n",'utf-8'))

    while arduino.inWaiting()>0: # Analizar si hay datos entrantes en espera

        line=arduino.readline()  # Leer datos del serial en este caso linea compleata, con read() se puede asignar una cantidad de caracteres

        if str(line,'utf-8')=='Update\r\n':
            print("\nArduino is asking for time\n")
            print("Sending variables to Arduino:")
            arduino.write(bytes("a\r\n",'utf-8'))
            arduino.write(bytes(str(now.hour)+"\r\n",'utf-8'))
            arduino.write(bytes(str(now.minute)+"\r\n",'utf-8'))
            print("{0}:{1}\n".format(int(now.hour),int(now.minute)))
            continue


        if str(line,'utf-8')=='Fuzzy\r\n':
            print("\nNext variables to execute control algoritm are:")
            action_state=1
            count=0
            fuzzyData=[]
            continue

        if (str(line,'utf-8')=='Sensor\r\n'):
            print("\nGuardando variables de los sensores:\n")
            data=[]
            action_state=2
            count=0
            continue

        if(str(line,'utf-8')!='Update\r\n' and str(line,'utf-8')!='Fuzzy\r\n' and str(line,'utf-8')!='Sensor\r\n' and action_state==-1):
            print("\nArduino sends unknown command: {}".format(str(line,'utf-8')))


        if (action_state==1 and count<14):
            print(float(line.strip().decode('utf-8')))
            fuzzyData.append(float(line.strip().decode('utf-8')))
            count+=1

        if (action_state==1 and count==14 and len(fuzzyData)==14):
            print("Processing Fuzzy algorithm...")
            clim_result, disp_result, clim1_result = fuzzy_climateControl([fuzzyData[0],fuzzyData[2],fuzzyData[4],
            fuzzyData[6],fuzzyData[8],fuzzyData[10]], [fuzzyData[1],fuzzyData[3],fuzzyData[5],fuzzyData[7],fuzzyData[9],
            fuzzyData[11]], fuzzyData[12], fuzzyData[13], setpoint_temp, setpoint_hum)

            time_fan_on, time_fan_off, time_extractor_on, time_extractor_off, ir_Code = controlClimate_decision(clim_result, disp_result, clim1_result)
            print("Sending variables to Arduino:")
            arduino.write(bytes("b\r\n",'utf-8'))
            arduino.write(bytes(str(time_fan_on)+"\r\n",'utf-8'))
            arduino.write(bytes(str(time_fan_off)+"\r\n",'utf-8'))
            arduino.write(bytes(str(time_extractor_on)+"\r\n",'utf-8'))
            arduino.write(bytes(str(time_extractor_off)+"\r\n",'utf-8'))
            time.sleep(2)
            arduino.write(bytes("c\r\n",'utf-8'))
            arduino.write(bytes(str(ir_Code)+"\r\n",'utf-8'))
            count+=1
            action_state=-1
            print("Fuzzy variables sending are:\nFan On: {0}s\nFan Off: {1}s\nExtractor On: {2}s\nExtractor Off: {3}s\nAir Code: {4}\n".format(float(time_fan_on),float(time_fan_off),float(time_extractor_on),float(time_extractor_off),int(ir_Code)))

        if (action_state==2 and count<16):
            if count==0:
                data.append(str(now.hour))
                data.append(str(now.minute))
                data.append(str(now.second))
            count+=1
            data.append(float((str(line,'utf-8')).strip()))

        if (action_state==2 and count==16 and len(data)==19):
            print("{}-{}-{}       {}:{}:{}".format(now.day,now.month,now.year,data[0],data[1],data[2]))
            print("Zona GB:\n\tTemperatura= {0} °C\tHumedad= {1} %".format(data[3],data[4]))
            print("Zona 1B:\n\tTemperatura= {0} °C\tHumedad= {1} %".format(data[5],data[6]))
            print("Zona 2B:\n\tTemperatura= {0} °C\tHumedad= {1} %".format(data[7],data[8]))
            print("Zona GC:\n\tTemperatura= {0} °C\tHumedad= {1} %".format(data[9],data[10]))
            print("Zona 1C:\n\tTemperatura= {0} °C\tHumedad= {1} %".format(data[11],data[12]))
            print("Zona 2C:\n\tTemperatura= {0} °C\tHumedad= {1} %".format(data[13],data[14]))
            temp_mean = (data[3]+data[5]+data[7]+data[9]+data[11]+data[13])/6
            hum_mean = (data[4]+data[6]+data[8]+data[10]+data[12]+data[14])/6
            dvp_mean = DVP(temp_mean,hum_mean)
            print("Promedio:\n\tTemperatura= {0} °C\tHumedad= {1} %".format(round(temp_mean,2),round(hum_mean,2)))
            print("\nSet_DVP={0}\tActual_DVP={1} kPa\n".format(round(DVP(setpoint_temp,setpoint_hum),2),round(dvp_mean,2)))
            print("Exterior:\n\tTemperatura= {0} °C\tHumedad= {1} %".format(data[15],data[16]))
            print("La presión es {0} psi".format(data[17]))
            print("Volumen= total {0} lts".format(data[18]))
            print("\n\n")
            count+=1
            action_state=-1
            file=open(filename,'a')
            with file:
                writer = csv.writer(file,lineterminator='\n')
                writer.writerow(data)
            file.close()
            
    while tocani.inWaiting()>0: # Analizar si hay datos entrantes en espera

        line_tocani=tocani.readline()  # Leer datos del serial en este caso linea compleata, con read() se puede asignar una cantidad de caracteres

        if str(line_tocani,'utf-8')=='Position\r\n':
            print("\nTocani is giving its position:")
            action_state_tocani=1
            count_tocani=0
            pos_data=[]
            continue

        if(str(line_tocani,'utf-8')!='Position\r\n' and action_state_tocani!=1):
            print("Tocani sends: {}".format(str(line_tocani,'utf-8')))

        if (action_state_tocani==1 and count_tocani<6):
            print(float(line_tocani.strip().decode('utf-8')))
            pos_data.append(float(line_tocani.strip().decode('utf-8')))
            count_tocani+=1

        if (action_state_tocani==1 and count_tocani==6 and len(pos_data)==6):
            print("Saving Tocani Position")
            if(os.path.isfile(filename_tocani)==True):
                os.remove(filename_tocani)
            #pos = (pos_data[0], pos_data[1], pos_data[2], pos_data[3], pos_data[4], pos_data[5])
            file_tocani=open(filename_tocani,'a')
            with file_tocani:
                writer = csv.writer(file_tocani,lineterminator='\n')
                writer.writerow(header_tocani)
                writer.writerow(pos_data)
            file_tocani.close()
            action_state_tocani = -1

#except:
if(x==1):
    #print("Error raised")
    print("Cerrando dispósitivos")
    arduino.close() # Cerrar arduino
    tocani.close() # Cerrar tocani
    print("Programa terminado")