// growerStepper.cpp
//
// Copyright (C) 2019 Grow

#include "growerStepper.h"

/***   growerStepper   ***/
// Statics variables definitions
//uint8_t growerStepper::__steppersRunning = 0;

growerStepper::growerStepper(
    uint8_t dirX1,
    uint8_t stepX1,
    uint8_t dirX2,
    uint8_t stepX2,
    uint8_t dirY,
    uint8_t stepY,
    uint8_t homeX1,
    uint8_t homeX2,
    uint8_t homeY,
    uint8_t en
  ){
    // Define all the pins
    __DirX1 = dirX1;
    __StepX1 = stepX1;
    __DirX2 = dirX2;
    __StepX2 = stepX2;
    __DirY = dirY;
    __StepY = stepY;
    __HomeX1 = homeX1;
    __HomeX2 = homeX2;
    __HomeY = homeY;
    __Enable = en;

    __MicroSteps = 0;
    __PulleyTeeth = 0;
    __Xmm = 0;
    __Ymm = 0;
    __StepPerRev = 0;

    // Define the motors
    stepperX1 = new AccelStepper(1, __StepX1, __DirX1);
    stepperX2 = new AccelStepper(1, __StepX2, __DirX2);
    stepperY = new AccelStepper(1, __StepY, __DirY);

    // Default paramaters
    __IsEnable = false; // Disable
    __Available = true; // Available
    __Home = false; // Not going at home
    __IsAtHome = false; // Not at home
    __Calibration = 0; // Not running

    __MaxX = DEFAULT_MAX_X_DISTANCE_MM;
    __MaxY = DEFAULT_MAX_Y_DISTANCE_MM;

    __OutHomeX1 = false;
    __OutHomeX2 = false;
    __OutHomeY = false;

    resetTime();
  }

void growerStepper::begin(
  uint8_t steps_per_rev = MOTOR_STEP_PER_REV,
  uint8_t microStep = DEFAULT_MICROSTEP,
  uint8_t pulleyTeeth = DEFAULT_PULLEY_TEETH,
  uint8_t Xmm = DEFAULT_X_MM_TOOTH,
  uint8_t Ymm = DEFAULT_Y_MM_TOOTH,
  bool goHome = HIGH
) {
     __StepPerRev = steps_per_rev;
     __MicroSteps = microStep;
     __PulleyTeeth = pulleyTeeth;
     __Xmm = Xmm;
     __Ymm = Ymm;

     // Initial Configuration
     stepperX1->setMaxSpeed(MOTOR_SPEED);
     stepperX1->setSpeed(0);
     stepperX1->setAcceleration(MOTOR_ACCEL);
     stepperX2->setMaxSpeed(MOTOR_SPEED);
     stepperX2->setSpeed(0);
     stepperX2->setAcceleration(MOTOR_ACCEL);
     stepperY->setMaxSpeed(MOTOR_SPEED);
     stepperY->setSpeed(0);
     stepperY->setAcceleration(MOTOR_ACCEL);
     // Check if we want this configuration of pinsInverted
     /* setPinsInverted parameters:
      1- bool directionInvert = false
      2- bool 	stepInvert = false
      2- bool 	enableInvert = false
     */
     /*
     stepperX1->setPinsInverted(true,false,false);
     stepperX2->setPinsInverted(true,false,false);
     */
     pinMode(__StepX1, OUTPUT);
     pinMode(__DirX1, OUTPUT);
     pinMode(__StepX2, OUTPUT);
     pinMode(__DirX2, OUTPUT);
     pinMode(__StepY, OUTPUT);
     pinMode(__DirY, OUTPUT);
     pinMode(__Enable, OUTPUT);
     pinMode(__HomeX1, INPUT_PULLUP);
     pinMode(__HomeX2, INPUT_PULLUP);
     pinMode(__HomeY, INPUT_PULLUP);

     pinMode(__Enable, HIGH); // Disable motors

     // Go home by default
     if(goHome){ home(); }
  }

long growerStepper::StepsToMM_X(long steps)
  { long resp = steps*(__PulleyTeeth*__Xmm)/(__StepPerRev*__MicroSteps);
    return resp;
  }

long growerStepper::MMToSteps_X(long dist_mm)
  { long resp = dist_mm*(__StepPerRev*__MicroSteps)/(__PulleyTeeth*__Xmm);
    return resp;
  }

long growerStepper::StepsToMM_Y(long steps)
  { long resp = steps*(__PulleyTeeth*__Ymm)/(__StepPerRev*__MicroSteps);
    return resp;
  }

