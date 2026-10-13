#ifndef COMMAND_H_
#define COMMAND_H_

#include <Arduino.h>
#include "p3dInterpolation.h"
#include "p3dGripper.h"

struct Cmd {
  char id;
  int num;
  float valueX;
  float valueY;
  float valueZ;
  float valueR;
  float valueF;
  float valueS; 
};

class Command {
  public:
    Command();
    bool handleGcode();
    bool processMessage(String msg);
    void value_segment(String msg_segment);
    Cmd getCmd() const;
    void cmdGetInfo(Point pos, Point pos_offset, 
      float highRad, float lowRad, float rotRad, 
      bool onFan, bool onMotors, Gripper aGripper, 
      bool onPump, bool onLaser, bool onLed);
    void coordsToRelative();
    void coordsToAbsolute();
    bool isRelativeCoord;
    Cmd new_command;
    void move(Cmd(&cmd), Point pos, Point pos_offset, bool isRelativeCoord);
    void dWwell(Cmd(&cmd));
    void printErr();

  private: 
    String message;
};

#endif
