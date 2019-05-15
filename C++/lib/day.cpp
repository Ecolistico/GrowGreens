// day.cpp
//
// Copyright (C) 2019 GrowGreens

#include "day.h"

/***   Day   ***/

Day::Day( ) // Default Constructor
    {__isItDay = LOW ;
     __dayBeginHour = 6 ;
     __nightBeginHour = 20 ;
    }

Day::Day( float dayTime_Hour, float nightTime_Hour ) // Constructor
    { __isItDay = LOW ;
      if(dayTime_Hour>=0 && dayTime_Hour<24 && nightTime_Hour>=0 && nightTime_Hour<24 && dayTime_Hour!=nightTime_Hour){
        __dayBeginHour = dayTime_Hour ;
        __nightBeginHour = nightTime_Hour ;
      }
      else{
        __dayBeginHour = 6 ;
        __nightBeginHour = 20 ;
      }
    }

Day::Day( byte dayTime_Hour, byte nightTime_Hour, byte dayTime_Minute, byte nightTime_Minute ) // Constructor
    { float dayHOUR = dayTime_Hour + (dayTime_Minute/60) ;
      float nightHOUR = nightTime_Hour + (nightTime_Minute/60) ;
      if(dayHOUR>=0 && dayHOUR<24 && nightHOUR>=0 && nightHOUR<24 && dayHOUR!=nightHOUR){
        __isItDay = LOW ;
        __dayBeginHour = dayHOUR ;
        __nightBeginHour = nightHOUR ;
      }
      else{
        __dayBeginHour = 6 ;
        __nightBeginHour = 20 ;
      }
    }

bool Day::changeDayHour(float dayTime_Hour)
  { if(dayTime_Hour>=0 && dayTime_Hour<24 && dayTime_Hour!=__nightBeginHour){
      __dayBeginHour = dayTime_Hour ;
      return true;
    }
    else{ return false; }
  }

bool Day::changeDayHour(byte dayTime_Hour, byte dayTime_Minute )
  { float HOUR = dayTime_Hour+(dayTime_Minute/60);
    if(HOUR>=0 && HOUR<24 && HOUR!=__nightBeginHour){
      __dayBeginHour = HOUR ;
      return true;
    }
    else{ return false; }
  }

bool Day::changeNightHour(float nightTime_Hour)
  { if(nightTime_Hour>=0 && nightTime_Hour<24  && nightTime_Hour!=__dayBeginHour){
      __nightBeginHour = nightTime_Hour ;
      return true;
    }
    else{ return false; }
  }

bool Day::changeNightHour(byte nightTime_Hour, byte nightTime_Minute )
  { float HOUR = nightTime_Hour+(nightTime_Minute/60);
    if(HOUR>=0 && HOUR<24 && HOUR!=__dayBeginHour){
    __nightBeginHour = HOUR ;
    return true;
    }
    else{ return false; }
  }

bool Day::changeHours(float dayTime_Hour, float nightTime_Hour )
  { if(dayTime_Hour>=0 && dayTime_Hour<24 && nightTime_Hour>=0 && nightTime_Hour<24 && dayTime_Hour!=nightTime_Hour){
      __dayBeginHour = dayTime_Hour ;
      __nightBeginHour = nightTime_Hour ;
      return true;
    }
    else{ return false; }
  }

bool Day::changeHours(byte dayTime_Hour, byte nightTime_Hour, byte dayTime_Minute, byte nightTime_Minute )
  { float dayHOUR = dayTime_Hour + (dayTime_Minute/60) ;
    float nightHOUR = nightTime_Hour + (nightTime_Minute/60) ;
    if(dayHOUR>=0 && dayHOUR<24 && nightHOUR>=0 && nightHOUR<24 && dayHOUR!=nightHOUR){
      __dayBeginHour = dayHOUR ;
      __nightBeginHour = nightHOUR ;
      return true;
    }
    else{ return false; }
  }

bool Day::run( byte HOUR, byte MINUTE )
   { __isItDay = LOW; // By default is night
     float actualHour = HOUR + float(MINUTE)/60;

    if(__dayBeginHour<__nightBeginHour){
      if(actualHour >= __dayBeginHour && actualHour < __nightBeginHour){
        __isItDay = HIGH;
      }
      else if (actualHour >= __nightBeginHour || actualHour < __dayBeginHour){
        __isItDay = LOW;
      }
    }
    else{
      if(actualHour >= __dayBeginHour || actualHour < __nightBeginHour){
        __isItDay = HIGH;
      }
      else if (actualHour >= __nightBeginHour && actualHour < __dayBeginHour){
        __isItDay = LOW;
      }
    }
      return (__isItDay) ;
   }

/***   multiDay   ***/
MultiDay::MultiDay( ) // Constructor
    {__isItDay = LOW ;
     __daysPerDay = 1 ;
     __hoursDay = 16;
     __hoursNight = 8;
     __beginHour[0] = 6 ;
     __initProblem = 0 ;
     }

