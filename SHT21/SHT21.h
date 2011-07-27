/*
LibHumidity - A Humidity Library for Arduino.

Supported Sensor modules:
SHT21-Breakout Module - http://www.moderndevice.com/products/sht21-humidity-sensor

Created by Christopher Ladden at Modern Device on December 2009.

This library is free software; you can redistribute it and/or
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
*/


#ifndef LIB_HUMIDITY_H
#define LIB_HUMIDITY_H

#include <WProgram.h>


typedef enum {
	eSHT21Address = 0x40,
} HUM_SENSOR_T;

typedef enum {
	eTempHoldCmd        = 0xE3,
	eRHumidityHoldCmd  = 0xE5,
	eTempNoHoldCmd      = 0xF3,
	eRHumidityNoHoldCmd = 0xF5,
	eWriteUserRegister  = 0xE6,
	eReadUserRegister	= 0xE7,
	eSoftReset			= 0xFE
} HUM_MEASUREMENT_CMD_T;

typedef enum {
	RES_MASK = 0x7E,
	RES_12_14 = 0x00,
	RES_10_13 = 0x80,
	RES_08_12 = 0x01,
	RES_11_11 = 0x81
} SENSOR_RESOLUTION_T;

class SHT21 {
	public:
		SHT21();

		// TODO: pull out the common methods into a super class. Think about a "sensor" class/template
		//--- General method shared with other temperature/humidity sensors
    int readSensor() { return readSensor(millis()); };
    int readSensor(unsigned long timeInMillis);
		float getHumidity(void);
		float getTemperature(bool celsius);
    void clockReset() { clockReset(millis()); };
    void clockReset(unsigned long timeInMillis);

    //--- Methods specific to the SHT21 sensor
		uint8_t getUserRegister(void);
		uint8_t setResolution(SENSOR_RESOLUTION_T res);
		void reset();
		
		void printDebug();

	private:
    uint16_t readDelay;
    float calculateHumidity(uint16_t analogHumValue);
    float calculateTemperature(uint16_t analogTempValue);
    uint16_t readSensorImpl(uint8_t command);
    uint8_t readUserRegister();
    void writeUserRegister(uint8_t value);
    void writeReset();

    float _humidity;
    float _temperature;
    unsigned long _lastReadTime;
};

#endif
