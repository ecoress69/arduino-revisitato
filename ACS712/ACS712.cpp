/*
 * ACS712.cpp
 *
 *  Created on: Jul 26, 2011
 *      Author: john
 */

#include "ACS712.h"

ACS712::ACS712(int analogPin, int vccPin)
  : SensorImpl() {

  _vccPin = vccPin;
  _analogPin = analogPin;
  reset();
}

Sensor::Error ACS712::reset() {
  _on = true;
  _value = 0;
  setCalibration(0, 1);

  // If there is a vcc pin, we are use power management
  if(_vccPin > -1) {
    pinMode(_vccPin, OUTPUT);
    digitalWrite(_vccPin, LOW);
    _on = false;
  }

  return NO_ERROR;
}

Sensor::Error ACS712::beginSamplingImpl() {
  // This is simple, we just turn the power on
  on();
  return NO_ERROR;
}

Sensor::Error ACS712::endSamplingImpl() {
  // Stop sampling is a little more difficult, b/c off() will not work
  if((_vccPin >= 0) && (_on)) {
    digitalWrite(_vccPin, LOW);
    _on = false;
  }

  return NO_ERROR;
}

Sensor::Error ACS712::readSensorImpl(unsigned long timeInMillis, int config) {
  on(); // Only has effect if there is a power pin and we are not in sampling mode
  _value = analogRead(_analogPin);
  off();

  return NO_ERROR;
}


int ACS712::getIntegerValue(int config) {
  return _value;
}

byte ACS712::getByteValue(int config) {
  return (byte)(_value >> 2);
}

float ACS712::getFloatValue(int config) {
  // I = (V - 2.5V) / 185 mV
  return ((float)(_value - _offset))/_scale;
}

void ACS712::on() {
  // We only turn on power to the sensor if we are not
  // in sampling mode and the power was off.
  if((_vccPin >= 0) && (!isSampling()) && (!_on)) {
    digitalWrite(_vccPin, HIGH);

    // 35us to 90% steady state, we wait twice that long to guarantee that
    // steady state has been reached.
    delayMicroseconds(70);
  }
}

void ACS712::off() {
  // We only turn off power to the sensor if we are not
  // in sampling mode.
  if((_vccPin >= 0) && (!isSampling())) {
    digitalWrite(_vccPin, LOW);
    _on = false;
  }
}

