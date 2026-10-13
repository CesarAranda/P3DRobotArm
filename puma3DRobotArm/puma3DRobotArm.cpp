// FIRMWARE de ROBOT 3DF
// por Cesar Aranda [Puma3D]
// Version 1.1 - Octubre 2026
// Otra información en:
// https://github.com/Puma3D/RobotArm
// https://www.thingiverse.com/thing:3674358

#include "p3dConfig.h"
#include "p3dPinout.h" // se adopta solo placa MEGA 2560  

#include <Arduino.h>
#include "p3dLogger.h"
#include "p3dRobotGeometry.h"
#include "p3dInterpolation.h"
#include "p3dFanControl.h"
#include "p3dRampsStepper.h"
#include "p3dQueue.h"
#include "p3dCommand.h"
#include "p3dEquipment.h"
#include "p3dEndStop.h"
#if GRIPPER==1
  #include "p3dByjGripper.h"     // gestiona el motor 24BYJ-18 (via ULN2003)
#elif GRIPPER==2
  #include "p3dServoGripper.h"   // gestiona el motor del gripper al usar 9G o MG996
#elif GRIPPER==3
  #include "p3dGripper.h"        // gestiona el motor 24BYJ-18 (via A4988)
#endif

//STEPPER OBJECTS
RampsStepper stepperHigher(HIG_STEP_PIN, HIG_DIR_PIN, HIG_ENABLE_PIN, HIG_INVERSE_STEPPER);
RampsStepper stepperLower(LOW_STEP_PIN, LOW_DIR_PIN, LOW_ENABLE_PIN, LOW_INVERSE_STEPPER);
RampsStepper stepperRotate(ROT_STEP_PIN, ROT_DIR_PIN, ROT_ENABLE_PIN, ROT_INVERSE_STEPPER);

//ENDSTOP OBJECTS
Endstop endStopHigher(HIG_MIN_PIN, HIG_DIR_PIN, HIG_STEP_PIN, HIG_ENABLE_PIN, HIG_MIN_INPUT, HIG_HOME_STEPS, HOME_DWELL, HIG_CHECK_DELAY);
Endstop endStopLower(LOW_MIN_PIN, LOW_DIR_PIN, LOW_STEP_PIN, LOW_ENABLE_PIN, LOW_MIN_INPUT, LOW_HOME_STEPS, HOME_DWELL, LOW_CHECK_DELAY);
Endstop endStopRotate(ROT_MIN_PIN, ROT_DIR_PIN, ROT_STEP_PIN, ROT_ENABLE_PIN, ROT_MIN_INPUT, ROT_HOME_STEPS, HOME_DWELL, ROT_CHECK_DELAY);

//RAIL OBJECTS
RampsStepper stepperRail(RAI_STEP_PIN, RAI_DIR_PIN, RAI_ENABLE_PIN, RAI_INVERSE_STEPPER);
Endstop endStopRail(RAI_MIN_PIN, RAI_DIR_PIN, RAI_STEP_PIN, RAI_ENABLE_PIN, RAI_MIN_INPUT, RAI_HOME_STEPS, HOME_DWELL, RAI_CHECK_DELAY);

//EQUIPMENT OBJECTS
#if GRIPPER==0
  // NO GRIPPER 
#elif GRIPPER==1
  // 28BYJ-48 MICRO STEPPER MOTOR UNIPOLAR WITH ULN2003
  BYJGripper gripper(BYJ_PIN_0, BYJ_PIN_1, BYJ_PIN_2, BYJ_PIN_3, BYJ_GRIP_STEPS);  
#elif GRIPPER==2
  // 9G SERVO OR MG996 SERVO EQUIVALENT
  ServoGripper gripper(SERVO_PIN, SERVO_GRIP_DEGREE, SERVO_UNGRIP_DEGREE);      
#else    
  // 28BYJ-48 STEPPER MOTOR BIPOLAR WITH A4988 
  // 2: FOR BASIC GRIPPER, 3: FOR JAW2 GRIPPER, 
  //    BOTH WITHOUT THE USE FOR ENDSTOP
  // 4: FOR PRESS GRIPPER ON NEXT VERSION
  Gripper gripper(GRI_STEP_PIN, GRI_DIR_PIN, GRI_ENABLE_PIN, GRI_INVERSE_STEPPER);
#endif

Equipment laser("LASER", LASER_PIN);
Equipment pump("PUMP", PUMP_PIN);
Equipment led("LED", LED_PIN);
FanControl fan(FAN_PIN, FAN_DELAY);

//EXECUTION & COMMAND OBJECTS
RobotGeometry geometry(END_EFFECTOR_OFFSET, LOW_SHANK_LENGTH, HIGH_SHANK_LENGTH);
Interpolation interpolator;
Queue<Cmd> queue(QUEUE_SIZE);
Command p3dCommand;
Cmd cmdCurrent;

