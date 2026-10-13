#include "p3dByjGripper.h"
#include "p3dLogger.h"
#include <Arduino.h>

BYJGripper::BYJGripper(int pin0, int pin1, int pin2, int pin3, int steps){
  grip_steps = steps;
  byj_pin_0 = pin0;
  byj_pin_1 = pin1;
  byj_pin_2 = pin2;
  byj_pin_3 = pin3;
  step_cycle = 0;
  pinMode(byj_pin_0, OUTPUT);
  pinMode(byj_pin_1, OUTPUT);
  pinMode(byj_pin_2, OUTPUT);
  pinMode(byj_pin_3, OUTPUT);
}

void BYJGripper::on() {
  direction = true;
  for (int i = 1; i <= grip_steps; i++) {
    moveSteps();
    delay(1);
    //unsigned long ini = millis();
    //while (millis() - ini < 1);
  }
    Logger::logINFO("GRIPPER ENABLED");
}

void BYJGripper::off() {
  direction = false;
  for (int i = 1; i <= grip_steps; i++) {
    moveSteps();
    delay(1);
    //unsigned long ini = millis();
    //while (millis() - ini < 1);
  }
    Logger::logINFO("GRIPPER DISABLED");
}

void BYJGripper::setDirection(){
  if (direction == true) {
    step_cycle++;
  }
  if (direction == false) {
    step_cycle--;
  }
  if (step_cycle > 7) {
    step_cycle = 0;
  }
  if (step_cycle < 0) {
    step_cycle = 7;
  }
}

void BYJGripper::moveSteps() {
  switch (step_cycle) {
  case 0:
    digitalWrite(byj_pin_0, LOW);
    digitalWrite(byj_pin_1, LOW);
    digitalWrite(byj_pin_2, LOW);
    digitalWrite(byj_pin_3, HIGH);
    break;
  case 1:
    digitalWrite(byj_pin_0, LOW);
    digitalWrite(byj_pin_1, LOW);
    digitalWrite(byj_pin_2, HIGH);
    digitalWrite(byj_pin_3, HIGH);
    break;
  case 2:
    digitalWrite(byj_pin_0, LOW);
    digitalWrite(byj_pin_1, LOW);
    digitalWrite(byj_pin_2, HIGH);
    digitalWrite(byj_pin_3, LOW);
    break;
  case 3:
    digitalWrite(byj_pin_0, LOW);
    digitalWrite(byj_pin_1, HIGH);
    digitalWrite(byj_pin_2, HIGH);
    digitalWrite(byj_pin_3, LOW);
    break;
  case 4:
    digitalWrite(byj_pin_0, LOW);
    digitalWrite(byj_pin_1, HIGH);
    digitalWrite(byj_pin_2, LOW);
    digitalWrite(byj_pin_3, LOW);
    break;
  case 5:
    digitalWrite(byj_pin_0, HIGH);
    digitalWrite(byj_pin_1, HIGH);
    digitalWrite(byj_pin_2, LOW);
    digitalWrite(byj_pin_3, LOW);
    break;
  case 6:
    digitalWrite(byj_pin_0, HIGH);
    digitalWrite(byj_pin_1, LOW);
    digitalWrite(byj_pin_2, LOW);
    digitalWrite(byj_pin_3, LOW);
    break;
  case 7:
    digitalWrite(byj_pin_0, HIGH);
    digitalWrite(byj_pin_1, LOW);
    digitalWrite(byj_pin_2, LOW);
    digitalWrite(byj_pin_3, HIGH);
    break;
  default:
    digitalWrite(byj_pin_0, LOW);
    digitalWrite(byj_pin_1, LOW);
    digitalWrite(byj_pin_2, LOW);
    digitalWrite(byj_pin_3, LOW);
    break;
  }
  
  setDirection();
}
