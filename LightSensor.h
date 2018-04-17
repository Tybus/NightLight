/*
 * LightSensor.h
 *
 *  Created on: Apr 16, 2018
 *      Author: david
 */

#ifndef LIGHTSENSOR_H_
#define LIGHTSENSOR_H_

#include <I2C.h>

#define CT_100MS (bool) 0
#define CT_800MS (bool) 1
#define SINGLE_SHOT (bool) 0
#define CONT_CONVERSION (bool) 1
#define TRANSP_HYSTH (bool) 0
#define LATCHED_WINDOW (bool) 1
#define ACTIVE_LOW (bool) 0
#define ACTIVE_HIGH (bool) 1

#define CONFIGURATION_REGISTER (uint8_t) 0x01

#define DFLT_CONFIGURATION_HIGH 0b11001100
#define DFLT_CONFIGURATION_LOW 0b00000000

struct nibble {
    uint8_t value:4;
};


//! Light Sensor use class for the MSP432 LauchPad using BoosterPack MKII
class LightSensor{
public:
    //! Will create an I2C object of the control of the Light Sensor int he BoosterPack.
    LightSensor();
    //! Create a I2C object with the selected parameters.
    LightSensor(nibble i_nRange, bool i_bConversionTime, bool i_bOperationMode,
                             bool i_bWindowMode, bool i_bINTMode);
    //! Returns the read from the Light Sensor.
    float read(void);
    //! Reconfigurate the already existante LightSensor.
    void reconfigure(nibble i_nRange, bool i_bConversionTime, bool i_bOperationMode,
                             bool i_bWindowMode, bool i_bINTMode);
    //! Dtor, TBD.
    virtual ~LightSensor();

private:
    static I2C m_I2CLightSensor;
    static uint16_t m_u16ConfigurationRegister;
};

#endif /* LIGHTSENSOR_H_ */
