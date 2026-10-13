#include "p3dConfig.h"
#include "p3dGripper.h"
#include "p3dLogger.h"
#include <Arduino.h>

Gripper::Gripper(int aStepPin, int aDirPin, int aEnablePin, bool aInverse) {
  stepPin = aStepPin;
  dirPin = aDirPin;
  enablePin = aEnablePin;
  inverse = aInverse;
  
  // El tipo de este objeto gripper depende del diente usado en la pinza
  // Puede cambiarse mediante GCodes (M12) desde el software de control
  // El tipo modifica básicamente la apertura máxima y la evaluación de presion
  // BASIC: type = 1; maxOpening = 22; pressDetect = false;
  // JAW2 : type = 2; maxOpening = 44; pressDetect = false;
  // PRESS: type = 3; maxOpening = 44; pressDetect = true;  (next version)
  type = 1;             // default
  maxOpening = 22;      // default
  pressDetect = false;  // default
  stepsPerMM = 44;      // default
  opening = 0;          // default
  
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(enablePin, OUTPUT);

  enable(false);
}

void Gripper::enable(bool value) {
  state = value;
  digitalWrite(enablePin, !value); // LOW=habilitado, HIGH=deshabilitado
}

int Gripper::getMaxOpening() const {
  return maxOpening;
}

int Gripper::getOpening() const {
  return opening;
}

int Gripper::getPress() const {
  return pressDetect ? 1 : 0;
}

bool Gripper::getState() const {
  return state;
}

int Gripper::getType() const {
  return type;
} 

void Gripper::homingManual() {
  stepsTargetOpening = 0;
  stepsCurrentOpening = 20 * stepsPerMM;

  digitalWrite(dirPin, inverse);
  while (stepsCurrentOpening > stepsTargetOpening) {    
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(P3D_GRIP_DELAY);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(P3D_GRIP_DELAY);

    stepsCurrentOpening--;
  }
  
  opening = stepsCurrentOpening/stepsPerMM;
  Logger::logINFO("GRIPPER OPEN " + String(opening) + " mm");
}

void Gripper::on() {
  enable(true);
  Logger::logINFO("GRIPPER ENABLED");
}

void Gripper::off() {
  enable(false);
  Logger::logINFO("GRIPPER DISABLED");
}

void Gripper::openAbsolute(int mmOpening) {
  if(mmOpening > maxOpening){
    Logger::logINFO("THE GRIPPER OPENING VALUE EXCEEDS THE MAXIMUM (" + String(maxOpening) + " mm)");
    return;
  }
  stepsTargetOpening = mmOpening * stepsPerMM;
  stepsCurrentOpening = opening * stepsPerMM;

  digitalWrite(dirPin, !inverse);
  while (stepsCurrentOpening < stepsTargetOpening) {  
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(P3D_GRIP_DELAY);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(P3D_GRIP_DELAY);

    stepsCurrentOpening++;
  }
  
  digitalWrite(dirPin, inverse);
  while (stepsCurrentOpening > stepsTargetOpening) {    
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(P3D_GRIP_DELAY);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(P3D_GRIP_DELAY);

    stepsCurrentOpening--;
  }
  
  opening = stepsCurrentOpening/stepsPerMM;
  Logger::logINFO("GRIPPER OPEN " + String(opening) + " mm");
}

void Gripper::openRelative(int mmOpening) {
  if(opening + mmOpening > maxOpening){
    Logger::logINFO("THE GRIPPER OPENING VALUE EXCEEDS THE MAXIMUM (" + String(maxOpening) + " mm)");
    return;
  }
  stepsTargetOpening = (opening + mmOpening) * stepsPerMM;
  stepsCurrentOpening = opening * stepsPerMM;

  digitalWrite(dirPin, !inverse);
  while (stepsCurrentOpening < stepsTargetOpening) {  
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(P3D_GRIP_DELAY);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(P3D_GRIP_DELAY);

    stepsCurrentOpening++;
  }
  
  digitalWrite(dirPin, inverse);
  while (stepsCurrentOpening > stepsTargetOpening) {    
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(P3D_GRIP_DELAY);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(P3D_GRIP_DELAY);

    stepsCurrentOpening--;
  }
  
  opening = stepsCurrentOpening/stepsPerMM;
  Logger::logINFO("GRIPPER OPEN " + String(opening) + " mm");
}

void Gripper::setType(int aType) {
  type = aType;
  if (type == 1) {
    maxOpening = 22;
    pressDetect = false;
  } else if (type == 2) {
    maxOpening = 44;
    pressDetect = false;
//  } else if (type == 3) {
//    maxOpening = 44;
//    pressDetect = true;
  } else {
    Logger::logERROR("INVALID GRIPPER TYPE");
  }
} 