void setStepperEnable(bool enable){
  String mMsg = enable?"MOTORS ENABLED":"MOTORS DISABLED";
  stepperRotate.enable(enable);
  stepperLower.enable(enable);
  stepperHigher.enable(enable);
  if(RAIL){
    stepperRail.enable(enable);
  }
  fan.enable(enable);
  
  Logger::logINFO(mMsg);
}

void homeSequence(){
  setStepperEnable(false);
  fan.enable(true);
  if (!LOW_HOME_STEPPER || !HIG_HOME_STEPPER){
    setStepperEnable(true);
    endStopLower.homeOffset(!LOW_INVERSE_STEPPER);
    endStopHigher.homeOffset(!HIG_INVERSE_STEPPER);
  } else {
    if (LOW_HOME_STEPPER){
      endStopLower.home(!LOW_INVERSE_STEPPER); //INDICATE STEPPER HOMING DIRECTION
    }
    if (HIG_HOME_STEPPER){
       endStopHigher.home(!HIG_INVERSE_STEPPER); //INDICATE STEPPER HOMING DIRECTION
    }
  }
  if (ROT_HOME_STEPPER){
    endStopRotate.home(ROT_INVERSE_STEPPER); //INDICATE STEPPER HOMING DIRECTION
  }

  if (RAIL){
    if (RAI_HOME_STEPPER){
      endStopRail.home(!RAI_INVERSE_STEPPER); //
    }
  }
  if((LOW_HOME_STEPPER and endStopLower.checkDelay())
    or (HIG_HOME_STEPPER and endStopHigher.checkDelay())
    or (ROT_HOME_STEPPER and endStopRotate.checkDelay())
    or (RAIL and RAI_HOME_STEPPER and endStopRail.checkDelay()) ){
    while(!queue.isEmpty()) {
      queue.pop();
    }
    Logger::logERROR("ROBOT FAILURE");
  }else{
    interpolator.setInterpolation(INITIAL_X, INITIAL_Y, INITIAL_Z, INITIAL_RAIL, INITIAL_X, INITIAL_Y, INITIAL_Z, INITIAL_RAIL);
    Logger::logINFO("HOMING COMPLETE");
  }
}

