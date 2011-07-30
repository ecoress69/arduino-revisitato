#include "WProgram.h"

#include "BBController.h"


BBController::BBController(Mode mode) {
  _mode = mode;
  _state = OFF;
}

bool BBController::setMode(Mode mode) {
  if(_mode != mode) {
    _mode = mode;
    _state = CONTROL_WAIT;
    return true;
  }

  return false;
}

bool BBController::updateSetpoint(float setpoint, float tolerance, float y) {
  _setpoint = setpoint;
  _tolerance = tolerance;
  _state = CONTROL_WAIT;
  return controlOn(y, 0);
}
  
bool BBController::controlOn(float y) {
  return controlOn(y, _tolerance);
}

// we pass in the tolerance so we can force the control to turn
// on when we update the setpoint (if required), by setting the tolerance to
// zero.
bool BBController::controlOn(float y, float tolerance) {
  if((_state == CONTROL_ON) &&
    (((y >= _setpoint) && (_mode == NORMAL)) ||
     ((y <= _setpoint) && (_mode == INVERSE)))) {
    // Need to change from ON to WAIT
    _state =  CONTROL_WAIT;
  }
  else if((_state == CONTROL_WAIT) &&
         (((y < (_setpoint - tolerance)) && (_mode == NORMAL)) ||
          ((y > (_setpoint + tolerance)) && (_mode == INVERSE)))) {
    // Need to change from WAIT to ON
    _state =  CONTROL_ON;
  }
  
  // Obviously, the conditions for turning off the control
  // aren't met, so we must be in CONTROL_ON state.
  return _state == CONTROL_ON;
}


void BBController::printDebug() {
  Serial.print(" setpoint:");
  Serial.print(_setpoint);
  Serial.print(", state:");
  Serial.print(_state);
  Serial.print(", tolerance:");
  Serial.print(_tolerance);
  Serial.print(", mode:");
  Serial.print(_mode);
}
