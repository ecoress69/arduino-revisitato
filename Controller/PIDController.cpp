/*
*/

#include "WProgram.h"

#include "PIDController.h"

#define NEUTRAL_STATE 0;
#define UPDATE_STATE 1;
#define CONTROL_STATE 2;

PIDController::PIDController(float uMin, float uMax, 
                             float pGain, float iGain, float dGain, 
                             float setpoint, float y) {
  updateParameters(uMin, uMax, pGain, iGain, dGain);
  updateSetpoint(setpoint);
  
	reset(y);
}


void PIDController::updateParameters(float uMin, float uMax, float pGain, float iGain, float dGain) {
  this->uMin = uMin;
  this->uMax = uMax;
  this->pGain = pGain;
  this->iGain = iGain;
  this->dGain = dGain;
  this->state = NEUTRAL_STATE;
}

void PIDController::updateSetpoint(float setpoint) {
  this->setpoint = setpoint;
  this->state = UPDATE_STATE;
}

void PIDController::reset(float y) {
  lastY = y;
  integral = 0;
  lastMillis = -1;
}

float PIDController::calculateOutput(float y) {
  float e = setpoint - y;
  float v;
  float p, i, d;
  float h;
  
  // We skip the first call to just set the last Millis
  if(lastMillis < 0) {
    lastMillis = millis();
    return 0;
  }
  
  h = getDeltaT();
  
  // Proportional term
  p = pGain * e;
  Serial.print("p:");
  Serial.print(p);
  
  // We forward calucalated the integral part
  i = iGain * integral;
  Serial.print(", i:");
  Serial.print(i);
  
  // Backward calculation of differential part
  d = dGain * (y - lastY) / h;
  Serial.print(", d:");
	Serial.print(d);
  
  // Calculate new u value 
  // d is negative, b/c a positive gain means we need to slow down
	v = p + i - d;
  
  Serial.print(", v");
	Serial.print(v);
  
  // limit the control value
  u = limitControl(v);
  Serial.print(", u");
	Serial.println(u);
  

  // Avoid integral windup. If the we reached max output,
  // we suspend further integration, i.e. the integral part
  // will not grow until the system is no longer on max.
  if(u == v) {
    integral = integral + e * h;  
  }

  // Memorize the last position
  lastY = y;
    
  return u;
}


float PIDController::limitControl(float v) {
  if(v > uMax) {
    v = uMax;
  }
  
  if(v < uMin) {
    v = uMin;
  }
  
  return v;
}

float PIDController::getDeltaT() {
  float now;
  float h;
  
  now = millis();
  h = now - lastMillis;
  lastMillis = now;
  
  return h / 1000.0;  
}

float PIDController::getIntegralState() {
  return integral;
}

float PIDController::getLastY() {
  return lastY;
}

void PIDController::printDebug() {
  Serial.print(" setpoint:");
  Serial.print(setpoint);
  Serial.print(", state");
  Serial.print(state);
  Serial.print(", u:");
  Serial.print(u);
  Serial.print(", integral:");
  Serial.print(integral);
  Serial.print(", lasty:");
  Serial.print(lastY);
  Serial.print(", lastMillis:");
  Serial.print(lastMillis);
}