long growerStepper::MMToSteps_Y(long dist_mm)
  { long resp = dist_mm*(__StepPerRev*__MicroSteps)/(__PulleyTeeth*__Ymm);
    return resp;
  }

void growerStepper::resetTime()
  { __ActualTime = millis(); }

bool growerStepper::isTimeToGoHome()
  { if(millis()-__ActualTime>WAIT_TIME_FOR_GO_HOME && !__IsAtHome){ return true;}
    else{return false;}
  }

long growerStepper::getXPosition()
  { long x1 = stepperX1->currentPosition()/2;
    long x2 = stepperX2->currentPosition()/2;
    long x = x1+x2;
    return StepsToMM_X(x);
  }

long growerStepper::getYPosition()
  { long y = stepperY->currentPosition();
    return StepsToMM_Y(y);
  }

void growerStepper::setMaxDistanceX(long maxDist)
  { __MaxX = maxDist; }

void growerStepper::setMaxDistanceY(long maxDist)
  { __MaxY = maxDist; }

long growerStepper::getMaxDistanceX()
  { return __MaxX; }

long growerStepper::getMaxDistanceY()
  { return __MaxY; }

bool growerStepper::moveX(long some_mm)
  { long actualPosition1 = StepsToMM_X(stepperX1->currentPosition());
    long actualPosition2 = StepsToMM_X(stepperX2->currentPosition());

    // If outHome Help is activated then check that the movement is allowed
    if(__OutHomeX1 || __OutHomeX2){
      if( (actualPosition1<__MaxX/2 && actualPosition2<__MaxX/2 && some_mm<0) ||
      (actualPosition1>__MaxX/2 && actualPosition2>__MaxX/2 && some_mm>0) )
      { return false; }
    }

    // Check that movement not exceed the container phisical dimensions or is unavailable
    if(actualPosition1+some_mm>__MaxX || actualPosition2+some_mm>__MaxX ||
       actualPosition1+some_mm<0  || actualPosition2+some_mm<0 || !__Available
     ){return false;}

    // If pass all the conditions and is disabled then enabled the stepper
    if(!__IsEnable){ enable(); }
    long some_steps = MMToSteps_X(some_mm);
    stepperX1->move(some_steps);
    stepperX2->move(some_steps);
    __IsAtHome = false;
    __Available = false;
    resetTime();
    return true;
  }

bool growerStepper::moveY(long some_mm)
  { long actualPosition = StepsToMM_Y(stepperY->currentPosition());

    // If outHome Help is activated then check that the movement is allowed
    if(__OutHomeY){
      if( (actualPosition<__MaxY/2 && some_mm<0) || (actualPosition>__MaxY/2 && some_mm>0) )
      { return false; }
    }

    // Check that movement not exceed the container phisical dimensions or is unavailable
    if(actualPosition+some_mm>__MaxY || actualPosition+some_mm<0 || !__Available){return false;}

    // If pass all the conditions and is disabled then enabled the stepper
    if(!__IsEnable){ enable(); }
    long some_steps = MMToSteps_Y(some_mm);
    stepperY->move(some_steps);
    __IsAtHome = false;
    __Available = false;
    resetTime();
    return true;
  }

bool growerStepper::moveXTo(long some_mm)
  { long actualPosition1 = StepsToMM_X(stepperX1->currentPosition());
    long actualPosition2 = StepsToMM_X(stepperX2->currentPosition());

    // If outHome Help is activated then check that the movement is allowed
    if(__OutHomeX1 || __OutHomeX2){
      if( (actualPosition1<__MaxX/2 && actualPosition2<__MaxX/2 && some_mm<0) ||
      (actualPosition1>__MaxX/2 && actualPosition2>__MaxX/2 && some_mm>0) )
      { return false; }
    }

    // Check that movement not exceed the container phisical dimensions or is unavailable
    if(some_mm<0 || some_mm>__MaxX || !__Available){return false;}

    // If pass all the conditions and is disabled then enabled the stepper
    if(!__IsEnable){ enable(); }
    long some_steps = MMToSteps_X(some_mm);
    stepperX1->moveTo(some_steps);
    stepperX2->moveTo(some_steps);
    __IsAtHome = false;
    __Available = false;
    resetTime();
    return true;
  }