void executeCommand(Cmd cmd) {
  if (cmd.id == -1) {
    p3dCommand.printErr();
    return;
  }

  String endstopMsg;
  if (cmd.id == 'G') {
    switch (cmd.num) {
      // Effector movement
      case 0:
      case 1:
        fan.enable(true);
        Point posoffset;
        posoffset = interpolator.getPosOffset();      
        p3dCommand.move(cmd, interpolator.getPosmm(), posoffset, p3dCommand.isRelativeCoord);
        interpolator.setInterpolation(cmd.valueX, cmd.valueY, cmd.valueZ, cmd.valueR, cmd.valueF);
        Logger::logINFO("LINEAR MOVE: [X:" + String(cmd.valueX-posoffset.xmm) + " Y:" + String(cmd.valueY-posoffset.ymm) + " Z:" + String(cmd.valueZ-posoffset.zmm) + " R:" + String(cmd.valueR-posoffset.rmm) + "]");
        break;
      case 4: 
        p3dCommand.dWwell(cmd); 
        break;

      case 26:
      // Homing sequence for Lower Shank
        setStepperEnable(false);
        if (HIG_HOME_STEPPER){
          stepperHigher.enable(true);
          endStopHigher.home(!HIG_INVERSE_STEPPER); //INDICATE STEPPER HOMING DIRECTION
        }
        Logger::logINFO("HOMING LOWER SHANK COMPLETE");
        break;

      case 27:
        // Homing sequence for Upper Shank
        setStepperEnable(false);
        if (LOW_HOME_STEPPER){
          stepperLower.enable(true);
          endStopLower.home(!LOW_INVERSE_STEPPER); //INDICATE STEPPER HOMING DIRECTION
        }
        Logger::logINFO("HOMING UPPER SHANK COMPLETE");
        break;

      case 28:
        // Homing sequence for all Shanks
        homeSequence();
        Logger::logINFO("HOMING COMPLETE");
        break;
      case 90:
        // Absolute coordinate mode
        p3dCommand.coordsToAbsolute(); 
        break;
      case 91:
        // Relative coordinate mode
        p3dCommand.coordsToRelative(); 
        break; 
      case 92: 
        // set current position as new origin (0,0,0,0) and reset offset
        interpolator.resetPosOffset();
        p3dCommand.move(cmd, interpolator.getPosmm(), interpolator.getPosOffset(), false);
        interpolator.setPosOffset(cmd.valueX, cmd.valueY, cmd.valueZ, cmd.valueR);
        break;
      default: 
        p3dCommand.printErr();
    }
  }
  else
  if (cmd.id == 'M') {
    switch (cmd.num) {
      case 1:
        // Enable pump
        if (PUMP == 0){
          Logger::logINFO("PUMP NOT AVAILABLE");
        }else{
          if (pump.getState()) {
            Logger::logINFO("PUMP IS READY ENABLED");  
          } else {
            pump.on(); 
          }
        }   
        break;
      case 2:
        // Disable pump 
        if (PUMP == 0){
          Logger::logINFO("PUMP NOT AVAILABLE");
        }else{
          if (!pump.getState()) {
            Logger::logINFO("PUMP IS READY DISABLED");  
          } else {
            pump.off(); 
          }
        }   
        break;
      case 6:
        // Enable laser 
        if (LASER == 0){
          Logger::logINFO("LASER NOT AVAILABLE");
        }else{
          if (laser.getState()) {
            Logger::logINFO("LASER IS READY ENABLED");  
          } else {
            laser.on(); 
          }
        }   
        break;
      case 7:
        // Disable laser 
        if (LASER == 0){
          Logger::logINFO("LASER NOT AVAILABLE");
        }else{
          if (!laser.getState()) {
            Logger::logINFO("LASER IS READY DISABLED");  
          } else {
            laser.off(); 
          }
        }   
        break;
      case 8:
        // Enable LED   
        if (LED == 0){
          Logger::logINFO("LED NOT AVAILABLE");
        }else{
          if (led.getState()) {
            Logger::logINFO("LED IS READY ENABLED");  
          } else {
            led.on();
          }
        }   
        break;
      case 9:
        // Disable LED  
        if (LED == 0){
          Logger::logINFO("LED NOT AVAILABLE");
        }else{
          if (!led.getState()) {
            Logger::logINFO("LED IS READY DISABLED");  
          } else {
            led.off();
          }
        }   
        break;
      case 10:
        // Enable gripper 
        if (GRIPPER == 0){
          Logger::logINFO("GRIPPER NOT AVAILABLE");
        } else{
          if (gripper.getState()) {
            Logger::logINFO("GRIPPER IS READY ENABLED");  
          } else {
            gripper.on();
          }
        }
        break;
      case 11:
        // Disable gripper
        if (GRIPPER == 0){
          Logger::logINFO("GRIPPER NOT AVAILABLE");
        } else{
          if (!gripper.getState()) {
            Logger::logINFO("GRIPPER IS READY DISABLED");  
          } else {
            gripper.off();
          }
        }
        break;
      case 12: 
        // Select gripper type
        if (GRIPPER == 0){
          Logger::logINFO("GRIPPER NOT AVAILABLE");
        } else
          if (GRIPPER < 3){
            Logger::logINFO("GRIPPER TYPE COMMAND INVALID FOR THIS GRIPPER TYPE");
          } else{
              gripper.setType(cmd.valueS);
          }
        break;
      case 13:
          // Open gripper to absolute position
        if (GRIPPER == 0){
          Logger::logINFO("GRIPPER NOT AVAILABLE");
        } else
          if (GRIPPER < 3){
            Logger::logINFO("GRIPPER MOVE COMMAND INVALID FOR THIS GRIPPER TYPE");
          } else{
          if (!gripper.getState()) {
            Logger::logINFO("GRIPPER IS DISABLED");  
            } else{
              gripper.openAbsolute(cmd.valueS);
            }
          }
        break;
      case 14:
        // Open gripper to relative position 
        if (GRIPPER == 0){
          Logger::logINFO("GRIPPER NOT AVAILABLE");
        } else
          if (GRIPPER < 3){
            Logger::logINFO("GRIPPER MOVE COMMAND INVALID FOR THIS GRIPPER TYPE");
          } else{
          if (!gripper.getState()) {
            Logger::logINFO("GRIPPER IS DISABLED");  
            } else{
              gripper.openRelative(cmd.valueS);
            }
          }
        break;
      case 15:
        // Homing gripper manual = Closed [Opening = 0mm]   
        if (GRIPPER == 0){
          Logger::logINFO("GRIPPER NOT AVAILABLE");
        } else
          if (GRIPPER < 3){
            Logger::logINFO("GRIPPER MOVE COMMAND INVALID FOR THIS GRIPPER TYPE");
          } else{
          if (!gripper.getState()) {
            Logger::logINFO("GRIPPER IS DISABLED");  
            } else{
              gripper.homingManual();
            }
          }
        break;
      case 17:
        // Enable stepper motors  
        setStepperEnable(true);
        break;
      case 18: 
        // Disable stepper motors
        setStepperEnable(false);
        break;
      case 106:
        // Enable fan    
        if (FAN == 0){
          Logger::logINFO("FAN NOT AVAILABLE");
        } else{ 
          fan.enable(true);
        }
        break;
      case 107:
        // Disable fan
        if (FAN == 0){
          Logger::logINFO("FAN NOT AVAILABLE");
        } else{       
          fan.enable(false);
        }
        break;
      case 114:
        // Return the current positions of all axis and other info
        p3dCommand.cmdGetInfo(interpolator.getPosmm(), interpolator.getPosOffset(), 
            stepperHigher.getPosition(), stepperLower.getPosition(), stepperRotate.getPosition(), 
            fan.getState(), stepperRotate.getState(), gripper,
            pump.getState(), laser.getState(), led.getState()); 
        break;
      case 119:
        // Return the current state of all endstops
        endstopMsg = "ENDSTOP: [X:";
        endstopMsg += String(endStopHigher.state());
        endstopMsg += String(" Y:");
        endstopMsg += String(endStopLower.state());
        endstopMsg += String(" Z:");
        endstopMsg += String(endStopRotate.state());
        endstopMsg += String("]");
        //ORIGINAL LOG STRING UNDESIRABLE FOR UNO PROCESSING
        //Logger::logINFO("ENDSTOP STATE: [UPPER_SHANK(X):"+String(endStopHigher.state())+" LOWER_SHANK(Y):"+String(endStopLower.state())+" ROTATE_GEAR(Z):"+String(endStopRotate.state())+"]");
        Logger::logINFO(endstopMsg);
        break;
      default:
        p3dCommand.printErr();
    }
  }
  else {
    p3dCommand.printErr();
  }
}

