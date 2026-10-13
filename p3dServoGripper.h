#ifndef SERVOGRIPPER_H_
#define SERVOGRIPPER_H_

#include <Servo.h>

class ServoGripper{
public:
  ServoGripper(int pin, float grip_degree, float ungrip_degree);
  void on();
  void off();
private:
  Servo servo_motor;
  int servo_pin;
  float servo_grip_deg;
  float servo_ungrip_deg;
};

#endif
