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
#include "DS1307RTC.h"

#define DS1307_I2C_ID 0x68

// Memory related constants
#define DS1307_MEMORY           0x40
#define DS1307_USERSPACE_START  0x09
#define DS1307_CONTROL_REG      0x07
#define DS1307_TIME_ZONE_REG    0x08

// Bit masks
#define DS1307_CLOCKHALT  0x80
#define DS1307_CTRL_OUT   0x80
#define DS1307_CTRL_SQWE  0x10
#define DS1307_CTRL_RS    0x03

#define DS1307_BCD_LO     0x0f
#define DS1307_BCD_HI     0xf0

#define DS1307_SEC_MASK   0x7f

#define DS1307_HR_MASK    0x3f
#define DS1307_AM_PM      0x30
#define DS1307_12_24      0x40


#define DS1307_DOW_LOW    0x07
#define DS1307_DATE_HI    0x30
#define DS1307_MTH_HI     0x30


/*
 * Constructor
 */
DS1307RTC::DS1307RTC() {

  //Wire.begin();


}

bool DS1307RTC::initialize(bool withWire) {
  byte buffer;

  if(withWire) {
    Wire.begin();
  }

  // Check the state of the clock
  if(readBytes(&buffer, 0, 1) == 1) {
    running = !(buffer & DS1307_CLOCKHALT);
  }
  else {
    return false;
  }

  // TODO: Set squareWave to correct value
  // Change the clock to 24 hr mode
  // TODO: Add that, but for now this is good enough.

  return true;
}
/*
 * start - will always force the clock to stop, no matter what the cached parameter
 * <code>running</code> says. Allows to get the clock back in a defined status.
 */