void setup() {
  Serial.begin(BAUD);
  delay(100);
  Serial.println();
  Logger::logINFO("System boot ...");

  stepperHigher.setPositionRad(PI / 2.0); // 90°
  stepperLower.setPositionRad(0);         // 0°
  stepperRotate.setPositionRad(0);        // 0°
  stepperRail.setPosition(0);

  Logger::logINFO("ROBOT ONLINE");
  delay(500);
  if (HOME_ON_BOOT) { //HOME DURING SETUP() IF HOME_ON_BOOT ENABLED
    homeSequence(); 
  } else {
    setStepperEnable(false); //ROBOT ADJUSTABLE BY HAND AFTER TURNING ON

    if (HIG_HOME_STEPPER && LOW_HOME_STEPPER && ROT_HOME_STEPPER){
      Logger::logINFO("SEND G28 TO CALIBRATE ARM");
    }
    else if (HIG_HOME_STEPPER && LOW_HOME_STEPPER && !ROT_HOME_STEPPER){
      Logger::logINFO("ROTATE ROBOT TO FACE FRONT CENTRE & SEND G28 TO CALIBRATE ARM");
    }
    else if (!HIG_HOME_STEPPER || !LOW_HOME_STEPPER){
      Logger::logINFO("HOME ROBOT MANUALLY & SEND G28 TO CALIBRATE ARM");
    }
  }
  if(GRIPPER){  
    Logger::logINFO("ADJUST OPENING INITIAL GRIPPER WITH PROCEDURE MANUAL");
  }
  interpolator.setInterpolation(INITIAL_X, INITIAL_Y, INITIAL_Z, INITIAL_RAIL, INITIAL_X, INITIAL_Y, INITIAL_Z, INITIAL_RAIL);
}

void loop() {
  interpolator.updateActualPosition();

  geometry.set(interpolator.getXPosmm(), interpolator.getYPosmm(), interpolator.getZPosmm());
  stepperRotate.stepToPositionRad(geometry.getRotRad());
  stepperLower.stepToPositionRad(geometry.getLowRad());
  stepperHigher.stepToPositionRad(geometry.getHighRad());
  if (RAIL){
    stepperRail.stepToPositionMM(interpolator.getRPosmm(), STEPS_PER_MM_RAIL);
  }
  stepperRotate.update();
  stepperLower.update();
  stepperHigher.update();
  if (RAIL){
    stepperRail.update();
  }
  fan.update();

  if (!queue.isFull()) {
    if (p3dCommand.handleGcode()) {
      queue.push(p3dCommand.getCmd());
      //Logger::logDEBUG("Command added ...");
    }
  }
  if ((!queue.isEmpty()) && interpolator.isFinished()) {
    cmdCurrent = queue.pop();
    executeCommand(cmdCurrent);
    //Logger::logINFO("EXECUTED: command "+ String(cmdCurrent.id) + String(cmdCurrent.num) );
  }
  
  if(LED){
    led.on();
    if (millis() % 2000 < 1000) {
      led.on();
    }
    else {
      led.off();
    }
  }
}
