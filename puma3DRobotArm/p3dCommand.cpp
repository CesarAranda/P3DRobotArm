#include "p3dConfig.h"
#include "p3dGripper.h"
#include "p3dCommand.h"
#include "p3dLogger.h"
#include <Arduino.h>

Command::Command() {
  //initialize Command to a zero-move value;
  new_command.valueX = NAN; // coordenada X en mm 
  new_command.valueY = NAN; // coordenada Y en mm 
  new_command.valueZ = NAN; // coordenada Z en mm
  new_command.valueR = NAN; // coordenada en el Rail en mm
  new_command.valueF = 0;   // velocidad de avance
  new_command.valueS = 0;   // parametro complemento
                            // ej: M13 S15 -> fija apertura de 15mm en la pinza
                            // ej: M12 S2  -> fija un diente de tipo 2
  message = "";
  isRelativeCoord = false;
}

bool Command::handleGcode() {
  if (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') {
       return false; 
    }
    if (c == '\r') {
       bool b = processMessage(message);
       message = "";
       return b;
    } else {
       message += c; 
    }
  }
  return false;
}

bool Command::processMessage(String msg){

  new_command.valueX = NAN; 
  new_command.valueY = NAN;
  new_command.valueZ = NAN;
  new_command.valueR = NAN;
  new_command.valueF = 0;
  new_command.valueS = 0;  
  msg.toUpperCase();
  msg.replace(" ", "");
  unsigned active_index = 0;
  new_command.id = msg[active_index];
  if((new_command.id != 'G') && (new_command.id != 'M')){
    printErr();
    return false;
  }

  active_index++;
  unsigned temp_index = active_index;
  while (temp_index<msg.length() && !isAlpha(msg[temp_index])){
    temp_index++;
  }
  new_command.num = msg.substring(active_index, temp_index).toInt();
  active_index = temp_index;
  temp_index++;
  while (temp_index<msg.length()){
    while (!isAlpha(msg[temp_index]) || msg[temp_index]=='.'){
      temp_index++;
      if (temp_index == msg.length()){
        break;
      }
    }
    value_segment(msg.substring(active_index, temp_index));
    active_index = temp_index;
    temp_index++;
  }
  return true;
}

void Command::value_segment(String msg_segment){
  float msg_value = msg_segment.substring(1).toFloat();
  switch (msg_segment[0]){
    case 'X': new_command.valueX = msg_value; break;
    case 'Y': new_command.valueY = msg_value; break;
    case 'Z': new_command.valueZ = msg_value; break;
    case 'R': new_command.valueR = msg_value; break;
    case 'F': new_command.valueF = msg_value; break;
    case 'S': new_command.valueS = msg_value; break;
  }
}


Cmd Command::getCmd() const {
  return new_command; 
}

void Command::cmdGetInfo(Point pos, Point pos_offset, float highRad, float lowRad, float rotRad, 
      bool onFan, bool onMotors, Gripper aGripper, 
      bool onPump, bool onLaser, bool onLed) {
  if(isRelativeCoord) {
    Logger::logINFO("RELATIVE MODE");
  } else {
    Logger::logINFO("ABSOLUTE MODE");
  }
  Logger::logINFO("CURRENT POSITION: [X:"+String(pos.xmm - pos_offset.xmm)+" Y:"+String(pos.ymm - pos_offset.ymm)+" Z:"+String(pos.zmm - pos_offset.zmm)+" R:"+String(pos.rmm - pos_offset.rmm)+"]");
  //Logger::logINFO("RADIANS: [HIGH:"+String(highRad)+" LOW:"+String(lowRad)+" ROT:"+String(rotRad));
  
  if(onMotors) {
    Logger::logINFO("MOTORS ENABLED");  
  } else {
    Logger::logINFO("MOTORS DISABLED");  
  }
  if(GRIPPER){
    String logMsg = "GRIPPER ";
    if(aGripper.getState()) {
      logMsg += "ENABLED. TYPE: " + String(aGripper.getType());
      logMsg += " / OPEN: " + String(aGripper.getOpening()) + " mm";  
      if(aGripper.getType() == 3){
        logMsg += " / PRESS: " + String(aGripper.getPress()) + " kg";  
      }   
    } else {
      logMsg += "DISABLED. TYPE: " + String(aGripper.getType());
    }
    Logger::logINFO(logMsg);  
  }
  if(FAN){
    if(onFan) {
      Logger::logINFO("FAN ENABLED");  
    } else {
      Logger::logINFO("FAN DISABLED");  
    }
  }
  if(PUMP){
    if(onPump) {
      Logger::logINFO("PUMP ENABLED");  
    } else {
      Logger::logINFO("PUMP DISABLED");   
    }
  }
  if(LASER){
    if(onLaser) {
      Logger::logINFO("LASER ENABLED");  
    } else {
      Logger::logINFO("LASER DISABLED");  
    }
  }
  if(LED){
    if(onLed) {
      Logger::logINFO("LED ENABLED");  
    } else {
      Logger::logINFO("LED DISABLED");  
    }
  }
}

void Command::coordsToRelative(){
  isRelativeCoord = true;
  Logger::logINFO("RELATIVE MODE ON");
}

void Command::coordsToAbsolute(){
  isRelativeCoord = false;
  Logger::logINFO("ABSOLUTE MODE ON");
}

void Command::move(Cmd(&cmd), Point pos, Point pos_offset, bool isRelativeCoord){

  if(isRelativeCoord == true){
    cmd.valueX = isnan(cmd.valueX) ? pos.xmm : cmd.valueX + pos.xmm;
    cmd.valueY = isnan(cmd.valueY) ? pos.ymm : cmd.valueY + pos.ymm;
    cmd.valueZ = isnan(cmd.valueZ) ? pos.zmm : cmd.valueZ + pos.zmm;
    cmd.valueR = isnan(cmd.valueR) ? pos.rmm : cmd.valueR + pos.rmm; 
  } else {
    cmd.valueX = isnan(cmd.valueX) ? pos.xmm : cmd.valueX + pos_offset.xmm;
    cmd.valueY = isnan(cmd.valueY) ? pos.ymm : cmd.valueY + pos_offset.ymm;
    cmd.valueZ = isnan(cmd.valueZ) ? pos.zmm : cmd.valueZ + pos_offset.zmm;
    cmd.valueR = isnan(cmd.valueR) ? pos.rmm : cmd.valueR + pos_offset.rmm;
  }
}

void Command::dWwell(Cmd(&cmd)){
  delay(int(cmd.valueS * 1000));
  Logger::logERROR("SPEED DELAY NOT IMPLEMENTED");
//    unsigned long ini = millis();
//    while (millis() - ini < int(cmd.valueS * 1000));
}

void Command::printErr() {
  Logger::logERROR("COMMAND NOT RECOGNIZED");
}
