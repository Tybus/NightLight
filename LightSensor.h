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
#define LS_POL (uint16_t) 0x0008

#define CONFIGURATION_REGISTER (uint8_t) 0x01
#define LOWLIMIT_REGISTER (uint8_t) 0x02
#define HIGHLIMIT_REGISTER (uint8_t) 0x03

#define DFLT_CONFIGURATION_HIGH 0b11001100
#define DFLT_CONFIGURATION_LOW 0b00000000

#define LS_ONOFF_TH (float) 6.0f

#define HIGH_LIMIT (bool) 1
#define LOW_LIMIT (bool) 0

#define LATCH_FIELD_1 (uint16_t) 0x0004
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
    //! Configure the Low Limit in the Latched Window Interrupt Mode
    void setLowLimit(float i_fLowLimit);
    //! Configure the Hight Limit in the Latched Window Interrupt Mode
    void setHighLimit(float i_fHighLimit);
    //! Configure the Comparation value in the Transparent Hystheresis Comparation Mode.
    bool setHysthValue(float i_fHysthValue);
    //! Dtor, TBD.
    virtual ~LightSensor();
    //! Configures Interrupts. using P4.6 of the Board
    void interruptConfigure(void);
    //! Used for reading a register from the light sensor.
    uint16_t readRegister(uint8_t i_u8Register);
    //! Handles the interrupt from the light sensor.
    static void PORT4_IRQHandler(void);

private:
    //! Used to set a certain limit.
    void setLimit(bool i_bWhichLimit, float i_fLimit);
    //! I2C object used for the Light Sensor Comunications
    static I2C m_I2CLightSensor;
    //! Configuration Register Backup.
    static uint16_t m_u16ConfigurationRegister;

};

#endif /* LIGHTSENSOR_H_ */
