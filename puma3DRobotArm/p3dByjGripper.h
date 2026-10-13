#ifndef BYJ_GRIPPER_H_
#define BYJ_GRIPPER_H_

class BYJGripper {
public:
  BYJGripper(int pin0, int pin1, int pin2, int pin3, int steps);
  void on();
  void off();
private:
  bool direction;
  void moveSteps();
  void setDirection();
  int byj_pin_0;
  int byj_pin_1;
  int byj_pin_2;
  int byj_pin_3;
  int grip_steps;
  int step_cycle;
};
#endif
