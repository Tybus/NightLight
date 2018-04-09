/*
 * ADC.cpp
 *
 *  Created on: Apr 8, 2018
 *      Author: david
 */

#include <ADC.h>
/*!
 * Converts 5 bit binary values into the 1 bit per state (OneHot encoding, in order to use them with most of the
 * registers.
 */
uint32_t bintoOneHot(uint8_t bit){
    uint32_t one_hot;
    one_hot = 1<< bit;
    return one_hot;
}
/*
 * Map used to convert ports of the ADC channel (p4.3 for instance) into its analog value (A10 in this case).
 */
uint32_t getAnalogChannel(DIO_PORT_Even_Interruptable_Type* i_pPort, uint16_t i_u16Bit){\
    uint32_t o_u32AnalogChannel;
    if(i_pPort == P4){
        switch(i_u16Bit){
        case BIT7: o_u32AnalogChannel = ADC14_MCTLN_INCH_6;
        break;
        case BIT6: o_u32AnalogChannel = ADC14_MCTLN_INCH_7;
        break;
        case BIT5: o_u32AnalogChannel = ADC14_MCTLN_INCH_8;
        break;
        case BIT4: o_u32AnalogChannel = ADC14_MCTLN_INCH_9;
        break;
        case BIT3: o_u32AnalogChannel = ADC14_MCTLN_INCH_10;
        break;
        case BIT2: o_u32AnalogChannel = ADC14_MCTLN_INCH_11;
        break;
        default: printf("Invalid Analog Port");
        break;
        }
    }
    else{
        printf("Invalid Analog Port");
        o_u32AnalogChannel = 0;
    }
    return o_u32AnalogChannel;
}
vector<DIO_PORT_Even_Interruptable_Type *> ADC::m_vpStaticPorts;
vector<uint16_t> ADC::m_vu16StaticBit;
uint16_t ADC::m_u32CTL0;
uint16_t ADC::m_u32CTL1;
uint8_t ADC::m_u8AnalogChannels;
bool ADC::m_bStaticConfigured = 0;


ADC::ADC(){
    m_configured = 0;
}
ADC::ADC(DIO_PORT_Even_Interruptable_Type * i_pPort, uint16_t i_u16Bit){

    //Statics updates
    m_vpStaticPorts.push_back(i_pPort);
    m_vu16StaticBit.push_back(i_u16Bit);

    //Class Memebers Declarations
    m_configured = m_bStaticConfigured;
    m_vpPorts.push_back(i_pPort);
    m_vu16Bit.push_back(i_u16Bit);
}

ADC::ADC(vector<DIO_PORT_Even_Interruptable_Type*> i_vPort, vector<uint16_t> i_vBit){
    //Statics updates
    m_vpStaticPorts.insert(m_vpStaticPorts.end(), i_vPort.begin(), i_vPort.end());
    m_vu16StaticBit.insert(m_vu16StaticBit.end(), i_vBit.begin(), i_vBit.end());

    //Class Members Declarations
    m_configured = m_bStaticConfigured;
    m_vpPorts = i_vPort;
    m_vu16Bit = i_vBit;
}

