/*
  PIDController - Implementation of PID controller
*/


#ifndef PIDCONTROLLER_H_
#define PIDCONTROLLER_H_


class PIDController {
public:
  PIDController(float uMin, float uMax, float pGain, float iGain, float dGain, 
                float setpoint, float y);
  
  void reset(float y);
  
  void updateParameters(float uMin, float uMax, float pGain, float iGain, float dGain);
  
  void updateSetpoint(float setpoint);
  
  float calculateOutput(float y);

  float getIntegralState();

  float getLastY();
  
  void printDebug();

private:
  // Limits for controll function
  float uMin;
  float uMax;
  float u;
  
  //PID gains
  float pGain;
  float iGain;
  float dGain;
      
  // setpoint
  float setpoint;
  
  // running parameters
  float integral;
  int state;
  
  float lastY;
  
  float lastMillis;
  
  float limitControl(float u); 
  float getDeltaT();
};

#endif
