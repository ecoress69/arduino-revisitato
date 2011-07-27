/*
  LibHumidity - A Humidity Library for Arduino.

  Supported Sensor modules:
    SHT21-Breakout Module - http://www.moderndevice.com/products/sht21-humidity-sensor

  Created by Christopher Ladden at Modern Device on December 2009.
  modified by Paul Badger March 2010

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

#include <WProgram.h>
#include <Wire.h>
//#include <wiring.h>
#include "SHT21.h"

/******************************************************************************
 * Constructors
 ******************************************************************************/

/**********************************************************
 * Initialize the sensor based on the specified type.
 **********************************************************/
SHT21::SHT21() {
    Wire.begin();
    readDelay = 100;
}

/******************************************************************************
 * Global Functions
 ******************************************************************************/
/**
 * Read the sensor
 */

int SHT21::readSensor(unsigned long timeInMillis) {

  _lastReadTime = timeInMillis;
  // TODO: make sure the sensor doesn't get read too often

  _humidity = calculateHumidity(readSensorImpl(eRHumidityHoldCmd));
  _temperature = calculateTemperature(readSensorImpl(eTempHoldCmd));

  return 0;
}
/**********************************************************
 * GetHumidity
 *  Gets the current humidity from the sensor.
 *
 * @return float - The relative humidity in %RH
 **********************************************************/
float SHT21::getHumidity(void) {
    return _humidity;
}

/**********************************************************
 * getTemperature(bool celsius)
 * 
 * @param celsius if set to true the temperature is returned
 *                in degrees Celsius; otherwise in Fahrenheit
 *
 * @return the temperature in Celsius or Fahrenheit
 **********************************************************/
float SHT21::getTemperature(bool celsius) {
  if(!celsius) {
    return _temperature * 1.8 + 32;
  }
	return _temperature;
}

uint8_t SHT21::getUserRegister(void) {
	return readUserRegister();
}

uint8_t SHT21::setResolution(SENSOR_RESOLUTION_T res) {
	uint8_t reg;
	
	reg = getUserRegister();
	reg = (reg & RES_MASK) | res;
		
	switch(res) {
		case RES_12_14:
			readDelay = 85;
			break;
		case RES_10_13:
			readDelay = 43;
			break;
		case RES_08_12:
			readDelay = 29;
			break;
		case RES_11_11:
			readDelay = 11;
			break;
	}
	
	writeUserRegister(reg);
	
	return reg;
}

void SHT21::reset() {
	writeReset();
}

/******************************************************************************
 * Private Functions
 ******************************************************************************/

uint16_t SHT21::readSensorImpl(uint8_t command) {
	uint16_t result;

	Wire.beginTransmission(eSHT21Address);   //begin
	Wire.send(command);                      //send the pointer location
	delay(readDelay);
	Wire.endTransmission();                  //end

	Wire.requestFrom(eSHT21Address, 3);
	while(Wire.available() < 3) {
		delay(5);
	}

		//Store the result
	result = ((Wire.receive()) << 8);
	result += Wire.receive();
	result &= ~0x0003;   // clear two low bits (status bits)
	return result;
}

uint8_t SHT21::readUserRegister() {
	uint8_t result = 0xc5;
	
	Wire.beginTransmission(eSHT21Address);
	Wire.send(eReadUserRegister);
	Wire.endTransmission();
	
	Wire.requestFrom(eSHT21Address, 1);
	while(Wire.available() < 1) {
		delay(5);
	}
	result = Wire.receive();
}

void SHT21::writeUserRegister(uint8_t value) {
	Wire.beginTransmission(eSHT21Address);
	Wire.send(eWriteUserRegister);
	Wire.send(value);
	Wire.endTransmission();
}

void SHT21::writeReset() {
	Wire.beginTransmission(eSHT21Address);
	Wire.send(eSoftReset);
	Wire.endTransmission();
}

float SHT21::calculateTemperature(uint16_t analogTempValue) {
  //T= -46.85 + 175.72 * ST/2^16
  return  (((175.72/65536.0) * (float)analogTempValue) - 46.85);
}

float SHT21::calculateHumidity(uint16_t analogHumValue) {
  //-- calculate relative humidity [%RH] --
  // RH= -6 + 125 * SRH/2^16
  return -6.0 + 125.0 / 65536.0 * analogHumValue;
}


void SHT21::clockReset(unsigned long timeInMillis)
{
  _lastReadTime = timeInMillis;
}

void SHT21::printDebug() {
	Serial.print(" userRegister:");
  Serial.print(getUserRegister(), BIN);
}
