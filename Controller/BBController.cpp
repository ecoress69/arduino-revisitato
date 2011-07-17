#include "WProgram.h"

#include "BBController.h"


BBController::BBController(int uMax, unsigned long sampleTime, Mode mode) {
  this->uMax = uMax;
  this->sampleTime = sampleTime;
  this->mode = mode;
  this->state = OFF;
}

bool BBController::setMode(Mode newMode) {
  if(mode != newMode) {
    mode = newMode;
    return true;
  }
  return false;
}

void BBController::reset(float y) {
  this->lastY = y;
  this->timeSinceLastSample = 0;
  this->timePassed = 0;
  this->lastMillis = millis();
  this->state = CONTROL_ON;  
}

void BBController::updateSetpoint(float setpoint, float tolerance, float targetTime, float targetControlTimeRatio, float y) {
  this->setpoint = setpoint;
  this->tolerance = tolerance;
  this->targetTime = targetTime;
  this->targetControlTimeRatio = targetControlTimeRatio;
  reset(y);
  
  // Set the output
  this->u = 1;
}
  
int BBController::calculateOutput(float y) {
  float h;

  h = ((float)(millis() - lastMillis)) / 1000.0;
  timePassed += h;
  timeSinceLastSample += h;

  if(state == CONTROL_ON) {
    if(((y < setpoint) && (mode == NORMAL)) ||
       ((y > setpoint) && (mode == INVERSE))) {
      // We haven't reached the setpoint
      if(timeSinceLastSample >= sampleTime) {
        if((u < uMax) && increaseControlValue(y)) {
          u++;
        }
        timeSinceLastSample = 0;
      }
    }
    else {
      // The setpoint has been reached, so we go into contol wait mode
      u = 0;
      state = CONTROL_WAIT;
      timePassed = 0;
    }
  }
  else if(state == CONTROL_WAIT) {
    if(((y < (setpoint - tolerance)) && (mode == NORMAL)) ||
       ((y > (setpoint + tolerance)) && (mode == INVERSE))) {
      // The system reached the point that requires it to turn on again
      u = 1;
      state = CONTROL_ON;
      
      // As target time, we use the time it took to reach this point
      // making the time adaptive. For example, if it cools down quickly
      // we heat NORMAL quickly gain.
      targetTime = timePassed * targetControlTimeRatio;
      timePassed = 0;
      timeSinceLastSample = 0;
    }
  }
  else {
    u = 0;
  }
  
  lastY = y;
  // Remember when the last time was called
  lastMillis = millis();
  return u;
}


bool BBController::increaseControlValue(float y) {
  boolean increase = false;
  float a, b;
  float eta;
  
  a = (y -  lastY) / ((float) timeSinceLastSample);
  if(abs(a) > 0.1) {
    b = y - a * timePassed;
    eta = (setpoint - b) / a;
    if(eta > targetTime) {
      // We will not make our target time, however if we are close 
      // we will not further increase. We just slowly creep NORMAL to the
      // setpoint.
      if(timePassed < (0.8 * targetTime)) {
        increase = true;
      }
    }
  }
  else {
    // the system is flat, so increase
    increase = true;
  }
  
  return increase;
}

float BBController::getLastY() {
  return lastY;
}

void BBController::printDebug() {
  Serial.print(" setpoint:");
  Serial.print(setpoint);
  Serial.print(", state:");
  Serial.print(state);
  Serial.print(", tolerance:");
  Serial.print(tolerance);
  Serial.print(", u:");
  Serial.print(u);
  Serial.print(", lasty:");
  Serial.print(lastY);
  Serial.print(", lastMillis:");
  Serial.print(lastMillis);
  Serial.print(", timePassed:");
  Serial.print(timePassed);
  Serial.print(", timeSinceLastSample:");
  Serial.print(timeSinceLastSample);
  Serial.print(", targetTime:");
  Serial.print(targetTime);
  Serial.print(", targetControlTimeRatio:");
  Serial.print(targetControlTimeRatio);
  Serial.print(", mode:");
  Serial.print(mode);
}