bool growerStepper::moveYTo(long some_mm)
  { long actualPosition = StepsToMM_Y(stepperY->currentPosition());

    // If outHome Help is activated then check that the movement is allowed
    if(__OutHomeY){
      if( (actualPosition<__MaxY/2 && some_mm<0) || (actualPosition>__MaxY/2 && some_mm>0) )
      { return false; }
    }

    // Check that movement not exceed the container phisical dimensions or is unavailable
    if(some_mm<0 || some_mm>__MaxY || !__Available){return false;}

    // If pass all the conditions and is disabled then enabled the stepper
    if(!__IsEnable){ enable(); }
    long some_steps = MMToSteps_Y(some_mm);
    stepperY->moveTo(some_steps);
    __IsAtHome = false;
    __Available = false;
    resetTime();
    return true;
  }

void growerStepper::calibration()
  { __Calibration = 1; // First stage of the calibration is go home
    home();
  }

void growerStepper::enable()
  { digitalWrite(__Enable, LOW);
    __IsEnable = true;
  }

void growerStepper::disable()
  { digitalWrite(__Enable, HIGH);
    __IsEnable = false;
  }

bool growerStepper::isEnable()
  { return __IsEnable; }

bool growerStepper::isAvailable()
  { return __Available; }

bool growerStepper::home()
  { // Check if the steppers are available and there aren´t at home
    if(__Available && !__IsAtHome){
      // If pass all the conditions and is disabled then enabled the steppers
      if(!__IsEnable){ enable(); }
      // Move all the motors to home
      long moveX = MMToSteps_X(-15000); // Move -15m in X
      long moveY = MMToSteps_Y(-4000); // Move -4m in Y
      stepperX1->move(moveX);
      stepperX2->move(moveX);
      stepperY->move(moveY);
      __Available = false;
      __Home = true;
      resetTime();
      return true;
    }
    return false;
  }

void growerStepper::run()
  {
    if(digitalRead(__HomeX1) == HIGH || __OutHomeX1){
      stepperX1->run();
      if(digitalRead(__HomeX1) == HIGH && __OutHomeX1){
          __OutHomeX1 = false;
      }
    } else {
        stepperX1->moveTo(stepperX1->currentPosition());
        __OutHomeX1 = true;
    }

    if(digitalRead(__HomeX2) == HIGH || __OutHomeX2){
      stepperX2->run();
      if(digitalRead(__HomeX2) == HIGH && __OutHomeX2){
          __OutHomeX2 = false;
      }
    } else {
      stepperX2->moveTo(stepperX2->currentPosition());
      __OutHomeX2 = true;
    }

    if(digitalRead(__HomeY) == HIGH || __OutHomeY){
      stepperY->run();
      if(digitalRead(__HomeY) == HIGH && __OutHomeY){
          __OutHomeY = false;
      }
    } else {
      stepperY->moveTo(stepperY->currentPosition());
      __OutHomeY = true;
    }

    if(!stepperX1->isRunning() && !stepperX2->isRunning() && !stepperY->isRunning() ){
      if(!__Available){__Available = true;}

      if(__Home){
        __Home = false;
        __IsAtHome = true;
        stepperX1->setCurrentPosition(0);
        stepperX2->setCurrentPosition(0);
        stepperY->setCurrentPosition(0);
        disable(); // Disable the motors when home reached

        if(__Calibration == 1){
          long moveX = MMToSteps_X(15000); // Move 15m in X
          long moveY = MMToSteps_Y(4000); // Move 4m in Y
          stepperX1->move(moveX);
          stepperX2->move(moveX);
          stepperY->move(moveY);
          __Available = false;
          __IsAtHome = false;
          __Calibration = 2; // Second stage of the calibration is go to the opposite corner
        }

        else if(__Calibration == 3){
          long x1 = StepsToMM_X(stepperX1->currentPosition())/2;
          long x2 = StepsToMM_X(stepperX2->currentPosition())/2;
          long x = (x1 + x2)/2;
          long y = (StepsToMM_Y(stepperY->currentPosition()))/2;
          __MaxX = (__MaxX/2);
          __MaxY = (__MaxY/2) + y;
          __Calibration = 0; // There finished the calibration
        }
      }

      if(__Calibration == 2){
        long x1 = StepsToMM_X(stepperX1->currentPosition())/2;
        long x2 = StepsToMM_X(stepperX2->currentPosition())/2;
        long y = StepsToMM_Y(stepperY->currentPosition());
        __MaxX = x1 + x2;
        __MaxY = y;
        __Calibration = 3; // Second stage of the calibration is go home again
        home();
      }
    }

    if(isTimeToGoHome()){ home(); } // Go home if it´s been a while without using the motors

  }
