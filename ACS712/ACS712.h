/*
 * ACS712.h
 *
 *  Created on: Jul 26, 2011
 *      Author: john
 */

#ifndef ACS712_H_
#define ACS712_H_

#include <Sensor.h>
#include <WProgram.h>

class ACS712: public SensorImpl {
  public:
    ACS712();

    Sensor::Error initialize(int analogPin, int vccPin = -1);
    Sensor::Error initialize() {
      return initialize(0, -1); // Just using PIN as default
    };
    Sensor::Error reset();

    void setCalibration(int offset, float scale = 1.0) {
      _offset = 512 + offset;
      // TODO: this should be generalized for the different version of the chip
      _scale = 0.185 * scale * 1024.0 / 5.0;
    };

    /**
     * Returns the last integer value read from the ADC.
     *
     * @param[IN] config is ignored
     *
     * @return the last integer value read from the ADC.
     */
    int getIntegerValue(int config = 0);
    /**
     * Returns the last integer value read from the ADC divided by 4. This
     * method should not be used. Instead, <code>getIntegerValue</code> should
     * be used to obtain the ADC read out.
     *
     * @param[IN] config is ignored
     *
     * @return the last integer value read from the ADC.
     */
    byte getByteValue(int config = 0);

    /**
     * Returns the current that was measured in the last reading. The assumption is
     * that 0A is equivalent to 2.5V. In general, the sensor will require calibration based
     * on the specific setup and use of <code>setCalibration</code>.
     *
     * @param[IN] config is ignored
     *
     * @return the current in A that was measured in the last reading.
     */
    float getFloatValue(int config = 0);

  protected:
    Sensor::Error beginSamplingImpl();
    Sensor::Error endSamplingImpl();
    Sensor::Error readSensorImpl(unsigned long timeInMillis, int config = 0);

  private:

    void on();
    void off();

    int _value;
    int _offset;
    float _scale;
    int _vccPin;
    int _analogPin;
    bool _on;
};

#endif /* ACS712_H_ */
