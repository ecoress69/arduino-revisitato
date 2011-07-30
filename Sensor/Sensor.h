/*
 * Sensor.h
 *
 *  Created on: Jul 26, 2011
 *      Author: john
 */

#ifndef SENSOR_H_
#define SENSOR_H_

#include <WProgram.h>

// TODO: try this a template and see the implications

class Sensor {
  public:
    enum Error { NO_ERROR = 0,
                 FUNCTION_NOT_SUPPORTED,
                 SENSOR_INITIALIZATION_ERROR,
                 SENSOR_NOT_PRESENT,
                 UNKNOWN_CONFIG,
                 SYNC_TIMEOUT,
                 DATA_TIMEOUT,
                 TOO_QUICK,
                 CHECKSUM_ERROR,
                 BUS_ERROR,
               };

    virtual Sensor::Error initialize() = 0;
    virtual Sensor::Error reset() = 0;

    Sensor::Error readSensor(int config = 0) { return readSensor(millis()); };
    virtual Sensor::Error readSensor(unsigned long timeInMillis, int config = 0) = 0;

    /**
     * Returns the last integer value read by this sensor. In general, this is the last value obtained through
     * <code>readSensor</code>. However, it is up to the implementation of the specific sensor to decide if a <code>readSensor</code>
     * is triggered by this method.
     * <p>
     * The interpretation of the value depends on the specific sensor. Some sensors might not
     * support an integer value or the integer value has a different interpretation than the float value. For example,
     * if the sensor returns a voltage representing its value through the ATmega ADC, the integer value could represent
     * the value of the ADC, i.e. is in the range of 0...1023. However, the float value on the other hand might represent
     * the interpretation of that integer value with respect to what the sensor is measuring.
     * Please refer to the documentation of the specific sensor.
     * <p>
     * Implementations of the <code>Sensor</code> interface will have to implement this method.
     *
     * param[in] config a configuration value that can be passed to the sensor. The value is sensor specific and might
     *                  be ignored.
     *
     * @return the integer value of this sensor. In general, this is the last value that was obtained
     *             with <code>readSensor</code>. However, it is up to the implementation of the specific sensor.
     *
     * Sensor::readSensor(long, int), Sensor::getFloatValue(int), Sensor::getByteValue(int);
     */
    virtual int getIntegerValue(int config = 0) = 0;

    /**
     * Returns the last byte value read by the sensor of this adapter. In general, this is the last value obtained through
     * <code>readSensor</code>. However, it is up to the implementation of the specific sensor to decide if a <code>readSensor</code>
     * is triggered by this method.
     * <p>
     * The interpretation of the value depends on the specific sensor. Some sensors might not
     * support an integer value or the integer value has a different interpretation than the float value. For example,
     * if the sensor returns a voltage representing its value through the ATmega ADC, the integer value could represent
     * the value of the ADC, i.e. is in the range of 0...1023. However, the float value on the other hand might represent
     * the interpretation of that integer value with respect to what the sensor is measuring.
     * Please refer to the documentation of the specific sensor.
     * <p>
     * Implementations of the <code>Sensor</code> interface will have to implement this method.
     *
     * param[in] config a configuration value that can be passed to the sensor. The value is sensor specific and might
     *                  be ignored.
     *
     * @return the integer value of this sensor. In general, this is the last value that was obtained
     *             with <code>readSensor</code>. However, it is up to the implementation of the specific sensor.
     *
     * Sensor::readSensor(long, int), Sensor::getFloatValue(int), Sensor::getIntegerValue(int);
     */
    virtual byte getByteValue(int config = 0) = 0;

    /**
     * Returns the last float value read by this sensor. In general, this is the last value obtained through
     * <code>readSensor</code>. However, it is up to the implementation of the specific sensor to decide if a <code>readSensor</code>
     * is triggered by this method.
     * <p>
     * The interpretation of the value depends on the specific sensor. Some sensors might not
     * support an float value or the float value has a different interpretation than the integer value. For example,
     * if the sensor returns a voltage representing its value through the ATmega ADC, the integer value could represent
     * the value of the ADC, i.e. is in the range of 0...1023. However, the float value on the other hand might represent
     * the interpretation of that integer value with respect to what the sensor is measuring.
     * Please refer to the documentation of the specific sensor.
      * <p>
     * Implementations of the <code>Sensor</code> interface will have to implement this method.
     *
     * param[in] config a configuration value that can be passed to the sensor. The value is sensor specific and might
     *                  be ignored.
     *
     * @return the integer value of this sensor. In general, this is the last value that was obtained
     *             with <code>readSensor</code>. However, it is up to the implementation of the specific sensor.
     *
     * Sensor::readSensor(long, int), Sensor::getIntegerValue(int), Sensor::getByteValue(int);
     */
    virtual float getFloatValue(int config = 0)  = 0;

    // Sampling, only relevant if there is energy management involved, i.e. sensor is turn on and off
    virtual Sensor::Error beginSampling() = 0;
    virtual Sensor::Error endSampling() = 0;
    virtual bool isSampling() = 0;


    void clockReset() { clockReset(millis()); };
    virtual void clockReset(unsigned long timeInMillis) = 0;

    virtual unsigned long lastReadTime() = 0;
};

class SensorImpl: public Sensor  {
  public:
    SensorImpl();

    virtual Sensor::Error initialize();
    virtual Sensor::Error reset();

    Sensor::Error readSensor(unsigned long timeInMillis, int config = 0) { return readSensorImpl(timeInMillis); };

