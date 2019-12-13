// growerStepper.cpp
//
// Copyright (C) 2019 Grow

#include "growerStepper.h"

/***   growerStepper   ***/
// Statics variables definitions
uint8_t growerStepper::__steppersRunning = 0;

growerStepper::growerStepper(
    uint8_t fl,
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

    // Sequence default parameters
    __Sequence = 0;
    __SequenceStop = false;
    __SequenceStageFinished = false;
    __SequenceMovements = 0;
    __SequenceXMoves = 0;
    __SequenceYMoves = 0;
    __SequenceDir = 1;
    __SequenceXmm = 0;
    __SequenceYmm = 0;

    __MaxX = 0; // Until read eeprom is zero
    __MaxY = 0; // Until read eeprom is zero

    __OutHomeX1 = false;
    __OutHomeX2 = false;
    __OutHomeY = false;
    __MoveX1 = false;
    __MoveX2 = false;
    __MoveY = false;

    __Floor = fl;
    __Stop = false;

    // Limit Switches
    __HX1 = false;
    __HX2 = false;
    __HY = false;
    __CheckX1 = false;
    __CheckX2 = false;
    __CheckY = false;
    __X1Time = millis();
    __X2Time = millis();
    __YTime = millis();
    
    resetTime();
  }

void growerStepper::begin(
  bool goHome = HIGH,
  uint8_t steps_per_rev = MOTOR_STEP_PER_REV,
  uint8_t microStep = DEFAULT_MICROSTEP,
  uint8_t pulleyTeeth = DEFAULT_PULLEY_TEETH,
  uint8_t Xmm = DEFAULT_X_MM_TOOTH,
  uint8_t Ymm = DEFAULT_Y_MM_TOOTH
) {
     __StepPerRev = steps_per_rev;
     __MicroSteps = microStep;
     __PulleyTeeth = pulleyTeeth;
     __Xmm = Xmm;
     __Ymm = Ymm;

     // Initial Configuration
     setNormalAccel();

     // Check if we want this configuration of pinsInverted
     /* setPinsInverted parameters:
      1- bool directionInvert = false
      2- bool 	stepInvert = false
      2- bool 	enableInvert = false
     */
     /*
     stepperX1->setPinsInverted(true,false,false);
     */
     /* Invert X2 */
     stepperX2->setPinsInverted(true,false,false);

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

     printAction(F("Started Correctly"), 0);

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
  { if(millis()-__ActualTime>WAIT_TIME_FOR_GO_HOME && !__IsAtHome){
      printAction(F("Time without move exceed. It is time to go home"), 2);
      return true;
    }
    else{return false;}
  }

void growerStepper::printAction(String act, uint8_t level=0)
  { if(level==0){ Serial.print(F("debug,")); } // Debug
    else if(level==1){ Serial.print(F("info,")); } // Info
    else if(level==2){ Serial.print(F("warning,")); } // Warning
    else if(level==3){ Serial.print(F("error,")); } // Error
    else if(level==4){ Serial.print(F("critical,")); } // Error
    Serial.print(F("Grower"));
    Serial.print(__Floor);
    Serial.print(F(": "));
    Serial.println(act);
  }

void growerStepper::printAction(String act1, String act2, String act3, uint8_t level)
  { if(level==0){ Serial.print(F("debug,")); } // Debug
    else if(level==1){ Serial.print(F("info,")); } // Info
    else if(level==2){ Serial.print(F("warning,")); } // Warning
    else if(level==3){ Serial.print(F("error,")); } // Error
    else if(level==4){ Serial.print(F("critical,")); } // Error
    Serial.print(F("Grower"));
    Serial.print(__Floor);
    Serial.print(F(": "));
    Serial.print(act1);
    Serial.print(act2);
    Serial.println(act3);
  }

void growerStepper::setMaxAccel(uint8_t stepper)
  {
    switch(stepper){
      case 0:
        stepperX1->setAcceleration(MOTOR_ACCEL*10);
        if(stepperX1->speed()<0){stepperX1->setSpeed(1);}
        else{stepperX1->setSpeed(-1);}
        break;
      case 1:
        stepperX2->setAcceleration(MOTOR_ACCEL*10);
        if(stepperX2->speed()<0){stepperX2->setSpeed(1);}
        else{stepperX2->setSpeed(-1);}
        break;
      case 2:
        stepperY->setAcceleration(MOTOR_ACCEL*10);
        if(stepperY->speed()<0){stepperY->setSpeed(1);}
        else{stepperY->setSpeed(-1);}
        break;
      default:
        break;
    }
  }

void growerStepper::setNormalAccel()
  { stepperX1->setMaxSpeed(MOTOR_SPEED);
    stepperX1->setSpeed(0);
    stepperX1->setAcceleration(MOTOR_ACCEL);
    stepperX2->setMaxSpeed(MOTOR_SPEED);
    stepperX2->setSpeed(0);
    stepperX2->setAcceleration(MOTOR_ACCEL);
    stepperY->setMaxSpeed(MOTOR_SPEED);
    stepperY->setSpeed(0);
    stepperY->setAcceleration(MOTOR_ACCEL);
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
  { __MaxX = DEFAULT_MAX_X_DISTANCE_MM-maxDist;
    printAction(F("MaxDistanceX "), String(getMaxDistanceX()), F(""), 0);
  }

void growerStepper::setMaxDistanceY(long maxDist)
  { __MaxY = DEFAULT_MAX_Y_DISTANCE_MM-maxDist;
    printAction(F("MaxDistanceY "), String(getMaxDistanceY()), F(""), 0);
  }

long growerStepper::getMaxDistanceX()
  { return DEFAULT_MAX_X_DISTANCE_MM-__MaxX; }

long growerStepper::getMaxDistanceY()
  { return DEFAULT_MAX_Y_DISTANCE_MM-__MaxY; }

bool growerStepper::moveX(long some_mm, bool seq = false)
  { long actualPosition1 = StepsToMM_X(stepperX1->currentPosition());
    long actualPosition2 = StepsToMM_X(stepperX2->currentPosition());
    int minDist = MIN_LIMIT_SECURITY_DISTANCE;

    // If outHome Help is activated then check that the movement is allowed
    if(__OutHomeX1 || __OutHomeX2){
      if( (actualPosition1<getMaxDistanceX()/2 && actualPosition2<getMaxDistanceX()/2 && some_mm<0) ||
      (actualPosition1>getMaxDistanceX()/2 && actualPosition2>getMaxDistanceX()/2 && some_mm>0) )
      { printAction(F("Direction of movement not allowed"), 3);
        return false;
      }
    }
    // Check that movement not exceed the container phisical dimensions
    if(actualPosition1+some_mm>getMaxDistanceX()-minDist ||
       actualPosition2+some_mm>getMaxDistanceX()-minDist ||
       actualPosition1+some_mm<minDist  ||
       actualPosition2+some_mm<minDist){
        printAction(F("Movement not allowed because will exceed physical dimensions"), 3);
        return false;
      }
    // Check available status
    if(!__Available){
      printAction(F("Unavailable"), 3);
      return false;
    }
    // Check that is not a sequence running
    if(__Sequence!=0 && !seq){
      printAction(F("Routine in progress"), 3);
      return false;
    }

    // If pass all the conditions and is disabled then enabled the stepper
    if(!__IsEnable){ enable(); }
    long some_steps = MMToSteps_X(some_mm);
    stepperX1->move(some_steps);
    stepperX2->move(some_steps);
    __IsAtHome = false;
    __Available = false;
    __MoveX1 = true;
    __MoveX2 = true;
    __steppersRunning += 2;
    resetTime();
    printAction(F("Moving X "), String(some_mm), F(" mm"), 0);
    return true;
  }

bool growerStepper::moveY(long some_mm, bool seq = false)
  { long actualPosition = StepsToMM_Y(stepperY->currentPosition());
    int minDist = MIN_LIMIT_SECURITY_DISTANCE;

    // If outHome Help is activated then check that the movement is allowed
    if(__OutHomeY){
      if( (actualPosition<getMaxDistanceY()/2 && some_mm<0) || (actualPosition>getMaxDistanceY()/2 && some_mm>0) )
      { printAction(F("Direction of movement not allowed"), 3);
        return false;
      }
    }
    // Check that movement not exceed the container phisical dimensions
    if(actualPosition+some_mm>getMaxDistanceY()-minDist || actualPosition+some_mm<minDist){
      printAction(F("Movement not allowed because will exceed physical dimensions"), 3);
      return false;
    }
    // Check available status
    if(!__Available){
      printAction(F("Unavailable"), 3);
      return false;
    }
    // Check that is not a sequence running
    if(__Sequence!=0 && !seq){
      printAction(F("Routine in progress"), 3);
      return false;
    }

    // If pass all the conditions and is disabled then enabled the stepper
    if(!__IsEnable){ enable(); }
    long some_steps = MMToSteps_Y(some_mm);
    stepperY->move(some_steps);
    __IsAtHome = false;
    __Available = false;
    __MoveY = true;
    __steppersRunning++;
    resetTime();
    printAction(F("Moving Y "), String(some_mm), F(" mm"), 0);
    return true;
  }

bool growerStepper::moveXTo(long some_mm, bool seq = false)
  { long actualPosition1 = StepsToMM_X(stepperX1->currentPosition());
    long actualPosition2 = StepsToMM_X(stepperX2->currentPosition());
    int minDist = MIN_LIMIT_SECURITY_DISTANCE;

    // If outHome Help is activated then check that the movement is allowed
    if(__OutHomeX1 || __OutHomeX2){
      if( (actualPosition1<getMaxDistanceX()/2 && actualPosition2<getMaxDistanceX()/2 && some_mm<0) ||
      (actualPosition1>getMaxDistanceX()/2 && actualPosition2>getMaxDistanceX()/2 && some_mm>0) )
      { printAction(F("Direction of movement not allowed"), 3);
        return false;
      }
    }
    // Check that movement not exceed the container phisical dimensions
    if(some_mm<minDist || some_mm>getMaxDistanceX()-minDist){
      printAction(F("Movement not allowed because will exceed physical dimensions"), 3);
      return false;
    }
    // Check available status
    if(!__Available){
      printAction(F("Unavailable"), 3);
      return false;
    }
    // Check that is not a sequence running
    if(__Sequence!=0 && !seq){
      printAction(F("Routine in progress"), 3);
      return false;
    }

    // If pass all the conditions and is disabled then enabled the stepper
    if(!__IsEnable){ enable(); }
    long some_steps = MMToSteps_X(some_mm);
    stepperX1->moveTo(some_steps);
    stepperX2->moveTo(some_steps);
    __IsAtHome = false;
    __Available = false;
    __MoveX1 = true;
    __MoveX2 = true;
    __steppersRunning += 2;
    resetTime();
    printAction(F("Moving X to "), String(some_mm), F(" mm"), 0);
    return true;
  }

bool growerStepper::moveYTo(long some_mm, bool seq = false)
  { long actualPosition = StepsToMM_Y(stepperY->currentPosition());
    int minDist = MIN_LIMIT_SECURITY_DISTANCE;

    // If outHome Help is activated then check that the movement is allowed
    if(__OutHomeY){
      if( (actualPosition<getMaxDistanceY()/2 && some_mm<0) || (actualPosition>getMaxDistanceY()/2 && some_mm>0) )
      { printAction(F("Direction of movement not allowed"), 3);
        return false;
      }
    }
    // Check that movement not exceed the container phisical dimensions
    if(some_mm<minDist || some_mm>getMaxDistanceY()-minDist){
      printAction(F("Movement not allowed because will exceed physical dimensions"), 3);
      return false;
    }
    // Check available status
    if(!__Available){
      printAction(F("Unavailable"), 3);
      return false;
    }
    // Check that is not a sequence running
    if(__Sequence!=0 && !seq){
      printAction(F("Routine in progress"), 3);
      return false;
    }

    // If pass all the conditions and is disabled then enabled the stepper
    if(!__IsEnable){ enable(); }
    long some_steps = MMToSteps_Y(some_mm);
    stepperY->moveTo(some_steps);
    __IsAtHome = false;
    __Available = false;
    __MoveY = true;
    __steppersRunning++;
    resetTime();
    printAction(F("Moving Y to "), String(some_mm), F(" mm"), 0);
    return true;
  }

bool growerStepper::calibration()
  { if(__Available){
      if(__Sequence==0){
        __Calibration = 1; // First stage of the calibration is go home
        printAction(F("Starting Calibration Sequence Stage 1"), 1);
        if(!home()){
          __Home = true;
        }
        return true;
      }
      printAction(F("Routine in progress"), 3);
      return false;
    }
    printAction(F("Unavailable"), 3);
    return false;
  }

void growerStepper::enable()
  { digitalWrite(__Enable, LOW);
    __IsEnable = true;
    printAction(F("Enable"), 0);
  }

void growerStepper::disable()
  { digitalWrite(__Enable, HIGH);
    __IsEnable = false;
    printAction(F("Disable"), 0);
  }

bool growerStepper::isEnable()
  { return __IsEnable; }

bool growerStepper::isAvailable()
  { if(__Available){printAction(F("Available"), 1);}
    else{printAction(F("Unavailable"), 2);}
    return __Available;
  }

bool growerStepper::isInCalibration()
  { if(__Calibration!=0){return true;}
    else{return false;}
  }

bool growerStepper::continueSequence()
  { if(__Available){
      if(__Sequence>0){
        __SequenceStop = false;
        printAction(F("Continue routine"), 0);
        return true;
      }
      printAction(F("Cannot continue routine because it has not started"), 3);
      return false;
    }
    printAction(F("Unavailable"), 3);
    return false;
  }

void growerStepper::stopSequence()
  { if(__Sequence>0){
      __Sequence = 0; // Stopping sequence
      printAction(F("Routine Stopped"), 2);
    }
    else{printAction(F("Cannot stop routine because it has not started"), 3);}
  }

bool growerStepper::sequence(long mm_X, long mm_Y)
  { if(__Available){ // If available
      if(__Sequence == 0){ // Ready to start a sequence
        long secureXDistance = getMaxDistanceX()-2*MIN_LIMIT_SECURITY_DISTANCE;
        long secureYDistance = getMaxDistanceY()-2*MIN_LIMIT_SECURITY_DISTANCE;

        if(mm_X>5 && mm_X<(secureXDistance*2/3) && mm_Y>5 && mm_Y<(secureYDistance*2/3) ){ // If parameters are valid
          __Sequence = 1; // First stage of the calibration is go home
          __SequenceDir = 1; // Direction is positive at least the first time
          __SequenceStop = false; // By default enable to continue
          __SequenceXmm = mm_X;
          __SequenceYmm = mm_Y;
          __SequenceXMoves = int(secureXDistance/__SequenceXmm);
          __SequenceYMoves = int(secureYDistance/__SequenceYmm);
          // Number of movements is all the points + home()
          __SequenceMovements = __SequenceXMoves*(__SequenceYMoves+1) + __SequenceYMoves + 1;
          printAction(F("Starting Routine Stage "), String(__Sequence), F(""), 1);
          if(!home()){
            __Home = true;
          }
          return true;
        }
        printAction(F("Imposible start that routine. Parameters are wrong"), 3);
        return false;
      }
      printAction(F("Imposible start that routine. Grower is not ready"), 3);
      return false;
    }
    printAction(F("Unavailable"), 3);
    return false;
  }

bool growerStepper::home()
  { // Check if the steppers are available
    if(__Available){
      if(__Sequence<=1){
        if(!__IsAtHome){
          // If pass all the conditions and is disabled then enabled the steppers
          if(!__IsEnable){ enable(); }
          // Move all the motors to home
          long moveX = MMToSteps_X(-X_HOME_DISTANCE_MM); // Move -12.5m in X
          long moveY = MMToSteps_Y(-Y_HOME_DISTANCE_MM); // Move -2.5m in Y
          if(!__OutHomeX1 || __Calibration!=0){stepperX1->move(moveX);}
          if(!__OutHomeX2 || __Calibration!=0){stepperX2->move(moveX);}
          if(!__OutHomeY || __Calibration!=0){stepperY->move(moveY);}

          __Available = false;
          __Home = true;
          __MoveX1 = true;
          __MoveX2 = true;
          __MoveY = true;
          __steppersRunning += 3;
          resetTime();
          printAction(F("Moving to Home"), 1);
          return true;
        }
        printAction(F("Already at Home"), 2);
        return false;
      }
      printAction(F("Routine in progress"), 3);
      return false;
    }
    printAction(F("Unavailable"), 3);
    return false;
  }

void growerStepper::stop()
  { if(__MoveX1){
      setMaxAccel(0);
      stepperX1->stop();
      __MoveX1 = false;
      __steppersRunning--;
      printAction(F("Stepper X1 Stopped"), 2);
      if(!__Stop){
        __Stop = true;
      }
    }

    if(__MoveX2){
      setMaxAccel(1);
      stepperX2->stop();
      __MoveX2 = false;
      __steppersRunning--;
      printAction(F("Stepper X2 Stopped"), 2);
      if(!__Stop){
        __Stop = true;
      }
    }

    if(__MoveY){
      setMaxAccel(2);
      stepperY->stop();
      __MoveY = false;
      __steppersRunning--;
      printAction(F("Stepper Y Stopped"), 2);
      if(!__Stop){
        __Stop = true;
      }
    }
  }

void growerStepper::stop(uint8_t st)
  { switch(st){
      case 0:
        setMaxAccel(0);
        stepperX1->stop();
        __MoveX1 = false;
        __steppersRunning--;
        printAction(F("Stepper X1 Stopped"), 2);
        break;
      case 1:
        setMaxAccel(1);
        stepperX2->stop();
        __MoveX2 = false;
        __steppersRunning--;
        printAction(F("Stepper X2 Stopped"), 2);
        break;
      case 2:
        setMaxAccel(2);
        stepperY->stop();
        __MoveY = false;
        __steppersRunning--;
        printAction(F("Stepper Y Stopped"), 2);
        break;
    }
  }

void growerStepper::run()
  { // Run all motors
    stepperX1->run();
    stepperX2->run();
    stepperY->run();
    
    // Maybe limit switch X1 was touched
    if(__HX1!=!digitalRead(__HomeX1) && !__CheckX1){
      __CheckX1 = true;
      __X1Time = millis();
    }
    else if(__CheckX1 && millis()-__X1Time>DEBOUNCE_TIME){
      bool limitX1 = !digitalRead(__HomeX1);
      if(__HX1!=limitX1){ __HX1=limitX1; }
      __CheckX1 = false;
    }
      
    // Stop X1 when limit switch touched
    if(__HX1 && !__OutHomeX1){
      __OutHomeX1 = true;
      stop(0);
    }
    else if(!__HX1 && __OutHomeX1){
      __OutHomeX1 = false;
    }
      
    // Maybe limit switch X2 was touched
    if(__HX2!=!digitalRead(__HomeX2) && !__CheckX2){
      __CheckX2 = true;
      __X2Time = millis();
    }
    else if(__CheckX2 && millis()-__X2Time>DEBOUNCE_TIME){
      bool limitX2 = !digitalRead(__HomeX2);
      if(__HX2!=limitX2){ __HX1=limitX2; }
      __CheckX2 = false;
    }
      
    // Stop X2 when limit switch touched
    if(__HX2 && !__OutHomeX2){
      __OutHomeX2 = true;
      stop(1);
    }
    else if(!__HX2 && __OutHomeX2){
      __OutHomeX2 = false;
    }
    
    // Maybe limit switch Y was touched
    if(__HY!=!digitalRead(__HomeY) && !__CheckY){
      __CheckY = true;
      __YTime = millis();
    }
    else if(__CheckY && millis()-__YTime>DEBOUNCE_TIME){
      bool limitY = !digitalRead(__HomeY);
      if(__HY!=limitY){ __HY=limitY; }
      __CheckY = false;
    }
      
    // Stop Y when limit switch touched
    if(__HY && !__OutHomeY){
      __OutHomeY = true;
      stop(2);
    }
    else if(!__HY && __OutHomeY){
      __OutHomeY = false;
    }
    
    /*
    // Stop X1 when limit switch touched
    if(!digitalRead(__HomeX1) && !__OutHomeX1){
      __OutHomeX1 = true;
      stop(0);
    }
    else if(digitalRead(__HomeX1) && __OutHomeX1){
      __OutHomeX1 = false;
    }

    // Stop X2 when limit switch touched
    if(!digitalRead(__HomeX2) && !__OutHomeX2){
      __OutHomeX2 = true;
      stop(1);
    }
    else if(digitalRead(__HomeX2) && __OutHomeX2){
      __OutHomeX2 = false;
    }

    // Stop Y when limit switch touched
    if(!digitalRead(__HomeY) && !__OutHomeY){
      __OutHomeY = true;
      stop(2);
    }
    else if(digitalRead(__HomeY) && __OutHomeY){
      __OutHomeY = false;
    }
    */
    
    // If X1 was running and stop it substract 1 to __steppersRunning
    if(__MoveX1 && !stepperX1->isRunning()){
      __MoveX1 = false;
      __steppersRunning--;
    }

    // If X2 was running and stop it substract 1 to __steppersRunning
    if(__MoveX2 && !stepperX2->isRunning()){
      __MoveX2 = false;
      __steppersRunning--;
    }

    // If Y was running and stop it substract 1 to __steppersRunning
    if(__MoveY && !stepperY->isRunning()){
      __MoveY = false;
      __steppersRunning--;
    }

    // If all the motors are stopped there are some actions that maybe need to be executed
    if(!stepperX1->isRunning() && !stepperX2->isRunning() && !stepperY->isRunning() ){
      bool decition = false;

      // If status was unavailable then change it and set normal accel and speeds again
      if(!__Available){
        __Available = true;
        setNormalAccel();
      }

      // If calibration is at stage 2 then starts stage 3
      if(__Calibration == 2 && !decition && !__Stop){
        long x1 = StepsToMM_X(stepperX1->currentPosition()/2);
        long x2 = StepsToMM_X(stepperX2->currentPosition()/2);
        long y = StepsToMM_Y(stepperY->currentPosition());
        setMaxDistanceX(x1 + x2);
        setMaxDistanceY(y);
        __Calibration = 3;
        decition = true;
        home();
        printAction(F("Starting Calibration Sequence Stage 3"), 1);
      }

      // If the grower is in routine (sequence value in range)
      if(__Sequence>1 && __Sequence<=__SequenceMovements){
        if(!__SequenceStageFinished){
          __SequenceStageFinished = true;
          printAction(F("In Position"), 0);
        }
        // If the central computer gives the permission to continue then
        if(!__SequenceStop){
          long x1 = StepsToMM_X(stepperX1->currentPosition()/2);
          long x2 = StepsToMM_X(stepperX2->currentPosition()/2);
          long x = x1 + x2 + 3; // +3 is to assure that long variable not cut decimals

          int whatNext = whatNext =  __Sequence-2-int(x/__SequenceXmm)*(__SequenceYMoves+1);
          __Sequence++;
          __SequenceStop = true;
          __SequenceStageFinished = false;
          printAction(F("Starting Routine Stage "), String(__Sequence), F(""), 0);
          if(whatNext<__SequenceYMoves){
            moveY(__SequenceDir*__SequenceYmm, true);
          }
          else{
            moveX(__SequenceXmm, true);
            __SequenceDir = -1*__SequenceDir;
          }
        }
      }

      // If the computer sends signal to continue and already finished
      else if(__Sequence>__SequenceMovements){
        if(!__SequenceStageFinished){
          __SequenceStageFinished = true;
          printAction(F("In Position"), 0);
        }
        if(!__SequenceStop){
          __Sequence = 0;
          printAction(F("Routine Finished"), 1);
        }
      }

      // If the grower was going home and not because it start stage 3 of calibration then
      if(__Home  && !decition){
        // Reset all the variables involves in home()
        __Home = false;

        if(!__Stop){ // If the motors were not stopped by force
          __IsAtHome = true;
          // Not changed if is in calibration sequence
          if(__Calibration!=3){
            stepperX1->setCurrentPosition(0);
            stepperX2->setCurrentPosition(0);
            stepperY->setCurrentPosition(0);
          }
          disable(); // Disable the motors when home reached
          printAction(F("Home Reached"), 1);

          // If calibration is at stage 1 then starts stage 2
          if(__Calibration == 1){
            long moveX = MMToSteps_X(X_HOME_DISTANCE_MM); // Move 12.5m in X
            long moveY = MMToSteps_Y(Y_HOME_DISTANCE_MM); // Move 2.5m in Y
            stepperX1->move(moveX);
            stepperX2->move(moveX);
            stepperY->move(moveY);
            __Available = false;
            __IsAtHome = false;
            __MoveX1 = true;
            __MoveX2 = true;
            __MoveY = true;
            __steppersRunning += 3;
            __Calibration = 2;
            decition = true;
            enable();
            printAction(F("Starting Calibration Sequence Stage 2"), 1);
          }

          // If calibration is at stage 3 then finish it and reset all the variables involves
          else if(__Calibration == 3){
            long x1 = (getMaxDistanceX()-StepsToMM_X(stepperX1->currentPosition()))/2;
            long x2 = (getMaxDistanceX()-StepsToMM_X(stepperX2->currentPosition()))/2;
            long x = x1 + x2;
            long y = getMaxDistanceY()-StepsToMM_Y(stepperY->currentPosition());
            setMaxDistanceX((getMaxDistanceX()+x)/2);
            setMaxDistanceY((getMaxDistanceY()+y)/2);
            __Calibration = 0; // There finished the calibration
            printAction(F("Calibration Finished"), 1);

            stepperX1->setCurrentPosition(0);
            stepperX2->setCurrentPosition(0);
            stepperY->setCurrentPosition(0);
          }

          // If the grower is starting routine then send it to the start position
          if(__Sequence==1){
            long moveX = MMToSteps_X(MIN_LIMIT_SECURITY_DISTANCE); // Move minDist in X
            long moveY = MMToSteps_Y(MIN_LIMIT_SECURITY_DISTANCE); // Move minDist in Y
            stepperX1->move(moveX);
            stepperX2->move(moveX);
            stepperY->move(moveY);
            __Available = false;
            __IsAtHome = false;
            __MoveX1 = true;
            __MoveX2 = true;
            __MoveY = true;
            __steppersRunning += 3;
            __Sequence++;
            __SequenceStop = true;
            enable();
            printAction(F("Starting Routine Stage "), String(__Sequence), F(""), 0);
          }
        }
      }
      if(__Stop){
        __Stop = false;
        __Calibration = 0;
        __Sequence = 0;
      }
    }

    // Go home if it´s been a while without using the motors
    if(isTimeToGoHome()){ home(); }

  }
