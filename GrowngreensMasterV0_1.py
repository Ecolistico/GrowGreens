# Import modules
import select
import time
import sys
import serial
from datetime import datetime
import csv
import numpy as np
import skfuzzy as fuzz
import math
import io
import os

# Get an Input Line
def GetLine(Block=False):
  if Block or select.select([sys.stdin], [], [], 0) == ([sys.stdin], [], []):
      return input()

# Calculation of Vapor Pressure Deficit (VPD) with the equation of Penman-Montheith
def DVP(temperature,HR):
    PVsat= 0.61078*np.exp((17.27*temperature)/(237.3+temperature))
    dvp = PVsat * (1-HR/100)
    return dvp

# Default settings Temp= 22°C, RH=70%, Standar Desviation on Temp = 0.33°C.
# Optional values External Temperature and Relative Humidity
def fuzzy_climateControl(temp_list, hum_list, temp_ext=-1, hum_ext=-1, set_temp=22, set_hum=70, set_temp_std=0.33):
    clim_result = -1
    disp_result = -1
    clim1_result = -1

    # Drop NaN of the temperature list
    index=[]
    for i in range(len(temp_list)):
        if(math.isnan(temp_list[i])):
            index.append(i)
            index.sort(reverse = True)
    for i in range(len(index)):
        temp_list.pop(index[i])

    # Drop NaN of the humidity list
    index=[]
    for i in range(len(hum_list)):
        if(math.isnan(hum_list[i])):
            index.append(i)
            index.sort(reverse = True)
    for i in range(len(index)):
        hum_list.pop(index[i])

    # If there are not elements to work return -1,-1,-1
    if(len(temp_list)<=0 or len(hum_list)<=0):
        return clim_result, disp_result, clim1_result

    # Debug data for the temperature, only if the list has more than 5 elements
    if(len(temp_list)>5):
        index=[]
        # Mean of the list without max and min values
        temp_mean=(sum(temp_list)-max(temp_list)-min(temp_list))/(len(temp_list)-2)
        for i in range(len(temp_list)):
            # Condition to drop data: Value >or< than mean+-6°C
            if(temp_list[i]>=temp_mean+6 or temp_list[i]<=temp_mean-6):
                index.append(i)
                index.sort(reverse = True)
        for i in range(len(index)):
            temp_list.pop(index[i])

    # Debug data for the humidity, only if the list has more than 5 elements
    if(len(hum_list)>5):
        index=[]
        # Mean of the list without max and min values
        hum_mean=(sum(hum_list)-max(hum_list)-min(hum_list))/(len(hum_list)-2)
        for i in range(len(hum_list)):
            # Condition to drop data: Value >or< than mean+-35%
            if(hum_list[i]>=hum_mean+35 or hum_list[i]<=hum_mean-35):
                index.append(i)
                index.sort(reverse = True)
        for i in range(len(index)):
            hum_list.pop(index[i])


    # Calculation of means
    temp_mean = sum(temp_list)/len(temp_list)
    hum_mean = sum(hum_list)/len(hum_list)

    # Calculation of VPD desired
    set_DVP = DVP(set_temp,set_hum)
    # Calculation of mean VPD
    dvp_mean = DVP(temp_mean,hum_mean)
    # Error in VPD
    er_DVP = (dvp_mean-set_DVP)/set_DVP*100

    # Calculation of Standar Desviations (std)
    temp_std = np.std(temp_list)
    #hum_std = npstd(hum_list)

    # Generate universe variables
    #   * Error in DVP meditions with respect to our setpoint. Range er_dvp [-100, 400]
    #   * Standar Desviation on Temperatura. Range temp_std [0,5]
    #   * Climate Response range of [-100, 100] witouth units
    #   * Dispersion Response range of [0, 100] witouth units

    er_dvp = np.arange(-100, 505, 5)
    fu_temp_std = np.arange(0, 5.1, 0.1)
    clim_resp  = np.arange(0, 205, 5)
    disp_resp = np.arange(0, 102, 2)

    # Generate fuzzy membership functions
    dvp_temp_ulo = fuzz.trapmf(er_dvp, [-100, -100, -67, -34])
    dvp_temp_lo = fuzz.trimf(er_dvp, [-67, -34, -0])
    dvp_temp_md = fuzz.trimf(er_dvp, [-34, 0, 34])
    dvp_temp_hi = fuzz.trimf(er_dvp, [0, 34, 67])
    dvp_temp_uhi = fuzz.trapmf(er_dvp, [34, 67, 500, 500])

    step_std = (10*set_temp_std)/4

    temp_std_lo = fuzz.trapmf(fu_temp_std, [0, 0 , set_temp_std, set_temp_std+step_std])
    temp_std_md = fuzz.trimf(fu_temp_std, [set_temp_std, set_temp_std+step_std, set_temp_std+2*step_std])
    temp_std_hi = fuzz.trapmf(fu_temp_std, [set_temp_std+step_std, set_temp_std+2*step_std, 5 , 5])

    clim_resp_ulo = fuzz.trapmf(clim_resp, [0, 0, 33, 66])
    clim_resp_lo =  fuzz.trimf(clim_resp, [33, 66, 100])
    clim_resp_md =  fuzz.trimf(clim_resp, [66, 100, 133])
    clim_resp_hi =  fuzz.trimf(clim_resp, [100, 133, 166])
    clim_resp_uhi = fuzz.trapmf(clim_resp, [133, 166, 200, 200])

    disp_resp_lo = fuzz.trapmf(disp_resp, [0, 0 , 25, 50])
    disp_resp_md = fuzz.trimf(disp_resp, [25, 50, 75])
    disp_resp_hi = fuzz.trapmf(disp_resp, [50, 75, 100 , 100])

    # We need the activation of our fuzzy membership functions at the read values.
    # This is what fuzz.interp_membership exists for!

    dvp_level_ulo = fuzz.interp_membership(er_dvp, dvp_temp_ulo, er_DVP)
    dvp_level_lo = fuzz.interp_membership(er_dvp, dvp_temp_lo, er_DVP)
    dvp_level_md = fuzz.interp_membership(er_dvp, dvp_temp_md, er_DVP)
    dvp_level_hi = fuzz.interp_membership(er_dvp, dvp_temp_hi, er_DVP)
    dvp_level_uhi = fuzz.interp_membership(er_dvp, dvp_temp_uhi, er_DVP)

    temp_std_level_lo = fuzz.interp_membership(fu_temp_std, temp_std_lo, temp_std)
    temp_std_level_md = fuzz.interp_membership(fu_temp_std, temp_std_md, temp_std)
    temp_std_level_hi = fuzz.interp_membership(fu_temp_std, temp_std_hi, temp_std)

    # Climate Control
    # Rules 1 #
    climate_response_ulo = np.fmin(dvp_level_ulo, clim_resp_ulo)
    # Rule 2 #
    climate_response_lo = np.fmin(dvp_level_lo, clim_resp_lo)
    # Rule 3 #
    climate_response_md = np.fmin(dvp_level_md, clim_resp_md)
    # Rule 4 #
    climate_response_hi = np.fmin(dvp_level_hi, clim_resp_hi)
    # Rule 5 #
    climate_response_uhi = np.fmin(dvp_level_uhi, clim_resp_uhi)

    # Distribution Temperature
    # Rules 1 #
    dispersion_response_lo = np.fmin(temp_std_level_lo, disp_resp_lo)
    # Rule 2 #
    dispersion_response_md = np.fmin(temp_std_level_md, disp_resp_md)
    # Rule 3 #
    dispersion_response_hi = np.fmin(temp_std_level_hi, disp_resp_hi)

    # Aggregate all output membership functions together
    aggregated1 = np.fmax(climate_response_ulo,np.fmax(climate_response_lo,np.fmax(climate_response_md, np.fmax(climate_response_hi, climate_response_uhi))))
    aggregated2 = np.fmax(dispersion_response_lo, np.fmax(dispersion_response_md, dispersion_response_hi))


    # Calculate defuzzified result depending on previous results

    # If Climate levels Low, Medium, High and Ultra-High are close to zero use "Min of Maximum"
    if(dvp_level_lo<=0.05 and dvp_level_md<=0.05 and dvp_level_hi<=0.05 and dvp_level_uhi<=0.05):
        clim_result = fuzz.defuzz(clim_resp, aggregated1, 'som')
    # If Climate levels Ultra-Low, Low, Medium and High are close to zero use "Max of Maximum"
    elif(dvp_level_ulo<=0.05 and dvp_level_lo<=0.05 and dvp_level_md<=0.05 and dvp_level_hi<=0.05):
        clim_result = fuzz.defuzz(clim_resp, aggregated1, 'lom')
    # In any other case, use "Bisector"
    else:
        clim_result = fuzz.defuzz(clim_resp, aggregated1, 'bisector')

    clim_result-=100

    # If Standar Desviation levels Medium and High are close to zero use "Min of Maximum"
    if(temp_std_level_md<=0.05 and temp_std_level_hi<=0.05):
        disp_result = fuzz.defuzz(disp_resp, aggregated2, 'som')
    # If Standar Desviation levels Low and Medium are close to zero use "Max of Maximum"
    elif(temp_std_level_lo<=0.05 and temp_std_level_md<=0.05):
        disp_result = fuzz.defuzz(disp_resp, aggregated2, 'lom')
    # In any other case, use "Bisector"
    else:
        disp_result = fuzz.defuzz(disp_resp, aggregated2, 'bisector')

    # If user gave info about external climate adjust the results
    if( temp_ext!=-1 and hum_ext!=-1 and not(np.isnan(temp_ext)) and not(np.isnan(hum_ext))):
        ext_dvp =  DVP(temp_ext,hum_ext)
        ext_er_DVP = (ext_dvp-set_DVP)/set_DVP*100
        # We need the activation of our fuzzy membership functions at the read values.
        # This is what fuzz.interp_membership exists for!

        ext_dvp_level_ulo = fuzz.interp_membership(er_dvp, dvp_temp_ulo, ext_er_DVP)
        ext_dvp_level_lo = fuzz.interp_membership(er_dvp, dvp_temp_lo, ext_er_DVP)
        ext_dvp_level_md = fuzz.interp_membership(er_dvp, dvp_temp_md, ext_er_DVP)
        ext_dvp_level_hi = fuzz.interp_membership(er_dvp, dvp_temp_hi, ext_er_DVP)
        ext_dvp_level_uhi = fuzz.interp_membership(er_dvp, dvp_temp_uhi, ext_er_DVP)

        # Climate Control
        # Rules 1 #
        climate1_response_ulo = np.fmin(ext_dvp_level_ulo, clim_resp_ulo)
        # Rule 2 #
        climate1_response_lo = np.fmin(ext_dvp_level_lo, clim_resp_lo)
        # Rule 3 #
        climate1_response_md = np.fmin(ext_dvp_level_md, clim_resp_md)
        # Rule 4 #
        climate1_response_hi = np.fmin(ext_dvp_level_hi, clim_resp_hi)
        # Rule 5 #
        climate1_response_uhi = np.fmin(ext_dvp_level_uhi, clim_resp_uhi)
        # Aggregate all output membership functions together
        aggregated3 = np.fmax(climate1_response_ulo,np.fmax(climate1_response_lo,np.fmax(climate1_response_md, np.fmax(climate1_response_hi, climate1_response_uhi))))

        # Calculate defuzzified result depending on previous results

        # If Climate levels Low, Medium, High and Ultra-High are close to zero use "Min of Maximum"
        if(ext_dvp_level_lo<=0.05 and ext_dvp_level_md<=0.05 and ext_dvp_level_hi<=0.05 and ext_dvp_level_uhi<=0.05):
            clim1_result = fuzz.defuzz(clim_resp, aggregated3, 'som')
        # If Climate levels Ultra-Low, Low, Medium and High are close to zero use "Max of Maximum"
        elif(ext_dvp_level_ulo<=0.05 and ext_dvp_level_lo<=0.05 and ext_dvp_level_md<=0.05 and ext_dvp_level_hi<=0.05):
            clim1_result = fuzz.defuzz(clim_resp, aggregated3, 'lom')
        # In any other case, use "Bisector"
        else:
            clim1_result = fuzz.defuzz(clim_resp, aggregated3, 'bisector')

        clim1_result-=100


    return clim_result, disp_result, clim1_result

