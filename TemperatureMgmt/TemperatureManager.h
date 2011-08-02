/*
 * TemperatureManager.h
 *
 *  Created on: Jul 23, 2011
 *      Author: john
 */

#ifndef TEMPERATUREMANAGER_H_
#define TEMPERATUREMANAGER_H_

#include <WProgram.h>
#include <Time.h>

#define TEMPMGR TemperatureManager::instance
/**
 * Assumption: Every profile has at least one entry, which has to be ensured when the profile id is set
 */
class TemperatureManager {
  public:
    static TemperatureManager instance;
    static void setMemoryInfo(int addr);
    static int getMemorySize() { return sizeof(TemperatureManager); };

    enum Days { SUNDAY = 0, MONDAY = 1, TUESDAY = 2, WEDNESDAY = 3, THURSDAY = 4, FRIDAY = 5, SATURDAY = 6, HOLIDAY = 7 , MAX_DAYS};
    enum TimeOfDay { AM = 0, PM = 1 , MAX_TIME_OF_DAY };
    enum TimeOfYear { SUMMER = 0, WINTER = 1, MAX_TIME_OF_YEAR };

    bool setProfile(int profile, Days day, TimeOfDay timeOfDay, TimeOfYear timeOfYear) {
      _profiles[day][timeOfDay][timeOfYear] = profile;
      return save();
    };

    int getProfile(Days day, TimeOfDay timeOfDay, TimeOfYear timeOfYear) {
      return _profiles[day][timeOfDay][timeOfYear];
    }

    bool setVacationTemperature(uint8_t winter, uint8_t summer) {
      _vacationTemperature[SUMMER] = summer;
      _vacationTemperature[WINTER] = winter;
    }

    bool getVacationTemperature() {
      return _vacationTemperature[_timeOfYear];
    }

    void setTimeOfYear(TimeOfYear timeOfYear) {
      _timeOfYear = timeOfYear;
      save();
    };

    TimeOfYear getTimeOfYear() { return (TimeOfYear) _timeOfYear; };

    time_t nextSetPointChange(time_t now);
    /**
     * Returns the set point for the given time.
     * @return the set point for the given time.
     */
    int getSetPointFor(time_t timeInSeconds);

    /**
     * Clears all entries and saves it to the
     */
    void clear();

    void printDebug();

  private:
    TemperatureManager();
    bool load();
    bool save();
    int getTemperatureProfileID(tmElements_t &te);
    bool loadProfile(tmElements_t &te);
    int timeToQuarterHour(tmElements_t &te, bool roundUp);

    /**
     * Calculates the hours, minutes, and seconds of the given time in seconds and returns it.
     */
    long calcTimeOffset(tmElements_t &te);
    /**
     * Subtracts <code>_amBegin</code> from the given time to adjust for when "AM" begins from a temperature
     * profile management perspective. In other words, it moves the time to the day change, e.g. the PM time
     * from 00:00 to _amBegin is moved into the previous day to line up with the normal definition of PM.
     * This allows to avoid some more complicated calculations and simply define a AM/PM profile for each day.
     * It is similar to a "timezone". The temperature profile runs in its own time zone, which is always 6 hours
     * behind the timezone of the clock.
     *
     * TODO: Need better description.
     */
    time_t adjustTime(time_t time);

    static int _memoryAddr;
    /**
     * Three dimensional array to hold the IDs of the profiles for the different days (Seven weekdays plus one for holidays).
     * The dimensions are <code>_profile[dayOfTheWeek][AM|PM][SUMMER|WINTER]</code>.
     */
    int8_t _timeOfYear;
    int8_t _amBegin;
    int8_t _profiles[MAX_DAYS][MAX_TIME_OF_DAY][MAX_TIME_OF_YEAR];
    int8_t _vacationTemperature[MAX_TIME_OF_YEAR];
};

#endif /* TEMPERATUREMANAGER_H_ */
