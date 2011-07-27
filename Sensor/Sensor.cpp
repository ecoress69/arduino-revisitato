/*
 * Sensor.cpp
 *
 *  Created on: Jul 26, 2011
 *      Author: john
 */

#include "Sensor.h"

SensorImpl::SensorImpl() {
  initialize();
}

Sensor::Error SensorImpl::initialize() {
  _lastReadTime = 0;
  _sampling = false;

  return NO_ERROR;
}


Sensor::Error SensorImpl::beginSampling() {
  Error err = NO_ERROR;

  if(!_sampling) {
    err = beginSamplingImpl();
    if(err == NO_ERROR) {
      _sampling = true;
    }
  }

  return err;
}

Sensor::Error SensorImpl::endSampling() {
  Error err = NO_ERROR;

  if(_sampling) {
    err = endSamplingImpl();
    if(err == NO_ERROR) {
      _sampling = false;
    }
  }

  return err;
}


Sensor::Error SensorImpl::reset() {
  _sampling = false;
  _lastReadTime = 0;

  return NO_ERROR;
}

// ---- SensorAdaptor ------
SensorAdapter::SensorAdapter(Sensor *sensor, int config) {
  _sensor = sensor;
  _config = config;
};

