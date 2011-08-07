/*
 * DS1307RTC.h - library for DS1307 RTC

  Copyright (c) Michael Margolis 2009
  This library is intended to be uses with Arduino Time.h library functions

  The library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

  30 Dec 2009 - Initial release
 */

#include <Wire.h>
#include "DS1339.h"

#define DS1339_I2C_ID 0x68

#define I2C_MAX_BYTES_TRANSFER 0x1f

// Memory related constants
#define DS1339_MEMORY           0x40
#define DS1339_USERSPACE_START  0x09
#define DS1339_CONTROL_REG      0x07
#define DS1339_TIME_ZONE_REG    0x08

// Bit masks
#define DS1339_CLOCKHALT  0x80
#define DS1339_CTRL_OUT   0x80
#define DS1339_CTRL_SQWE  0x10
#define DS1339_CTRL_RS    0x03

#define DS1339_BCD_LO     0x0f
#define DS1339_BCD_HI     0xf0

#define DS1339_SEC_MASK   0x7f

#define DS1339_HR_MASK    0x3f
#define DS1339_AM_PM      0x30
#define DS1339_12_24      0x40


#define DS1339_DOW_LOW    0x07
#define DS1339_DATE_HI    0x30
#define DS1339_MTH_HI     0x30


DS1339 DS1339::instance = DS1339();

/*
 * Constructor
 */
DS1339::DS1339() {
}

bool DS1339::initialize(int8_t pin, bool withWire) {
  byte buffer;

  // Do we need to initialize the wire library?
  if(withWire) {
    Wire.begin();
  }

  // if the pin is -1, it means no power management. Otherwise there
  // is a pin that can be used to keep the power requirements for the
  // RTC at a minimum. Mostly, turning off the power will cause the
  // clock to go into low power mode instead of standby.
  powerMgmtPin = pin;
  if(pin >= 0) {
    pinMode(powerMgmtPin, OUTPUT);
    digitalWrite(powerMgmtPin, LOW);
  }


  // Check the state of the clock
  if(readBytes(&buffer, 0, 1) == 1) {
    running = !(buffer & DS1339_CLOCKHALT);
  }
  else {
    return false;
  }

  // At initialization, we turn off the square wave
  stopSquareWave(false);
  // Change the clock to 24 hr mode
  // TODO: Add that, but for now this is good enough.

  return true;
}
/*
 * start - will always force the clock to stop, no matter what the cached parameter
 * <code>running</code> says. Allows to get the clock back in a defined status.
 */
bool DS1339::start() {
  byte seconds;

  //  Read the seconds byte
  if(readBytes(&seconds, 0, 1) != 1) {
    return false;
  }

  // Clear the CH bit in the register
  seconds &= DS1339_SEC_MASK;

  // Write it back
  if(writeBytes(&seconds, 0, 1) != 1) {
    return false;
  }

  running = true;

  return true;
}

/*
 * stop - will always force the clock to stop, no matter what the cached parameter
 * <code>running</code> says. Allows to get the clock back in a defined status.
 *
 */
bool DS1339::stop() {
  byte seconds;

  //  Read the seconds byte
  if(readBytes(&seconds, 0, 1) != 1) {
    return false;
  }

  // Set the CH bit in the register
  seconds |= DS1339_CLOCKHALT;

  // Write it back
  if(writeBytes(&seconds, 0, 1) != 1) {
    return false;
  }

  running = false;

  return true;
}

/*
 * isRunning
 */
bool DS1339::isRunning() {
  return running;
}


/*
 * getTime
 */
time_t DS1339::getTime()   // Aquire data from buffer and convert to time_t
{
  byte buffer[7];
  tmElements_t tm;

   if(readBytes(buffer, 0, 7) == 7) {
     tm.Second = bcd2dec(buffer[0] & DS1339_SEC_MASK);
     tm.Minute = bcd2dec(buffer[1]);
     tm.Hour =  bcd2dec(buffer[2] & DS1339_HR_MASK);  // mask assumes 24hr clock
     tm.Wday = bcd2dec(buffer[3]);
     tm.Day = bcd2dec(buffer[4]);
     tm.Month = bcd2dec(buffer[5]);
     tm.Year = y2kYearToTm((bcd2dec(buffer[6])));
   }

  return(makeTime(tm));
}

/*
 * setTime
 */
bool  DS1339::setTime(time_t t) {
  byte buffer[7];
  tmElements_t tm;

  breakTime(t, tm);
  buffer[0] = dec2bcd(tm.Second);
  buffer[1] = dec2bcd(tm.Minute);
  buffer[2] = dec2bcd(tm.Hour);
  buffer[3] = dec2bcd(tm.Wday);
  buffer[4] = dec2bcd(tm.Day);
  buffer[5] = dec2bcd(tm.Month);
  buffer[6] = dec2bcd(tmYearToY2k(tm.Year));

  buffer[0] |= DS1339_CLOCKHALT;  // stop the clock
  if(writeBytes(buffer, 0, 7) !=7) {
    return false;
  }

  if(isRunning()) {
    buffer[0] &= DS1339_SEC_MASK;
    if(writeBytes(buffer, 0, 7) != 7) {
      return false;
    }
  }

  return true;
}

/*
 * getTimeZone - timezone is offset by 12 when it is written, hence it needs to be
 * reversed.
 */
