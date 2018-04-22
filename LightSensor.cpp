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
void LightSensor::interruptConfigure(){
    P4SEL0 &= ~BIT6; // Selects P4.6 as GPIO
    P4SEL1 &= ~BIT6;

    P4DIR &= ~BIT6; //Selects the P4.6 as a input pin.
    P4REN &= ~BIT6; //Dissables the pull up/down resistors for P4.6


    P4IE |= BIT6; //Enables interrupts for the pin.
    if(P4IN & BIT6) //Start condition is high/
        P4IES |= BIT6; //Will interrupt in high to low transitions.
    else
        P4IES &= ~BIT6; //Will interrupt in low to high transitions.

    // Lets accept interrupts.
    NVIC_SetPriority(PORT4_IRQn,1);
    NVIC_EnableIRQ(PORT4_IRQn);

}

float LightSensor::read(){
    uint8_t l_u8RegisterAddress = 0;
    uint8_t l_aResultRegister[2];
    uint16_t l_u16FractionalResult;
    uint16_t l_u16PowTwo;
    uint8_t l_u8Exponent;
    bool * l_pReadDone = (bool *) malloc(sizeof(bool)); //Finally found the culprit
    float l_fLSBSize, o_fLux;
    while(!m_I2CLightSensor.send(&l_u8RegisterAddress,1)); //Send the Register Address to be read.
    while(!m_I2CLightSensor.read(l_aResultRegister, 2, l_pReadDone)); //Wait till you can send the read.

    while(!*l_pReadDone){ // Wait till its done reading
       // m_I2CLightSensor.send(&l_u8RegisterAddress,1);
       // m_I2CLightSensor.read(l_aResultRegister, 2, l_pReadDone); //Wait till you can send the read.
    }

    //Converting the value found in the register to a uint16t
    l_u8Exponent = l_aResultRegister[0] >> 4; //Get the real value of the exponent.
    //printf("Exponent %d \n", l_u8Exponent);
    l_u16FractionalResult = ((uint16_t) l_aResultRegister[0]) << 8; //Shift the Fractional Result bits.
    l_u16FractionalResult &= 0x0FFF; //Get Rid of the Exponent Values
    l_u16FractionalResult |= (uint16_t) l_aResultRegister[1];
    //printf("Fractional Result %d \n", l_u16FractionalResult);
    l_u16PowTwo = 1 << l_u8Exponent;
    //printf("2^EXP %d \n", l_u16PowTwo);
    l_fLSBSize = 0.01*l_u16PowTwo;
    o_fLux = l_fLSBSize*l_u16FractionalResult;
    free(l_pReadDone); //Free the memory!.
    return o_fLux;
}
LightSensor::~LightSensor()
{
    // TODO Auto-generated destructor stub
}
void LightSensor::setLowLimit(float i_fLowLimit){
    LightSensor::setLimit(LOW_LIMIT, i_fLowLimit);
}
void LightSensor::setHighLimit(float i_fHighLimit){
    LightSensor::setLimit(HIGH_LIMIT, i_fHighLimit);
}
void LightSensor::setLimit(bool i_bWhichLimit, float i_fLimit){
    uint16_t l_u16XE, l_u16TX, l_u16RegValue;
    uint8_t l_u8RegWriteValue[3];
    float l_fTX;
    if(i_fLimit <= 1*40.95)
        l_u16XE = 0;
    else if(i_fLimit <= 2*40.95)
        l_u16XE = 1;
    else if(i_fLimit <= 4*81.90)
        l_u16XE = 2;
    else if(i_fLimit <= 8*40.95)
        l_u16XE = 3;
    else if(i_fLimit <= 16*40.95)
        l_u16XE = 4;
    else if(i_fLimit <= 32*40.95)
        l_u16XE = 5;
    else if(i_fLimit <= 64*40.95)
        l_u16XE = 6;
    else if(i_fLimit <= 128*40.95)
        l_u16XE = 7;
    else if(i_fLimit <= 256*40.95)
        l_u16XE = 8;
    else if(i_fLimit <= 512*40.95)
        l_u16XE = 9;
    else if(i_fLimit <= 1024*40.95)
        l_u16XE = 10;
    else if(i_fLimit <= 2048*40.95)
        l_u16XE = 11;
    else if(i_fLimit <= 4096*40.95)
        l_u16XE = 12;
    else if(i_fLimit <= 8192*40.95)
        l_u16XE = 13;
    else if(i_fLimit <= 16384*40.95)
        l_u16XE = 14;
    else if(i_fLimit <= 32768*40.95)
        l_u16XE = 15;
    else //Item out of bounds.
        l_u16XE = 0;

    l_fTX = i_fLimit /(0.01*(1 << l_u16XE)); //The TX will be set as lux_max/ 2^XE
    l_u16TX = (uint16_t) l_fTX;

    l_u16RegValue = l_u16TX | l_u16XE;
    l_u8RegWriteValue[1] = (uint8_t) (l_u16RegValue >> 8);
    l_u8RegWriteValue[2] = (uint8_t) (l_u16RegValue & 0x00FF);
    switch(i_bWhichLimit){
    case(LOW_LIMIT):
        l_u8RegWriteValue[0] = LOWLIMIT_REGISTER;
    case(HIGH_LIMIT):
        l_u8RegWriteValue[0] = HIGHLIMIT_REGISTER;
    }
    while(!m_I2CLightSensor.send(l_u8RegWriteValue,3)){ //Make sure you start writing
    }
    LightSensor::interruptConfigure();
}
extern "C"{
void PORT4_IRQHandler(void){ //This may overwrite an existing IRQ
    __disable_irq();
    if(P4IFG & BIT6){ //Interrupt from P4.6
        if(P4IES & BIT6){ //High to low Transition
            P4IES &=~BIT6; //Now wait for a Low to High Transition
            //Do something about the transition.
        }
        else { //Low to high transition
            P4IES |= BIT6; //Now wait for a High to low transition Transition
            //Do something about the transition
        }
        P4IFG &= ~BIT6; //Clear the interrupt flag
    }
}
}
