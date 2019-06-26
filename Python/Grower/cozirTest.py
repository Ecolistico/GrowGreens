import sys
sys.path.insert(0, '../')
import cozir

# Setting Up Cozir
co2Sensor = cozir.Cozir()

# For now just stable in polling mode
if(co2Sensor.opMode(co2Sensor.polling)):
    print("Cozir: Set Mode = K{0}".format(co2Sensor.act_OpMode))

# Uncomment one of the next lines to see the difference
# You can add any value that exist in file cozir.py under "Parameters request constants"
# Max 5 values can be set to obtaiend in one line
if(co2Sensor.setData_output(co2Sensor.Hum + co2Sensor.Temp + co2Sensor.CO2_filt)): # Get hum, temp and co2_filter
#if(co2Sensor.setData_output(co2Sensor.Hum + co2Sensor.Temp)):  # Get hum and temp
#if(co2Sensor.setData_output(co2Sensor.Hum + co2Sensor.CO2)): # Get hum and co2 witouth filter 
#if(co2Sensor.setData_output(co2Sensor.Zero_setPoint + co2Sensor.D_filt)): # Not supported yet # Get values related with LED strength signal
    print("Cozir: Data Mode = M{}".format(co2Sensor.act_OutMode))
    
print(co2Sensor.getData())