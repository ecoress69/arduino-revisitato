/*
 * TemperatureManager.cpp
 *
 *  Created on: Jul 23, 2011
 *      Author: john
 */

#include "TemperatureManager.h"
#include "TemperatureProfileManager.h"
#include "TemperatureProfile.h"
#include <Time.h>
#include <EEPROM.h>

TemperatureManager TemperatureManager::instance = TemperatureManager();

int TemperatureManager::_memoryAddr = -1;

void TemperatureManager::setMemoryInfo(int addr) {
  _memoryAddr = addr;
  instance.load();
};

TemperatureManager::TemperatureManager() {
  _timeOfYear = SUMMER;
  _amBegin = 6;
}

void TemperatureManager::clear() {
  for(int i = 0; i < MAX_DAYS; i++) {
    for(int j = 0; j < MAX_TIME_OF_DAY; j ++) {
      for(int k = 0; k < MAX_TIME_OF_YEAR; k++) {
        _profiles[i][j][k] = -1;
      }
    }
  }
  save();
}


time_t TemperatureManager::nextSetPointChange(time_t now) {
  int index;
  int time = -1, temperature;
  tmElements_t nowTE, nextTE;
  time_t changeTime = 0;

  // We need to correct the time to offset the shift in AM/PM profiles
  // Profiles don't start at 12 AM, they start at 12 AM + _amBegin
  // the offset allows to find the correct profiles.
  now = adjustTime(now);

  // Break time into its elements
  //This is nasty, but what to do at this point
  breakTime(now, nowTE);

  if(loadProfile(nowTE)) {
    // Get the index that matches exact or is larger
    index = TPROFILE.indexOf(timeToQuarterHour(nowTE, true), 1);
    if(index >= 0) {
      TPROFILE.getAt(index, time, temperature);
    }
    else {
      // We need the next profile
      breakTime(now + 12L * 3600L, nowTE);
      if (loadProfile(nowTE)) {
        // All we need is the first entry
        TPROFILE.getAt(0, time, temperature);
        // B/c this is the next profile, we need to add 12 hrs to it
      }
    }
  }

  if(time != -1) {
    // The change time is the time given minus offset (find the the beginning time of the profile)
    // plus the time found in the profile (time 15 minutes).
    changeTime = makeTime(nowTE) - calcTimeOffset(nowTE) + time * 15L * 60L + (long)_amBegin * 3600L;
  }

  return changeTime;
}

int TemperatureManager::getSetPointFor(time_t timeInSeconds) {
  int time, temperature, index;
  tmElements_t te;

  // Initialize with failure of obtaining a temperature
  temperature = -1;

  timeInSeconds = adjustTime(timeInSeconds);

  // Break the time into its elements
  breakTime(timeInSeconds, te);
  /*Serial.print(millis());
  Serial.print("|Time: ");
  Serial.print(te.Day, DEC);
  Serial.print("-");
  Serial.print(te.Hour, DEC);
  Serial.print(":");
  Serial.print(te.Minute, DEC);
  Serial.print(":");
  Serial.print(te.Second, DEC);*/

  if(loadProfile(te)) {

    index = TPROFILE.indexOf(timeToQuarterHour(te, false), -1);
    if(index >= 0) {
      TPROFILE.getAt(index, time, temperature);
    }
    else {
      // This means we need the previous profile
      breakTime(timeInSeconds - 12L * 3600L, te);
      if(loadProfile(te)) {
        // All we need is the last entry
        TPROFILE.getAt(TPROFILE.size() - 1, time, temperature);
      }
    }
    /*
    Serial.print(", Profile Info:");
    Serial.print(TPROFILE.getId());
    Serial.print("-");
    Serial.print(time);
    Serial.print("-");
    Serial.println(temperature);
    */
  }

  return temperature;
}

bool TemperatureManager::loadProfile(tmElements_t &te) {
  int profileId;

  // Get the id of the profile that applies to the given time
  profileId = getTemperatureProfileID(te);
  if(profileId >= 0) {
    // Load the profile into memory
    if(TPM.load(profileId)) {
      return true;
    }
  }
  Serial.println("... loading failed");

  return false;
}

int TemperatureManager::getTemperatureProfileID(tmElements_t &te) {
  int ampm = PM;

  if(te.Hour < 12){
    ampm = AM;
  }

  return _profiles[te.Wday - 1][ampm][_timeOfYear];
}

bool TemperatureManager::load() {
  int addr = _memoryAddr;
  if(addr >= 0) {
    _timeOfYear = (int8_t)EEPROM.read(addr++);
    _vacationTemperature[SUMMER] = (uint8_t)EEPROM.read(addr++);
    _vacationTemperature[WINTER] = (uint8_t)EEPROM.read(addr++);

    for(int i = 0; i < MAX_DAYS; i++) {
      for(int j = 0; j < MAX_TIME_OF_DAY; j ++) {
        for(int k = 0; k < MAX_TIME_OF_YEAR; k++) {
          _profiles[i][j][k] = (uint8_t)EEPROM.read(addr++);
        }
      }
    }

    return true;
  }

  return false;
}

bool TemperatureManager::save() {
  int addr = _memoryAddr;

  if(addr >= 0) {
    EEPROM.write(addr++, _timeOfYear);
    EEPROM.write(addr++, _vacationTemperature[SUMMER]);
    EEPROM.write(addr++, _vacationTemperature[WINTER]);

    for(int i = 0; i < MAX_DAYS; i++) {
      for(int j = 0; j < MAX_TIME_OF_DAY; j ++) {
        for(int k = 0; k < MAX_TIME_OF_YEAR; k++) {
          EEPROM.write(addr++, _profiles[i][j][k]);
        }
      }
    }
    return true;
  }

  return false;
}


long TemperatureManager::calcTimeOffset(tmElements_t &te) {
  int hourCorrection = 12;

  if(te.Hour < 12) {
    hourCorrection = 0;
  }

  return (te.Hour - hourCorrection) * 3600L + te.Minute * 60L + te.Second;
}

time_t TemperatureManager::adjustTime(time_t time) {
  return time - (((long)_amBegin) * 3600L);
}


int TemperatureManager::timeToQuarterHour(tmElements_t &te, bool roundUp) {
  int time = te.Hour * 4 + te.Minute / 15;

  if(roundUp && (te.Second > 0)) {
    time++;
  }

  return time > 47 ? time - 48 : time;
}