    // Default implementations
    virtual int getIntegerValue(int config = 0) { return 0; };
    virtual byte getByteValue(int config = 0) { return 0; };
    virtual float getFloatValue(int config = 0) { return 0; };

    // Sampling, only relevant if there is energy management involved, i.e. sensor is turn on and off
    Sensor::Error beginSampling();
    Sensor::Error endSampling();
    bool isSampling() { return _sampling; };


    void clockReset(unsigned long timeInMillis) {_lastReadTime = timeInMillis; };

    unsigned long lastReadTime() { return _lastReadTime; };

  protected:
    // Default implementations will return no error, so code can be generalized
    virtual Sensor::Error beginSamplingImpl() { return NO_ERROR; };
    virtual Sensor::Error endSamplingImpl() { return NO_ERROR; };
    virtual Sensor::Error readSensorImpl(unsigned long timeInMillis, int config = 0) { return FUNCTION_NOT_SUPPORTED; };

  private:
    unsigned long _lastReadTime;
    bool _sampling;
};

class SensorAdapter:public Sensor {
  public:
    SensorAdapter(Sensor *sensor, int config);

    Sensor::Error initialize() { return _sensor->initialize(); };
    Sensor::Error reset()  { return _sensor->initialize(); };

    Sensor::Error readSensor(unsigned long timeInMillis, int config = 0) {
      return _sensor->readSensor(timeInMillis, config ? config :_config);
    };

    /**
     * Returns the last integer value read by the sensor of this adapter. This method simply calls
     * <code>getIntegerValue(config)</code> using either the config of this adapter if zero is passed in
     * for config or if a different value than zero is passed it will be passed instead of the config value
     * of this adapter. This allows to override the config value that is passed to the adapter. However,
     * it is suggested to not use the feature, as the purpose of the adapter is handle the config parameter.
     * In that way, it is possible to have a single sensor object that provide multiple values, but treat
     * the single sensor as it were multiple. One example are sensors that provide humidity and temperature
     * measurements. There could be a single TempHumiditySensor, but two adapter, one for humidity and one for
     * temperature with very little overhead.
     *
     * param[in] config can be used to override the configuration value of this adapter. However, it is suggested
     *                  to not override the configuration value of this adapter.
     * @return the integer value of the sensor of this adapter. In general, this is the last value that was obtained
     *             with <code>readSensor</code>. However, it is up to the implementation of the specific sensor.
     *
     * @see SensorAdapter::readSensor(long, int)
     * @see Sensor::readSensor(long, int)
     * @see Sensor::getIntegerValue(int)
     */
    int getIntegerValue(int config = 0) { return _sensor->getIntegerValue(config ? config :_config); };

    /**
     * Returns the last byte value read by the sensor of this adapter. This method simply calls
     * <code>getByteValue(config)</code> using either the config of this adapter if zero is passed in
     * for config or if a different value than zero is passed it will be passed instead of the config value
     * of this adapter. This allows to override the config value that is passed to the adapter. However,
     * it is suggested to not use the feature, as the purpose of the adapter is handle the config parameter.
     * In that way, it is possible to have a single sensor object that provide multiple values, but treat
     * the single sensor as it were multiple. One example are sensors that provide humidity and temperature
     * measurements. There could be a single TempHumiditySensor, but two adapter, one for humidity and one for
     * temperature with very little overhead.
     *
     * param[in] config can be used to override the configuration value of this adapter. However, it is suggested
     *                  to not override the configuration value of this adapter.
     * @return the byte value of the sensor of this adapter. In general, this is the last value that was obtained
     *             with <code>readSensor</code>. However, it is up to the implementation of the specific sensor.
     *
     * @see SensorAdapter::readSensor(long, int)
     * @see Sensor::readSensor(long, int)
     * @see Sensor::getByteValue(int)
     */
    byte getByteValue(int config = 0) { return _sensor->getByteValue(config ? config :_config); };

    /**
     * Returns the last float value read by the sensor of this adapter. This method simply calls
     * <code>getFloatValue(config)</code> using either the config of this adapter if zero is passed in
     * for config or if a different value than zero is passed it will be passed instead of the config value
     * of this adapter. This allows to override the config value that is passed to the adapter. However,
     * it is suggested to not use the feature, as the purpose of the adapter is handle the config parameter.
     * In that way, it is possible to have a single sensor object that provide multiple values, but treat
     * the single sensor as it were multiple. One example are sensors that provide humidity and temperature
     * measurements. There could be a single TempHumiditySensor, but two adapter, one for humidity and one for
     * temperature with very little overhead.
     *
     * param[in] config can be used to override the configuration value of this adapter. However, it is suggested
     *                  to not override the configuration value of this adapter.
     * @return the float value of the sensor of this adapter. In general, this is the last value that was obtained
     *             with <code>readSensor</code>. However, it is up to the implementation of the specific sensor.
     *
     * @see SensorAdapter::readSensor(long, int)
     * @see Sensor::readSensor(long, int)
     * @see Sensor::getFloatValue(int)
     */
    float getFloatValue(int config = 0) { return _sensor->getFloatValue(config ? config :_config); };

    // Sampling, only relevant if there is energy management involved, i.e. sensor is turn on and off
    Sensor::Error beginSampling() { return _sensor->beginSampling(); };
    Sensor::Error endSampling() { return _sensor->endSampling(); };
    bool isSampling() { return _sensor->isSampling(); };


    void clockReset(unsigned long timeInMillis) {  _sensor->clockReset(); };
    unsigned long lastReadTime() { return _sensor->lastReadTime(); };

  private:
    Sensor* _sensor;
    int _config;
};

#endif /* SENSOR_H_ */
