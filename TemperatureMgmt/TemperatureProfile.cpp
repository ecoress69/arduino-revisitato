/*
 * TemperatureProfile.cpp
 *
 *  Created on: Jul 16, 2011
 *      Author: john
 */

#include "TemperatureProfile.h"
#include <stdio.h>
#include <string.h>

TemperatureProfile::TemperatureProfile() {
  _id = -1;
  _size = 0;

  for(int i = 0; i < MAX_SIZE + TP_ARRAY_OFFSET * 2; i++) {
    _entries[i][0] =TP_DEFAULT_VALUE;
    _entries[i][1] = TP_DEFAULT_VALUE;
  }
}

void TemperatureProfile::setId(int id) {
  _id = id;
  clear();
}

int TemperatureProfile::getId() {
  return _id;
}

void TemperatureProfile::setName(const char * name) {
  strncpy(_name, name, MAX_NAME_SIZE - 1);
  _name[MAX_NAME_SIZE - 1] = 0;
}

char * TemperatureProfile::getName() {
  return _name;
}

int TemperatureProfile::add(int time, int setPoint) {
  int index = -1 + TP_ARRAY_OFFSET;

  index = findIndex(time);

  // Check if we already have an entry with the given time
  if((index < (_size + TP_ARRAY_OFFSET)) && (_entries[index][TIME] == time)) {
    // if so, just override the value;
    _entries[index][TEMPERATURE] = setPoint;
  }
  else {
    // We don't insert if the profile is already full
    if(_size < MAX_SIZE) {
      insertAt(index, time, setPoint);
      return index - TP_ARRAY_OFFSET;
    }
  }

  return -1;
}

bool TemperatureProfile::getAt(int index, int &time, int &setPoint) {
  bool found = false;
  time = -1;
  setPoint = -1;

  if((index >= 0) && (index < _size )) {
    time = _entries[index + TP_ARRAY_OFFSET][TIME];
    setPoint = _entries[index + TP_ARRAY_OFFSET][TEMPERATURE];
    found = true;
  }

  return found;
}

int TemperatureProfile::get(int time) {
  int index;

  if(_size >  0) {
    index = findIndex(time);
    if((index < (_size + TP_ARRAY_OFFSET)) &&
       (index >= TP_ARRAY_OFFSET) &&
       (_entries[index][TIME] == time)) {
      // We want the "next" higher index or the exact index.
      return _entries[index][TEMPERATURE];
    }
  }

  return -1;
}


int TemperatureProfile::indexOf(int time, int exact) {
  int index;

  if(_size >  0) {
    index = findIndex(time);
    if((index >= TP_ARRAY_OFFSET) && (index <= (_size + TP_ARRAY_OFFSET))) {
      if((_entries[index][TIME] == time) && (index < (_size + TP_ARRAY_OFFSET))) {
        return index - TP_ARRAY_OFFSET;
      }
      else if((exact > 0) && (index < (_size + TP_ARRAY_OFFSET))){
        return index - TP_ARRAY_OFFSET;
      }
      else if((exact < 0) && (index > TP_ARRAY_OFFSET)) {
        return index - TP_ARRAY_OFFSET - 1;
      }
    }
  }

  return -1;
}

int TemperatureProfile::replace(int timeToReplace, int newTime, int newSetPoint) {

  // for now, we just do remove and add
  remove(timeToReplace);
  return add(newTime, newSetPoint);
}


int TemperatureProfile::remove(int time) {
  int index;

  if(_size > 0) {
    index = findIndex(time);

    if((index < (_size + TP_ARRAY_OFFSET)) && (index >= 0) && (_entries[index][TIME] == time)) {
      for(int i = index; i < (_size + TP_ARRAY_OFFSET - 1); i++) {
        _entries[i][TIME] = _entries[i + 1][TIME];
        _entries[i][TIME] = _entries[i + 1][TEMPERATURE];
      }
      _size--;
      return index - TP_ARRAY_OFFSET;
    }
  }

  return -1;
}

void TemperatureProfile::clear() {
  _size = 0;
  _name[0] = 0;
}

int TemperatureProfile::size() {
  return _size;
}


// ---------------------------------------------------------------
// Private methods
//

int TemperatureProfile::findIndex(int time) {
  int index = TP_ARRAY_OFFSET;

  for(index = TP_ARRAY_OFFSET; index < (_size + TP_ARRAY_OFFSET); index++) {
    if(_entries[index][TIME] >= time) {
      break;
    }
  }

  return index;
}

void TemperatureProfile::insertAt(int index, int time, int setPoint) {
  if(index < (MAX_SIZE + TP_ARRAY_OFFSET))  {
    // Prevent overruns
    if(_size < MAX_SIZE) {
      _size++;
    }

    for(int i = _size - 1 + TP_ARRAY_OFFSET; i > index; i--) {
      _entries[i][TIME] = _entries[i - 1][TIME];
      _entries[i][TEMPERATURE] = _entries[i - 1][TEMPERATURE];
    }

    _entries[index][TIME] = (uint8_t) time;
    _entries[index][TEMPERATURE] = (uint8_t) setPoint;
  }
}

bool TemperatureProfile::getRawAt(int index, int &time, int &setPoint) {
#if TP_ARRAY_OFFSET > 0
  // Let's make sure that we return something that is not equal to the integrity value
  time = setPoint = TP_DEFAULT_VALUE - 10;
  if((index >=0) && (index < (MAX_SIZE + TP_ARRAY_OFFSET * 2))) {
    time = _entries[index][TIME];
    setPoint = _entries[index][TEMPERATURE];
    return true;
  }

  return false;
#else
  return true;
#endif
};



bool TemperatureProfile::checkIntegrity() {
#if TP_ARRAY_OFFSET > 0
  bool passed = true;

  for(int i = 0; i < TP_ARRAY_OFFSET; i++) {
    passed &= (_entries[i][0] == TP_DEFAULT_VALUE);
    passed &= (_entries[i][1] == TP_DEFAULT_VALUE);
    passed &= (_entries[i + MAX_SIZE + TP_ARRAY_OFFSET][0] == TP_DEFAULT_VALUE);
    passed &= (_entries[i + MAX_SIZE + TP_ARRAY_OFFSET][1] == TP_DEFAULT_VALUE);
  }

  return passed;
#else
return true;
#endif
};


