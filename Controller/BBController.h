/*
 BBController - Implementation of Bang Bang controller
 */


#ifndef BBCONTROLLER_H_
#define BBCONTROLLER_H_


class BBController {
public:
  enum Mode { NEUTRAL = 0, NORMAL = 1, INVERSE = 2 };
  enum State { OFF = 0, CONTROL_WAIT = 1, CONTROL_ON = 2 };

  
  BBController(Mode mode);
  
  /*!
   * targetControlTimeRatio the ratio between how long it takes to reach the lower control boundary
   *                        when no energy is applied (cooling down) to how long it takes to reach the
   *                        setpoint when energy is applied.
   */
  bool updateSetpoint(float setpoint, float y);

  void setTolerance(float tolerance) { _tolerance = tolerance; };

  /**
   * @return <code>true</code> if <code>newMode</code> changed the mode of the controller. This will require
   *         a call to <code>updateSetpoint</code>. <code>false</code> will be returned if <code>mode</code>
   *         did not change the mode of the controller, i.e. the controller was already in <code>mode</code>.
   */
  bool setMode(Mode mode);

  bool controlOn(float y);

  void printDebug();
  
private:
  bool controlOn(float y, float tolerance);
  // setpoint
  float _setpoint;
  float _tolerance;

  // running parameters
  State _state;
  Mode _mode;
  
};

#endif
