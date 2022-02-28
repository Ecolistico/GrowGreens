#!/usr/bin/env python3

# Import directories
import numpy as np
import skfuzzy as fuzz
import math

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