ADC::ADC(DIO_PORT_Even_Interruptable_Type* i_pPort, uint16_t i_u16Bit, uint32_t i_u32CTL0, uint32_t i_u32CTL1){
    //Statics Updates
    m_vpStaticPorts.push_back(i_pPort);
    m_vu16StaticBit.push_back(i_u16Bit);
    m_u32CTL0 = i_u32CTL0;
    m_u32CTL1 = i_u32CTL1;
    m_bStaticConfigured = 1;

    //Class Members Declarations
    m_vpPorts.push_back(i_pPort);
    m_vu16Bit.push_back(i_u16Bit);
    m_configured = 1;
    this->setup(1);
}
ADC::ADC(vector<DIO_PORT_Even_Interruptable_Type*> i_vPort, vector<uint16_t> i_vBit, uint32_t i_u32CTL0, uint32_t i_u32CTL1){
    //Statics updates
    m_vpStaticPorts.insert(m_vpStaticPorts.end(), i_vPort.begin(), i_vPort.end());
    m_vu16StaticBit.insert(m_vu16StaticBit.end(), i_vBit.begin(), i_vBit.end());
    m_u32CTL0 = i_u32CTL0;
    m_u32CTL1 = i_u32CTL1;
    m_bStaticConfigured = 1;

    //Class Members Declarations
    m_vpPorts = i_vPort;
    m_vu16Bit = i_vBit;
    m_configured = 1;
    this->setup(1); //Currently  accepting interrupts.
}
ADC::ADC(const ADC& i_ADC){
    m_vpPorts = i_ADC.m_vpPorts;
    m_vu16Bit = i_ADC.m_vu16Bit;
    m_configured = i_ADC.m_configured;
}
ADC::~ADC(){
//tbd
}
bool ADC::quick_setup(bool i_bInterruptEnable){
    ADC14->CTL0 &= ~ADC14_CTL0_ENC; //Make sure you disable ENC before modifying certain variables.

    for(int i = 0; i<m_vpPorts.size(); i ++){ //Configure the selected ports as ADC ports.
        m_vpPorts[i]->SEL0 |= m_vu16Bit[i];
        m_vpPorts[i]->SEL1 |= m_vu16Bit[i];
        ADC14->MCTL[m_u8AnalogChannels] = getAnalogChannel(m_vpPorts[i],m_vu16Bit[i]);
        m_u8AnalogChannels++;
        if(i_bInterruptEnable){ // Get interrupts running (if specified)
            ADC14->IER0 |= bintoOneHot(m_u8AnalogChannels);
            NVIC_SetPriority(ADC14_IRQn,1);
            NVIC_EnableIRQ(ADC14_IRQn);
        }
    }
    //CTL0 Config
    ADC14->CTL0 = ADC14_CTL0_SHP | ADC14_CTL0_SHT0_0 | ADC14_CTL0_ON |ADC14_CTL0_MSC; //Sample and hold settings, turn on ADC and multiple samples
    if(m_vpPorts.size()>1) ADC14->CTL0 |= ADC14_CTL0_CONSEQ_3; // If more than one, consecrutive samples
    else ADC14->CTL0 |= ADC14_CTL0_CONSEQ_2;
    //CTL1 Config
    ADC14->CTL1 |= ADC14_CTL1_RES_3; //Best resolution
    ADC14->CTL1 |= ADC14_CTL1_DF; // Signed intergers.
    //ENC ADC converter
    ADC14->CTL0 |= ADC14_CTL0_ENC; //Enable Convertions from the ADC.

    //Update member variables
    m_configured = 1;
    m_bStaticConfigured = 1;
    m_u32CTL0 = ADC14->CTL0;
    m_u32CTL1 = ADC14->CTL1;
    return 1;
}
bool ADC::setup(bool i_bInterruptEnable){
    if(m_configured){
        ADC14->CTL0 = m_u32CTL0 &~ ADC14_CTL0_ENC; //Disable ENC in order to modify
        ADC14->CTL1 = m_u32CTL1;

        for(int i = 0; i<m_vpPorts.size(); i ++){ //Configure the ports as ADC ports
            m_vpPorts[i]->SEL0 |= m_vu16Bit[i];
            m_vpPorts[i]->SEL1 |= m_vu16Bit[i];
            ADC14->MCTL[m_u8AnalogChannels] = getAnalogChannel(m_vpPorts[i],m_vu16Bit[i]); //Assing where the value will be.
            m_u8AnalogChannels++;
            if(i_bInterruptEnable){ //enable interrupts.
                ADC14->IER0 |= bintoOneHot(m_u8AnalogChannels);
                NVIC_SetPriority(ADC14_IRQn,1);
                NVIC_EnableIRQ(ADC14_IRQn);
            }
        }
        ADC14->CTL0 |=  ADC14_CTL0_ENC;
        return 1;
    }
    else{
        printf("Go to quick Setup");
        return this->quick_setup(i_bInterruptEnable);
    }
}
bool ADC::setup(uint32_t i_u32CTL0, uint32_t i_u32CTL1, bool i_bInterruptEnable){
    m_configured =1;
    printf("Go to normal Setup");
    return this->setup(i_bInterruptEnable);
}
int16_t ADC::read(DIO_PORT_Even_Interruptable_Type * i_pPort, uint16_t i_u16pBit){
    uint32_t l_u32AnalogChannel = getAnalogChannel(i_pPort,i_u16pBit);
    int16_t result = 0;
    for(uint8_t i = 0; i<32 ; i++){
        if(ADC14->MCTL[i] == l_u32AnalogChannel){ //return the correct ADC value.
            result = ADC14->MEM[i];//Extend the sign. currently only valid for 14 bit conversions.
            result >>= 2;
            if(result & 0x2000){
                result |= 0xC000;
            }
            break;
        }
    }
    return result;
}
bool ADC::init(void){
    bool o_bRetval = 0;
    if(m_configured){
        ADC14->CTL0 |= ADC14_CTL0_SC; //Set the start convertion bit.
        o_bRetval = 1;
    }
    return o_bRetval;

}



