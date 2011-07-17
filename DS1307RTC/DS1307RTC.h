/*
 * DS1307RTC.h - library for DS1307 RTC
 * This library is intended to be uses with Arduino Time.h library functions
 */

#ifndef _DS1307RTC_h_
#define _DS1307RTC_h_

#include <WProgram.h>
#include <Time.h>

#define RTC DS1307RTC::instance

// library interface description
class DS1307RTC {
  public:
    enum SquareWaveRate {
      SQW_1HZ = 0x00,
      SQW_4096HZ = 0x01,
      SQW_8192HZ = 0x02,
      SQW_32768HZ = 0x03
    };

    static DS1307RTC instance;

		/*
		 * Initializes the clock object instance. This method will disable the power management
		 * feature and will not initialize the <code>Wire</code> library. It will call
		 * <code>initialize(-1, false)</code> and is implemented as an inline function.
		 *
		 * @return <code>true</code> if the clock was properly initialized;<code>false</code> otherwise.
		 * @see DS1307RTC::initialize(int8_t, bool)
		 */
    bool initialize();

    /*
     * Initializes the clock object instance. This method will call
     * <code>initialize(pin, false)</code> and is implemented as an inline function.
     *
     * @param[in] pin the Arduino I/O pin that controls the VCC of the clock chip. If <code>-1</code>
     *                is passed it is interpreted as no I/O pin connected to the VCC of the clock chip.
     *
     * @return <code>true</code> if the clock was properly initialized;<code>false</code> otherwise.
     * @see DS1307RTC::initialize(int8_t, bool)
     */
    bool initialize(int8_t pin);

    /*
     * Initializes the clock object instance. The <code>DS1307RTC</code> object has to be initialized before
     * it can be used properly. If not initialized, the proper function is not guaranteed. The initialization
     * goes through four steps:
     * <ol>
     *  <li>
     *    <b>Power Management</b> - If the specified <code>pin</code> is greater than <code>-1</code>
     *    power management is enabled.
     *    The power management function assumes that the specified <code>pin</code> is
     *    connect to VCC of the DS1307, hence can control the power to the DS1307. Turning off the power will
     *    send the DS1307 into low power mode and draw it's power from the backup battery. Power will only
     *    be turned on when data is read from or written to the DS1307.
     *  <li>
     *    <b>DS1307 Status</b> - The method will read the <code>second</code> register to see if the DS1307 is started
     *    or not.
     *  <li>
     *    <b>Turn of Squae Wave Generator</b> - The method will turn of the wave generation by calling
     *    <code>stopSquareWave</code>.
     *  <li>
     *    <b>24-hour Mode</b> - The method will make sure that the chip is in 24-hour mode, rather than in
     *    AM/PM.
     * <ol>
     *
     * @param[in] pin the Arduino I/O pin that controls the VCC of the clock chip. If <code>-1</code>
     *                is passed it is interpreted as no I/O pin connected to the VCC of the DS1307 and
     *                power management is turned off.
     * @param[in] initWire if <code>true</code> is passed, <code>Wire.begin()</code> is called to initialized
     *                     the I2C bus; otherwise, it is assumed that the I2C bus was initialized externally.
     *
     * @return <code>true</code> if the clock was properly initialized;<code>false</code> otherwise.
     * @see <a href="http://arduino.cc/en/Reference/Wire">Arduino Wire Library</a>
     */
    bool initialize(int8_t pin, bool withWire);

		/**
		 * Start the clock by setting the CH bit (bit seven) of the seconds register).
		 *
		 * @return <code>true</code> if the clock could be started;<code>false</code> otherwise.
		 */
		bool start();
	
		/**
		 * Stops the clock by clearing the CH bit (bit seven) in the seconds register.
		 *
     * @return <code>true</code> if the clock could be started;<code>false</code> otherwise.
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

	  byte readControlRegister();

	  bool startSquareWave(SquareWaveRate rate);

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
    DS1307RTC();

		uint8_t dec2bcd(uint8_t num);
  	uint8_t bcd2dec(uint8_t num);

//    static DS1307RTC instance = RTC();

  	bool running;
  	bool squareWave;
  	int8_t powerMgmtPin;

};

inline bool DS1307RTC::initialize() {
  return initialize((int8_t)-1, (bool)false);
}

inline bool DS1307RTC::initialize(int8_t pin) {
  return initialize(pin, false);
}

#endif
 

