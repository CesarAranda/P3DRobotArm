#include "p3dConfig.h"
#include "p3dFanControl.h"
#include "p3dLogger.h"
#include <Arduino.h>

FanControl::FanControl(int aPin, int aFanDelay) {
  fan_delay = aFanDelay * 1000;
  nextShutdown = 0;
  pin = aPin;
  installed=FAN;

  pinMode(pin , OUTPUT);
  digitalWrite(pin , LOW);

  state = false;
}

void FanControl::enable(bool value) {
  if(!installed){
    Logger::logERROR("FAN NOT IMPLEMENTED");  
    return;
  }

  if(value) {
    state = true;
    digitalWrite(pin, HIGH);
  } else {
    disable();
  }

  if(state){
    Logger::logINFO("FAN ENABLED");
  }else{
    Logger::logINFO("FAN DISABLED");
  }
}

void FanControl::disable() {
  state = false;
  nextShutdown = millis() + fan_delay;
  update();
}

void FanControl::update() {
  if (!state) {
     if (millis() >= nextShutdown) {
       digitalWrite(pin, LOW);
     }
  }
}

bool FanControl::getState(){
  return state;
}
