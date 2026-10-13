#ifndef EQUIPMENT_H_
#define EQUIPMENT_H_
#include <Arduino.h>

class Equipment {
public:
  Equipment(String equipment_name, int equipment_pin);
  bool getState() const;
  void on();
  void off();
private:
  int pin;
  String name;
  bool installed;
  bool state;
};

#endif
