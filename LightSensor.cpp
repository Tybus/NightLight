/*
 * LightSensor.cpp
 *
 *  Created on: Apr 16, 2018
 *      Author: david
 */

#include <LightSensor.h>
I2C LightSensor::m_I2CLightSensor = I2C(LIGHT_SENSOR);
uint16_t LightSensor::m_u16ConfigurationRegister = 0b1100110000000000;
LightSensor::LightSensor(){
    m_I2CLightSensor = I2C(LIGHT_SENSOR);
    m_u16ConfigurationRegister = 0b1100110000000000;
    uint8_t l_aConfigurationMessage[3] = {CONFIGURATION_REGISTER, DFLT_CONFIGURATION_HIGH,
                                          DFLT_CONFIGURATION_LOW};
    while(!m_I2CLightSensor.send(l_aConfigurationMessage,3));
}

LightSensor::LightSensor(nibble i_nRange, bool i_bConversionTime, bool i_bOperationMode,
                         bool i_bWindowMode, bool i_bINTMode){
    uint16_t l_u16CastedRange, l_u16CastedConversionTime,l_u16CastedOperationMode,
             l_u16WindowMode, l_u16INTMode;
    uint8_t l_aConfigurationMessage[3];
    m_I2CLightSensor = I2C(LIGHT_SENSOR);
    //Creates a Configuration Register.


    l_u16CastedRange = ((uint16_t) i_nRange.value) << 12;
    l_u16CastedConversionTime = ((uint16_t) i_bConversionTime ) << 11;

    switch(i_bOperationMode){
    case(SINGLE_SHOT):
        l_u16CastedOperationMode = 0b01 << 9;
        break;
    case(CONT_CONVERSION):
        l_u16CastedOperationMode = 0b10 << 9;
        break;
    }

    l_u16WindowMode = ((uint16_t) i_bWindowMode) >> 4;
    l_u16INTMode = ((uint16_t) i_bINTMode) >> 3;
    m_u16ConfigurationRegister = l_u16CastedRange | l_u16CastedConversionTime|
            l_u16CastedOperationMode| l_u16WindowMode | l_u16INTMode ;
    //Set the message.
    l_aConfigurationMessage[0] = CONFIGURATION_REGISTER; //Should Have used 8 bit registers from the begining
    l_aConfigurationMessage[1] = (uint8_t) (m_u16ConfigurationRegister >> 8) ;
    l_aConfigurationMessage[2] = (uint8_t) (m_u16ConfigurationRegister & 0x00FF);

    while(!m_I2CLightSensor.send(l_aConfigurationMessage,3)){ //Make sure you start writing
    }
}
float LightSensor::read(){
    uint8_t l_aResultRegister[2];
    uint16_t l_u16FractionalResult;
    bool * l_pReadDone = (bool *) malloc(sizeof(bool));
    float l_fLSBSize, o_fLux,l_fExponent;

    while(!m_I2CLightSensor.read(l_aResultRegister, 2, l_pReadDone)){ //Wait till you can send the read.
    }

    while(!*l_pReadDone){ // Wait till its done reading
    }
    //Converting the value found in the register to a uint16t
    l_u16FractionalResult = (((uint16_t) l_aResultRegister[0])   << 8) | ((uint16_t) l_aResultRegister[1]);
    l_fExponent = l_aResultRegister[0] >> 4;
    l_fLSBSize = 0.01*pow(2,l_fExponent);//(2^l_fExponent);
    o_fLux = l_fLSBSize*l_u16FractionalResult;
    return o_fLux;
}
LightSensor::~LightSensor()
{
    // TODO Auto-generated destructor stub
}