# Take a decision based on the results of the function fuzzy_climateControl()
# To determinate the action for the fans, extractor and Air Conditioner
def controlClimate_decision(clim, disp, clim1=-1):
    # Calculate times for fan. It only depends on temperature dispersion
    if(disp<=18):
        time_fan_on = 0
    else:
        time_fan_on = 1.7073*disp - 20.732
    time_fan_off = 180-time_fan_on

    # Calculate times for extractor- It only depends on the VPD.
    if(clim<=-5):
        time_extractor_on = 0
    else:
        time_extractor_on = 1.3333*clim + 16.667
    time_extractor_off = 180-time_extractor_on

    time_fan_on = float(round(time_fan_on,3))
    time_fan_off = float(round(time_fan_off,3))
    time_extractor_on = float(round(time_extractor_on,3))
    time_extractor_off = float(round(time_extractor_off,3))

    # Decide function of the A/C. It depends on hour of the day, VPD and external climate

    # If we don´t have info about external climate
    if(clim1==-1):
        # if -100<clim<-10 --> Turn on A/C mode:heat
        if(clim<=-10): ir_Code = 1
        # if -10<clim<5 --> Turn off A/C
        elif(clim>-10 and clim<=5): ir_Code = 4
        # if 5<clim<15 --> Turn on A/C mode:FAN
        elif(clim>5 and clim<=15): ir_Code = 3
        # if 15<clim<100 --> Turn on A/C mode:cool
        elif(clim>15): ir_Code = 2

    # If we have info about external climate
    if(clim1!=-1 and not(np.isnan(clim1))):
        # If the signs of clim and clim1 are differents and -10<clim1<10 --> Turn on A/C mode:FAN
        if( np.sign(clim)!=np.sign(clim1) and clim1>-10 and clim1<10 ): ir_Code = 3

        else:
            # if -100<clim<-10 --> Turn on A/C mode:heat
            if(clim<=-10): ir_Code = 1
            # if -10<clim<5 --> Turn off A/C
            elif(clim>-10 and clim<=5): ir_Code = 4
            # if 5<clim<15 --> Turn on A/C mode:FAN
            elif(clim>5 and clim<=15): ir_Code = 3
            # if 15<clim<100 --> Turn on A/C mode:cool
            elif(clim>15): ir_Code = 2

    return time_fan_on, time_fan_off, time_extractor_on, time_extractor_off, ir_Code

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