int8_t DS1339::getTimeZone() {
  byte buffer;

  if(readBytes(&buffer, DS1339_TIME_ZONE_REG, 1) != 1) {
    return -128;
  }

  return (int8_t)buffer - 12;
}

/*
 * setTimeZone - to avoid negative values and conversion problems between signed and unsigned,
 * the time zone is offset by 12, i.e. it is always positive from 0 to 24 when written.
 */
bool DS1339::setTimeZone(int8_t tz) {
  byte buffer = tz + 12;

  if(writeBytes(&buffer, DS1339_TIME_ZONE_REG, 1) != 1) {
    return false;
  }

  return true;
}

/*
 * writeUserMemory - the user memory starts at byte 0x09 (0x08 is used for the timezone)
 */
int DS1339::writeUserMemory(byte *data, int offset, int len) {
  return writeBytes(data, offset + DS1339_USERSPACE_START, len);
}

/*
 * readUserMemory
 */
int DS1339::readUserMemory(byte *buffer, int offset, int len) {
  return readBytes(buffer, offset + DS1339_USERSPACE_START, len);
}

/*
 * Read the control register
 */
byte DS1339::readControlRegister() {
  byte ctrlReg = 0;

  if(readBytes(&ctrlReg, DS1339_CONTROL_REG, 1) != 1) {
    // Something went wrong so return -1
    return 0xff;
  }

  return ctrlReg;
}

/*
 * startSquareWave
 */
bool DS1339::startSquareWave(SquareWaveRate rate) {
  byte ctrlReg = DS1339_CTRL_SQWE | rate;

  if(writeBytes(&ctrlReg, DS1339_CONTROL_REG, 1) != 1) {
    return false;
  }

  squareWave = true;

  return true;
}

/*
 * stopSquarWave
 */
bool DS1339::stopSquareWave(bool out) {
  byte ctrlReg = (out) ? 0x80 : 0x00;

  if(writeBytes(&ctrlReg, DS1339_CONTROL_REG, 1) != 1) {
    return false;
  }

  squareWave = false;

  return true;
}

/*
 * readBytes
 */
int DS1339::readBytes(byte *buffer, int offset, int len) {
  int bytes_to_read = 0;
  int bytes_read = 0;

  if(offset < DS1339_MEMORY) {
    if(powerMgmtPin >= 0) {
      // Turn on the power
      digitalWrite(powerMgmtPin, HIGH);
    }
    while((offset < DS1339_MEMORY) && (bytes_read < len)) {

      // Let the DS1339 we want talk to it
      Wire.beginTransmission(DS1339_I2C_ID);
      Wire.send(offset);
      Wire.endTransmission();

      // Request the number of bytes we want to read and read the bytes
      // I2C will only allow 0x1F in one sweap to be read
      bytes_to_read = (len - bytes_read) > I2C_MAX_BYTES_TRANSFER ? I2C_MAX_BYTES_TRANSFER : len - bytes_read;
      bytes_to_read = ((offset + bytes_to_read) <= DS1339_MEMORY) ? bytes_to_read : DS1339_MEMORY - offset;
      Wire.requestFrom(DS1339_I2C_ID, bytes_to_read);
      for(int i = 0; i < bytes_to_read; i++) {
        buffer[bytes_read++] = (unsigned char)Wire.receive();
      }
      offset += bytes_to_read;
    }
    if(powerMgmtPin >= 0) {
      // We are done, we can send to clock back into low power mode
      digitalWrite(powerMgmtPin, LOW);
    }
  }

  return bytes_read;
}

/*
 * writeBytes
 */
int DS1339::writeBytes(const byte *buffer, int offset, int len) {
  int bytes_to_write = 0;
  int bytes_written = 0;

  if(offset < DS1339_MEMORY) {
    if(powerMgmtPin >= 0) {
      // Give the clock power so that we can talk to it.
      digitalWrite(powerMgmtPin, HIGH);
      // TODO, check if we need to delay a little here for the clock to get out of low power mode
    }

    while((offset < DS1339_MEMORY) && (bytes_written < len)) {

      bytes_to_write = ((offset + len) <= DS1339_MEMORY) ? len : DS1339_MEMORY - offset;

      // Let the DS1339 we want talk to it
      Wire.beginTransmission(DS1339_I2C_ID);
      Wire.send(offset);

      // and send the bytes we want to write
      // I2C will only allow 0x1F in one sweap to be read
      bytes_to_write = (len - bytes_written) > I2C_MAX_BYTES_TRANSFER ? I2C_MAX_BYTES_TRANSFER : len - bytes_written;
      bytes_to_write = ((offset + bytes_to_write) <= DS1339_MEMORY) ? bytes_to_write : DS1339_MEMORY - offset;
      for(int i = 0; i < bytes_to_write; i++) {
        Wire.send(buffer[bytes_written++]);
      }
      offset += bytes_to_write;
      Wire.endTransmission();
    }
    Wire.endTransmission();
    if(powerMgmtPin >= 0) {
      // We are done, we can send to clock back into low power mode
      digitalWrite(powerMgmtPin, LOW);
    }
  }

  return bytes_written;
}

//
// PRIVATE FUNCTIONS
//

// Convert Decimal to Binary Coded Decimal (BCD)
uint8_t DS1339::dec2bcd(uint8_t num)
{
  return ((num/10 * 16) + (num % 10));
}

// Convert Binary Coded Decimal (BCD) to Decimal
uint8_t DS1339::bcd2dec(uint8_t num)
{
  return ((num/16 * 10) + (num % 16));
}


