/*
 * TemperatureProfileManager.cpp
 *
 *  Created on: Jul 16, 2011
 *      Author: john wulf
 *
 *  TODO: Make the storage mechanism flexible.
 *
 *  TODO: Error handling for fundamental storage
 */

#include "TemperatureProfileManager.h"
#include "TemperatureProfile.h"
#include <EEPROM.h>

#define EMPTY_PROFILE_MARKER -1

int TemperatureProfileManager::_memoryAdr = -1;
int TemperatureProfileManager::_memorySize = -1;
int TemperatureProfileManager::_memoryBlockSize = -1;
int TemperatureProfileManager::_maxNumOfProfiles = 0;

TemperatureProfileManager TemperatureProfileManager::instance = TemperatureProfileManager();

void TemperatureProfileManager::setMemoryInfo(int start, int profileCount) {
  if((start >= 0) && (profileCount > 0)) {
    // TODO: We need to manage increases and decreases
    //       for now we assume that nothing changes and this is
    //       initialized always with the same size.
    //       The thing really is, writing a simple file system/memory
    //       management service that can work on any external storage
    //       and not just the EEPROM. Next library after I am done with
    //       the HVAC board.
    _memoryAdr = start;
    _maxNumOfProfiles = profileCount;
    _memoryBlockSize = sizeof(TemperatureProfile);
    _memorySize = profileCount * _memoryBlockSize;
  }
}

TemperatureProfileManager::TemperatureProfileManager() {
  // TODO Auto-generated constructor stub

}

TemperatureProfile& TemperatureProfileManager::getProfile() {
  return _profile;
}

bool TemperatureProfileManager::load(int id) {
  int addr, size, time, temperature, i;

  if(isInitialized()) {
    addr = find(id);

    if(addr >= 0) {
      // This all just assumes sunny day scenario, i.e. no corruption
      _profile.clear();

      // Profile id
      _profile.setId(readByte(addr++));

      // Size of the profile
      size = readByte(addr++);

      // Name of the profile
      for(i = 0; i < MAX_NAME_SIZE; i++, addr++) {
        _profile.getName()[i] = readByte(addr);
      }

      // Read the time/temperature value pairs
      for(i = 0; i < size; i++) {
        time = readByte(addr++);
        temperature = readByte(addr++);
        _profile.add(time, temperature);
      }

      return true;
    }
  }

  return false;
}


bool TemperatureProfileManager::save() {
  int addr, time, temperature, i;

   if(isInitialized()) {
     addr = find(_profile.getId());

     if(addr < 0) {
       // No existing profile with the defined id
       // So, let's see if we have space
       addr = find(EMPTY_PROFILE_MARKER);
     }

     if(addr >= 0) {
       writeByte(addr++, _profile.getId());

       // Size of the profile
       writeByte(addr++, _profile.size());

       for(i = 0; i < MAX_NAME_SIZE; i++, addr++) {
         writeByte(addr, _profile.getName()[i]);
       }

       // Read the time/temperature value pairs
       for(i = 0; i < _profile.size(); i++) {
         _profile.getAt(i, time, temperature);
         writeByte(addr++, time);
         writeByte(addr++, temperature);
       }

       return true;
     }
   }

   return false;
}

bool TemperatureProfileManager::remove() {
  if(isInitialized()) {
    int addr = find(_profile.getId());

    if(addr >= 0) {
      writeByte(addr, EMPTY_PROFILE_MARKER);
      return true;
    }
  }
  return false;
}

bool TemperatureProfileManager::format() {
  int addr = _memoryAdr;

  if(isInitialized()) {
    for(int i = 0; i < _maxNumOfProfiles; i++, addr += _memoryBlockSize) {
      // All we got to do is mark each record with -1.
      writeByte(addr, EMPTY_PROFILE_MARKER);
    }
    return true;
  }

  return false;
}

int TemperatureProfileManager::freeSpace() {
  int addr = _memoryAdr;
  int empty = -1;

  if(isInitialized()) {
    empty = 0;
    for(int i = 0; i < _maxNumOfProfiles; i++, addr += _memoryBlockSize) {
      if(readByte(addr) == EMPTY_PROFILE_MARKER) {
        empty++;
      }
    }
  }

  return empty;
}

// ------------------------------------------------------
// Private methods
//

int TemperatureProfileManager::find(int id) {
  int addr = _memoryAdr;

  if(isInitialized()) {
    for(int i = 0; i < _maxNumOfProfiles; i++, addr += _memoryBlockSize) {
      if(readByte(addr) == id) {
        // don't like this, return out of a for loop.
        // not really clean, but code efficient.
        return addr;
      }
    }
  }

  return -1;
}

inline int8_t TemperatureProfileManager::readByte(int addr) {
  return (int8_t) EEPROM.read(addr);
}

inline void TemperatureProfileManager::writeByte(int addr, int8_t value) {
  EEPROM.write(addr, value);
}

inline bool TemperatureProfileManager::isInitialized() {
  return _maxNumOfProfiles > 0;
}
