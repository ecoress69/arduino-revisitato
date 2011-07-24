/*
 * TemperatureProfileManager.h
 *
 *  Created on: Jul 16, 2011
 *      Author: john
 */

#include "TemperatureProfile.h"

#ifndef TEMPERATUREPROFILEMANAGER_H_
#define TEMPERATUREPROFILEMANAGER_H_

#include <WProgram.h>

#define TPM TemperatureProfileManager::instance

#define TPROFILE TemperatureProfileManager::instance.getProfile()

class TemperatureProfileManager {
  public:
    /**
     * Sets the memory address in the EEPROM where all profiles are stored. Each profile will take
     * <code>id * (TEMPERATUREPROFILE_SLOTS * 2 + 2)</code> bytes in the EEPROM. The number of profiles
     * is limited by the amount of memory made available. If a profile is deleted, the profile id
     * is set to <code>-1</code> indicating that the memory can be used for to store a new profile.
     */
    static void setMemoryInfo(int startAddress, int profileCount);
    static int maxNumOfProfiles() { return _maxNumOfProfiles; };
    static TemperatureProfileManager instance;

    TemperatureProfile &getProfile();

    bool load(int id);
    bool exists(int id);
    bool save();
    bool remove();
    bool format();
    /**
     * @return the number of free entries in the persistent store or <code>-1</code> if the store has
     *         not been initialized.
     */
    int freeSpace();

    int used() { return maxNumOfProfiles() - freeSpace(); };



  private:
    TemperatureProfileManager();

    /**
     * Finds the profile with the given id. If -1 is passed, it will find the first empty entry
     * if such exists.
     */
    int find(int id);
    bool isInitialized();
    int8_t readByte(int addr);
    void writeByte(int addr, int8_t value);

    TemperatureProfile _profile;
    static int _memoryAdr;
    static int _memorySize;
    static int _memoryBlockSize;
    static int _maxNumOfProfiles;
};

inline bool TemperatureProfileManager::exists(int id) {
  return find(id) >= 0;
}


#endif /* TEMPERATUREPROFILEMANAGER_H_ */
