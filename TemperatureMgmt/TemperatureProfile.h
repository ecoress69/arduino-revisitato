/*
 * TemperatureProfile.h
 *
 *  Created on: Jul 16, 2011
 *      Author: john
 */

#ifndef TEMPERATUREPROFILE_H_
#define TEMPERATUREPROFILE_H_

#include <WProgram.h>

#ifndef TEMPERATUREPROFILE_SLOTS
#define TEMPERATUREPROFILE_SLOTS 10
#endif

// Change TP_ARRAY_OFFSET from 1 to 0 to test the integrity of the underlying array
// For deployment this should be set to 0
// Not that this is a hack due to the Arduino IDE to run unit tests
// As soon as the unit test has run successfully this should be set to 0, as it adds 4 bytes to
// each profile object.

#define TP_ARRAY_OFFSET 0
#define TP_DEFAULT_VALUE  -2

#define TIME 0
#define TEMPERATURE 1
#define MAX_NAME_SIZE 12


class TemperatureProfile {


  public:

    static const int MAX_SIZE = TEMPERATUREPROFILE_SLOTS;

    TemperatureProfile();
    void setId(int id);
    int getId();

    void setName(const char * name);
    char * getName();

    /**
     * Adds a new entry with the given <code>time</code> and <code>setPoint</code> to this
     * profile. If the profile reached its maximum size, the new entry will not be added.
     * The client application will have to first delete an existing entry to make space.
     *
     * @param time a number from 0 to 47 representing a quarter of an hour in a 12 hour period.
     * @param setPoint the temperature that should be set at <code>time</code>.
     *
     * @return the index of the new entry if it could be added or <code>-1</code> if the
     *         entry could not be added.
     */
    int add(int time, int setPoint);

    /**
     * Replaces the set point of the specified <code>timeToReplace</code> with the the specified
     * <code>newTime> and <code>newSetPoint</code> value pair. This is a convenience method that
     * combines <code>remove</code> and <code>add</code>. Caution has to be taken if <code>newTime</code>
     * is different from <code>timeToReplace</code> as this will result in the removal of the
     * entry with <code>timeToReplace</code> and the replacement of the set point for the
     * already existing entry with <code>newTime</code>. In effect, the array will have
     * one element less, as one was removed and one was <i>updated</i>.
     *
     * @return the index of the new entry if it could be added or <code>-1</code> if the
     *         entry could not be added.
     * @see #add, #remove
     */
    int replace(int timeToReplace, int newTime, int newSetPoint);
    int remove(int time);

    bool getAt(int index, int &time, int &setPoint);

    /**
     * Returns the temperature for the given time or ?????
     */
    int get(int time);

    /**
     * Returns the index for the given time depending on the value of <code>exact</code>:
     * <ol>
     * <li><code>exact < 0</code> return the temperature for the exact time or the time before if there is no exact match
     * <li><code>exact == 0</code> return the temperature only for the exact time or -1 if there is no exact match
     * <li><code>exact > 0</code> return the temperature for the exact time or the time after if there is no exact match
     */
    int indexOf(int time, int exact);

    int indexOf(int time) { return indexOf(time, 0); };

    void clear();
    int size();

    /**
     * Only for unit testing
     */
    bool getRawAt(int index, int &time, int &setPoint);

    /**
     * Only if unit testing is turned on
     */
    bool checkIntegrity();

  private:
    /**
     * @return the index of the entry that is equal <code>time</code> to time. If there is no entry
     *         that is equal to <code>time</code>, the first index greater than <code>time</code> is
     *         returned. If no such index exists, the index returned is equal to <code>size()</code>.
     */
    int findIndex(int time);
    void insertAt(int index, int time, int setPoint);

    int8_t _id;
    char _name[MAX_NAME_SIZE];
    int8_t _entries[MAX_SIZE + 2 * TP_ARRAY_OFFSET][2];
    int8_t _size;
};

#endif /* TEMPERATUREPROFILE_H_ */
