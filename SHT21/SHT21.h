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


#ifndef SHT_21_H
#define SHT_21_H

#include <WProgram.h>
#include <Sensor.h>

#define RES_MASK 0x7E


class SHT21 : public SensorImpl {
	public:
    enum Address {
      eSHT21Address = 0x40,
    };

    enum Commands {
      eTempHoldCmd        = 0xE3,
      eRHumidityHoldCmd  = 0xE5,
      eTempNoHoldCmd      = 0xF3,
      eRHumidityNoHoldCmd = 0xF5,
      eWriteUserRegister  = 0xE6,
      eReadUserRegister = 0xE7,
      eSoftReset      = 0xFE
    };

    enum Resolution {
      RES_12_14 = 0x00,
      RES_10_13 = 0x80,
      RES_08_12 = 0x01,
      RES_11_11 = 0x81
    };

    enum Mode {
      TemperatureF = 0x00,
      TemperatureC = 0x01,
      Humidity = 0x02
    };

		SHT21();

		Sensor::Error initialize();
		Sensor::Error reset();

    /**
     * Returns the last integer value read from the ADC.
     *
     * @param[IN] config is ignored
     *
     * @return the last integer value read from the ADC.
     */
    int getIntegerValue(int config);

    /**
     * Returns the last integer value read from the ADC divided by 4. This
     * method should not be used. Instead, <code>getIntegerValue</code> should
     * be used to obtain the ADC read out.
     *
     * @param[IN] config is ignored
     *
     * @return the last integer value read from the ADC.
     */
    byte getByteValue(int config);

    /**
     * Returns the current that was measured in the last reading. The assumption is
     * that 0A is equivalent to 2.5V. In general, the sensor will require calibration based
     * on the specific setup and use of <code>setCalibration</code>.
     *
     * @param[IN] config is ignored
     *
     * @return the current in A that was measured in the last reading.
     */
    float getFloatValue(int config);

    /**
     * Gets the current humidity from the sensor.
     *
     * @return float - The relative humidity in %RH
     */
		float getHumidity(void) {
		  return getFloatValue(Humidity);
		};

		/**
		 * getTemperature(bool celsius)
		 *
		 * @param celsius if set to true the temperature is returned
		 *                in degrees Celsius; otherwise in Fahrenheit
		 *
		 * @return the temperature in Celsius or Fahrenheit
		 */

		float getTemperature(bool celsius) {
      return getFloatValue(celsius? TemperatureC : TemperatureF);
		};


    //--- Methods specific to the SHT21 sensor
		uint8_t getUserRegister(void);
		uint8_t setResolution(SHT21::Resolution res);
		
		void printDebug();

	protected:
	  Sensor::Error readSensorImpl(unsigned long timeInMillis, int config = 0);


	private:
    uint16_t readDelay;
    float calculateHumidity(uint16_t analogHumValue);
    float calculateTemperature(uint16_t analogTempValue);
    uint16_t readSensorValue(uint8_t command);
    uint8_t readUserRegister();
    void writeUserRegister(uint8_t value);
    void writeReset();

    float _humidity;
    float _temperature;
};

#endif