bool DS1307RTC::start() {
  byte seconds;

  //  Read the seconds byte
  if(readBytes(&seconds, 0, 1) != 1) {
    return false;
  }

  // Clear the CH bit in the register
  seconds &= DS1307_SEC_MASK;

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
bool DS1307RTC::stop() {
  byte seconds;

  //  Read the seconds byte
  if(readBytes(&seconds, 0, 1) != 1) {
    return false;
  }

  // Set the CH bit in the register
  seconds |= DS1307_CLOCKHALT;

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
bool DS1307RTC::isRunning() {
  return running;
}


/*
 * getTime
 */
time_t DS1307RTC::getTime()   // Aquire data from buffer and convert to time_t
{
  byte buffer[7];
  tmElements_t tm;

   if(readBytes(buffer, 0, 7) == 7) {
     tm.Second = bcd2dec(buffer[0] & DS1307_SEC_MASK);
     tm.Minute = bcd2dec(buffer[1]);
     tm.Hour =  bcd2dec(buffer[2] & DS1307_HR_MASK);  // mask assumes 24hr clock
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
bool  DS1307RTC::setTime(time_t t) {
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
#ifdef DEBUG
  Serial.print("Setting Time:[");
  Serial.print(buffer[0], HEX);
  Serial.print("] ");
  Serial.print(tm.Second, DEC);
  Serial.print(", [");
  Serial.print(buffer[1], HEX);
  Serial.print("] ");
  Serial.print(tm.Minute, DEC);
  Serial.print(", [");
  Serial.print(buffer[2], HEX);
  Serial.print("] ");
  Serial.print(tm.Hour, DEC);
  Serial.print(", [");
  Serial.print(buffer[3], HEX);
  Serial.print("] ");
  Serial.print(tm.Wday, DEC);
  Serial.print(", [");
  Serial.print(buffer[4], HEX);
  Serial.print("] ");
  Serial.print(tm.Day, DEC);
  Serial.print(", [");
  Serial.print(buffer[5], HEX);
  Serial.print("] ");
  Serial.print(tm.Month, DEC);
  Serial.print(", [");
  Serial.print(buffer[6], HEX);
  Serial.print("] ");
  Serial.print(tm.Year, DEC);
  Serial.println(" ...");


#endif

  buffer[0] |= DS1307_CLOCKHALT;  // stop the clock
  if(writeBytes(buffer, 0, 7) !=7) {
    return false;
  }

  if(isRunning()) {
    buffer[0] &= DS1307_SEC_MASK;
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
int8_t DS1307RTC::getTimeZone() {
  byte buffer;

  if(readBytes(&buffer, DS1307_TIME_ZONE_REG, 1) != 1) {
    return -128;
  }

  return buffer - 12;
}

/*
 * setTimeZone - to avoid negative values and conversion problems between signed and unsigned,
 * the time zone is offset by 12, i.e. it is always positive from 0 to 24 when written.
 */
bool DS1307RTC::setTimeZone(int8_t tz) {
  byte buffer = tz + 12;

  if(writeBytes(&buffer, DS1307_TIME_ZONE_REG, 1) != 1) {
    return false;
  }

  return true;
}

/*
 * writeUserMemory - the user memory starts at byte 0x09 (0x08 is used for the timezone)
 */
int DS1307RTC::writeUserMemory(byte *data, int offset, int len) {
  return writeBytes(data, offset + DS1307_USERSPACE_START, len);
}

/*
 * readUserMemory
 */
int DS1307RTC::readUserMemory(byte *buffer, int offset, int len) {
  return readBytes(buffer, offset + DS1307_USERSPACE_START, len);
}

/*
 * startSquareWave
 */
bool DS1307RTC::startSquareWave(DS1307SquareWaveRate rate) {
  byte ctrlReg = DS1307_CTRL_SQWE | rate;

  if(writeBytes(&ctrlReg, DS1307_CONTROL_REG, 1) != 1) {
    return false;
  }

  squareWave = true;

  return true;
}

/*
 * stopSquarWave
 */
bool DS1307RTC::stopSquareWave(bool out) {
  byte ctrlReg = (out) ? 0x00 : 0x80;

  if(writeBytes(&ctrlReg, DS1307_CONTROL_REG, 1) != 1) {
    return false;
  }

  squareWave = false;

  return true;
}

/*
 * readBytes
 */
int DS1307RTC::readBytes(byte *buffer, int offset, int len) {
  int bytes_to_read = 0;

  if(offset < DS1307_MEMORY) {
    bytes_to_read = ((offset + len) <= DS1307_MEMORY) ? len : DS1307_MEMORY - offset;

    // Let the DS1307 we want talk to it
    Wire.beginTransmission(DS1307_I2C_ID);
    Wire.send(0x00);
    Wire.endTransmission();

    // Request the number of bytes we want to read and read the bytes
    // TODO: Test this. There might be a bug that restricts the number of bytes that can be
    //       read in on shot.
    Wire.requestFrom(DS1307_I2C_ID, bytes_to_read);
    for(int i = 0; i < bytes_to_read; i++) {
      buffer[i] = (unsigned char)Wire.receive();
    }
  }

  return bytes_to_read;
}

/*
 * writeBytes
 */
int DS1307RTC::writeBytes(const byte *buffer, int offset, int len) {
  int bytes_to_write = 0;

  if(offset < DS1307_MEMORY) {
    bytes_to_write = ((offset + len) <= DS1307_MEMORY) ? len : DS1307_MEMORY - offset;

    // Let the DS1307 we want talk to it
    Wire.beginTransmission(DS1307_I2C_ID);
    Wire.send(0x00);

    // and send the bytes we want to write
    // TODO: Test this for the whole clock. There might be a bug that restricts the number of bytes
    //       that can be written in one shot.
    for(int i = 0; i < bytes_to_write; i++) {
      Wire.send(buffer[i]);
    }

    Wire.endTransmission();
  }

  return bytes_to_write;
}

//
// PRIVATE FUNCTIONS
//

// Convert Decimal to Binary Coded Decimal (BCD)
uint8_t DS1307RTC::dec2bcd(uint8_t num)
{
  return ((num/10 * 16) + (num % 10));
}

// Convert Binary Coded Decimal (BCD) to Decimal
uint8_t DS1307RTC::bcd2dec(uint8_t num)
{
  return ((num/16 * 10) + (num % 16));
}

DS1307RTC RTC = DS1307RTC(); // create an instance for the user