MultiDay::MultiDay(byte cyclesPerDay, float percentageLight, float initialHour ) // Constructor
   {__isItDay = LOW ;

    if(24%cyclesPerDay == 0 && percentageLight>=0 && percentageLight<=100 && initialHour<24 && initialHour>=0 ){ // If paremeters are wrong then...
      int hoursPerDay = 24/cyclesPerDay ;
      __daysPerDay = cyclesPerDay;
      __hoursDay = (hoursPerDay*percentageLight)/100;
      __hoursNight = hoursPerDay - __hoursDay;

      __beginHour[0] = initialHour-hoursPerDay*int(initialHour/hoursPerDay) ;

      if(__daysPerDay>1){
        for(int i=1; i<__daysPerDay; i++){
          __beginHour[i] = __beginHour[i-1] + ((24/__daysPerDay)) ;
        }
      }
      __initProblem = 0 ;
    }

    else{ // If parameters are wrong set default cycle.
       __daysPerDay = 1 ;
       __hoursDay = 16;
       __hoursNight = 8;
       __beginHour[0] = 6 ;
       if(24%cyclesPerDay != 0){
          __initProblem = 1 ; // Cycles are not a divisor of 24 hrs
       }
       else if(percentageLight<0 || percentageLight>100){
         __initProblem = 2 ; // percentageLight out of range [0-100]
       }
       else if(initialHour>=24 || initialHour<0){
         __initProblem = 3 ; // initialHourout of Range [0-24) hrs
       }
       else{ __initProblem = 4; } // Unknown reason of failure
    }
  }

bool MultiDay::getState()
  { return __isItDay; }

bool MultiDay::redefine( byte cyclesPerDay, float percentageLight, float initialHour )
  {
  if(24%cyclesPerDay == 0 && percentageLight>=0 && percentageLight<=100 && initialHour<24 && initialHour>=0 ){ // If paremeters are wrong then...
     int hoursPerDay = 24/cyclesPerDay ;
     __daysPerDay = cyclesPerDay;
     __hoursDay = (hoursPerDay*percentageLight)/100;
     __hoursNight = hoursPerDay - __hoursDay;

     __beginHour[0] = initialHour-hoursPerDay*int(initialHour/hoursPerDay) ;

     if(__daysPerDay>1){
       for(int i=1; i<__daysPerDay; i++){
         __beginHour[i] = __beginHour[i-1] + ((24/__daysPerDay)) ;
       }
     }
     __initProblem = 0 ;
     return true;
   }

   else{ // If parameters are wrong set default cycle.
      if(24%cyclesPerDay != 0){
         __initProblem = 1 ; // Cycles are not a divisor of 24 hrs
      }
      else if(percentageLight<0 || percentageLight>100){
        __initProblem = 2 ; // percentageLight out of range [0-100]
      }
      else if(initialHour>=24 || initialHour<0){
        __initProblem = 3 ; // initialHourout of Range [0-24) hrs
      }
      else{__initProblem = 4 ; } // Unknown reason of failure
      return false;
   }
  }

bool MultiDay::run( byte HOUR, byte MINUTE )
  { float actualHour = HOUR + MINUTE/60;
    byte control_Stage = 200 ;

    for(int i=__daysPerDay-1 ; i>=0; i--){
      if(__beginHour[i]<=actualHour){
        control_Stage = i;
        break;
      }
    }

    if(control_Stage == 200){
      control_Stage = __daysPerDay-1 ;
    }

    if(__daysPerDay-control_Stage==1 && actualHour-__beginHour[control_Stage]<0){
      if(actualHour >= __beginHour[control_Stage]-24 && actualHour < __beginHour[control_Stage]+__hoursDay-24){
        __isItDay = HIGH;
      }
      else if(actualHour >= __beginHour[control_Stage]+__hoursDay-24 && actualHour < __beginHour[control_Stage]+__hoursDay+__hoursNight-24){
        __isItDay = LOW;
      }
    }
    else{
      if(actualHour >= __beginHour[control_Stage] && actualHour < __beginHour[control_Stage]+__hoursDay){
        __isItDay = HIGH;
      }
      else if(actualHour >= __beginHour[control_Stage]+__hoursDay && actualHour < __beginHour[control_Stage]+__hoursDay+__hoursNight){
        __isItDay = LOW;
      }
    }

     return (__isItDay) ;
  }

/***   LED   ***/
LED::LED( String name ) // Constructor
    { __State = LOW ;
      __Enable = HIGH ;
      __Name = name;
    }

LED::LED( bool state, String name ) // Constructor
    { __State = state ;
      __Enable = HIGH ;
      __Name = name;
    }

void LED::printState()
  { Serial.print(F("LED Section ")); Serial.print(__Name);
    if(__State){Serial.println(F(": Turn On")); }
    else{Serial.println(F(": Turn Off")); }
  }

void LED::changeState()
    {  __State = !__State; printState(); }

void LED::setState(bool state)
    { if(__State!=state){
        __State = state;  printState();
      }
    }

bool LED::getState()
   { if(__Enable == HIGH){return __State ;}
     else{return LOW ;}
   }

void LED::enable(bool en)
    { __Enable = en;  }

bool LED::isEnable()
    { return __Enable;  }

void LED::turnOn()
    {  setState(HIGH);    }

void LED::turnOff()
    {  setState(LOW);    }
