#include "p3dConfig.h"
#include "p3dEquipment.h"
#include "p3dLogger.h"
#include <Arduino.h>

Equipment::Equipment(String equipment_name, int equipment_pin){
  name = equipment_name;
  pin = equipment_pin;
  installed = LASER*(name=="LASER") + PUMP*(name=="PUMP") + LED*(name=="LED");
  pinMode(pin, OUTPUT);
}

bool Equipment::getState() const {
  return state;
} 

void Equipment::on(){
  if(!installed){
    Logger::logERROR(String(name) + " NOT IMPLEMENTED");
    return;
  }

   state = true;
   digitalWrite(pin, HIGH);
   Logger::logINFO(String(name) + " ENABLED");
}

void Equipment::off(){
  if(!installed){
    Logger::logERROR(String(name) + " NOT IMPLEMENTED");  
    return;
  }

  state = false;
  digitalWrite(pin, LOW);
  Logger::logINFO(String(name) + " DISABLED");
}
