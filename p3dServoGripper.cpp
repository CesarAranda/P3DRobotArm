#include "p3dServoGripper.h"
#include "p3dLogger.h"
#include <Arduino.h>

ServoGripper::ServoGripper(int pin, float grip_degree, float ungrip_degree){
  servo_pin = pin;
  servo_grip_deg = grip_degree;
  servo_ungrip_deg = ungrip_degree;
  Servo servo_motor;
}

void ServoGripper::on(){
  servo_motor.attach(servo_pin);
  servo_motor.write(servo_grip_deg);
  
  delay(300);
//    unsigned long ini = millis();
//    while (millis() - ini < 300);

  servo_motor.detach();
}

void ServoGripper::off(){
  servo_motor.attach(servo_pin);
  servo_motor.write(servo_ungrip_deg);

  delay(300);
//    unsigned long ini = millis();
//    while (millis() - ini < 300);

  servo_motor.detach();
}
