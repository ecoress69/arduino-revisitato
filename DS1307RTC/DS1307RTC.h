/*
 * DS1307RTC.h - library for DS1307 RTC
 * This library is intended to be uses with Arduino Time.h library functions
 */

#ifndef __DS1307RTC_h__
#define __DS1307RTC_h__

#define DEBUG 1
#include <WProgram.h>
#include <Time.h>

typedef enum e_DS1307SquareWaveRate {
	DS1307_SQWE_1HZ = 0x00,
	DS1307_SQWE_4096HZ = 0x01,
	DS1307_SQWE_8192HZ = 0x02,
	DS1307_SQWE_32768HZ = 0x03
} DS1307SquareWaveRate;

// library interface description
class DS1307RTC {
  // user-accessible "public" interface
  public:
    DS1307RTC();

		/*
		 *Fundamental operations
		 */
    bool initialize(bool withWire);

		/**
		 * Start the clock by setting the CH bit (bit seven) of the seconds register).
		 */
		bool start();
	
		/**
		 * Stops the clock by clearing the CH bit (bit seven) in the seconds register.
		 */
		bool stop();
	
		/**
		 * Returns <code> true </code> if the clock is running or <code>false</code>
		 * if the clock has been halted.
		 * 
		 * @return <code>true</true> if the clock is running;<code>false</code> otherwise.
		 */
		bool isRunning();
	
		// 
		// High level time related functions
		//
	  time_t getTime();

	  bool setTime(time_t t);

	  int8_t getTimeZone();

	  bool setTimeZone(int8_t tz);

	  int writeUserMemory(byte *buffer, int offset, int len);

	  int readUserMemory(byte *buffer, int offset, int len);

	  bool startSquareWave(DS1307SquareWaveRate rate);

	  bool stopSquareWave(bool out);

    /**
     * Reads <code>len</code> bytes from the clock's registers starting at <code>offset</code>.
     *
     * @param[out] buffer The buffer that will hold the content of the registers. The buffer
     *                    has to be at least <code>len</code> bytes long.
     * @param[in] offset The offset of the first register. The value has to be between 0x00 and
     *                   and 0x3f.
     * @param[in] len The number of bytes read from the clock's register if <code>offset + len</code>
     *                is smaller than 0x40;
     *
     * @return the number of bytes read from the clock's register, which is either <code>len</code>
     *         if <code>offset + len < 0x40</code> or <code>0x40 - offset<code> if
     *         <code>offset + len > 0x3f</code>.
     */
    int readBytes(byte *buffer, int offset, int len);

    /**
     * Writes <code>len</code> bytes from the buffer to the clock's registers starting at
     * <code>offset</code>.
     *
     * @param[in] buffer The buffer that will holds the content to be written to the registers.
     *                   The buffer has to be at least <code>len</code> bytes long.
     * @param[in] offset The offset of the first register. The value has to be between 0x00 and
     *                   and 0x3f.
     * @param[in] len The number of bytes to be written to the clock's register if
     *                 <code>offset + len</code> is smaller than 0x40;
     *
     * @return the number of bytes written to the clock's register, which is either <code>len</code>
     *         if <code>offset + len < 0x40</code> or <code>0x40 - offset<code> if
     *         <code>offset + len > 0x3f</code>.
     */
    int writeBytes(const byte *buffer, int offset, int len);
  private:
		uint8_t dec2bcd(uint8_t num);
  	uint8_t bcd2dec(uint8_t num);

  	bool running;
  	bool squareWave;

};

extern DS1307RTC RTC;

#endif
 

