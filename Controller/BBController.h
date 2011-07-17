/*
 BBController - Implementation of Bang Bang controller
 */


#ifndef _BBCONTROLLER_H_
#define _BBCONTROLLER_H_


class BBController {
public:
  enum Mode { NEUTRAL = 0, NORMAL = 1, INVERSE = 2 };
  enum State { OFF = 0, CONTROL_WAIT = 1, CONTROL_ON = 2 };

  
  BBController(int uMax, unsigned long deltaTime, Mode mode);
  
  /*!
   * targetControlTimeRatio the ratio between how long it takes to reach the lower control boundary
   *                        when no energy is applied (cooling down) to how long it takes to reach the
   *                        setpoint when energy is applied.
   */
  void updateSetpoint(float setpoint, float tolerance, float targetTime, float targetControlTimeRatio, float y);

  // Do we need that as an extra method?
  void reset(float y);
  
  /**
   * @return <code>true</code> if <code>newMode</code> changed the mode of the controller. This will require
   *         a call to <code>updateSetpoint</code>. <code>false</code> will be returned if <code>newMode</code>
   *         did not change the mode of the controller, i.e. the controller was already in <code>newMode</code>.
   */
  bool setMode(Mode newMode);

  int calculateOutput(float y);
  
  float getLastY();
  
  void printDebug();
  
private:
  // Limits for controll function
  int uMax;
  int u;
  
  
  // setpoint
  float setpoint;
  float tolerance;
  
  // timing parameters
  float targetTime;
  float targetControlTimeRatio;
  float sampleTime;
  
  float timeSinceLastSample;
  float timePassed;
  
  // running parameters
  State state;
  Mode mode;
  
  float lastY;
  
  unsigned long lastMillis;
  
  bool increaseControlValue(float y);
};

#endif
