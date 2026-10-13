#ifndef GRIPPER_H_
#define GRIPPER_H_

class Gripper {
public:
  Gripper(int aStepPin, int aDirPin, int aEnablePin, bool aInverse);
  
  void enable(bool value = true);
    
  int getMaxOpening() const;
  int getOpening() const;
  int getPress() const;
  bool getState() const;
  int getType() const;

  void homingManual();
  void on();
  void off();
  void openAbsolute(int mm);
  void openRelative(int mm);
  
  void setType(int aType);
  
private:
  int stepPin;
  int dirPin;
  int enablePin;  
  bool inverse;           // direccion de movimiento considerando def en config.h
  bool state;
  int type; 
  int maxOpening;         // apertura máxima [mm] de la pinza
  bool pressDetect;
  float stepsPerMM;
  int opening;            // apertura [mm] de la pinza
  int stepsTargetOpening; // pasos desde la posicion inicial a la apertura deseada
  int stepsCurrentOpening;// pasos desde la posicion inicial a la apertura en curso
};
#endif
