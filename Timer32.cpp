/*
 * Timer32.cpp
 *
 *  Created on: Apr 22, 2018
 *      Author: david
 */

#include <Timer32.h>

bool Timer32::m_bTimer1Available = 1;
bool Timer32::m_bTimer2Available = 1;
Timer32::pin Timer32::m_pinBounceTIMER32_1 = {.m_pEvenPort = NULL, .m_pOddPort = NULL,
                                              .m_u16Bit = 0};
Timer32::pin Timer32::m_pinBounceTIMER32_2 = {.m_pEvenPort = NULL, .m_pOddPort = NULL,
                                              .m_u16Bit = 0};

//MCLK comes from the DCOCLK and its value is: 3Mhz

bool Timer32::suppressBounce(DIO_PORT_Even_Interruptable_Type * i_pPort,
                             uint16_t i_u16Bit, float i_fBounceMillis){
    uint32_t l_u32Prescale;
    uint32_t l_u32CountValue;
    i_pPort->IE &= ~i_u16Bit;//Disable interrupts from the port.
    bool o_bTimerAssigned = 0;
    if(m_bTimer1Available){
        m_bTimer1Available = 0;

        m_pinBounceTIMER32_1.m_pEvenPort = i_pPort;
        m_pinBounceTIMER32_1.m_pOddPort = NULL;
        m_pinBounceTIMER32_1.m_u16Bit = i_u16Bit;

        l_u32Prescale = calculatePrescale(i_fBounceMillis);
        l_u32CountValue = calculateValue(i_fBounceMillis, l_u32Prescale);
        TIMER32_1->LOAD = l_u32CountValue;
        TIMER32_1->CONTROL = TIMER32_CONTROL_ENABLE | TIMER32_CONTROL_IE | l_u32Prescale |
                TIMER32_CONTROL_SIZE | TIMER32_CONTROL_ONESHOT;//Ask this to professor.
        o_bTimerAssigned = 1;
        NVIC_SetPriority(T32_INT1_IRQn,1);
        NVIC_EnableIRQ(T32_INT1_IRQn);
    }
    else if(m_bTimer2Available){
        m_bTimer2Available = 0;

        m_pinBounceTIMER32_2.m_pEvenPort = i_pPort;
        m_pinBounceTIMER32_2.m_pOddPort = NULL;
        m_pinBounceTIMER32_2.m_u16Bit = i_u16Bit;

        l_u32Prescale = calculatePrescale(i_fBounceMillis);
        l_u32CountValue = calculateValue(i_fBounceMillis, l_u32Prescale);
        TIMER32_2->LOAD = l_u32CountValue;
        TIMER32_2->CONTROL = TIMER32_CONTROL_ENABLE | TIMER32_CONTROL_IE | l_u32Prescale |
                TIMER32_CONTROL_SIZE | TIMER32_CONTROL_ONESHOT;//Ask this to professor.
        o_bTimerAssigned = 1;
        NVIC_SetPriority(T32_INT2_IRQn,1);
        NVIC_EnableIRQ(T32_INT2_IRQn);
    }
    else {
        o_bTimerAssigned = 0;
        // No timer available.
    }
    return o_bTimerAssigned;

}
bool Timer32::suppressBounce(DIO_PORT_Odd_Interruptable_Type * i_pPort,
                             uint16_t i_u16Bit, float i_fBounceMillis){
    uint32_t l_u32Prescale;
    uint32_t l_u32CountValue;
    i_pPort->IE &= ~i_u16Bit;//Disable interrupts from the port.
    bool o_bTimerAssigned = 0;
    if(m_bTimer1Available){
        m_bTimer1Available = 0;

        //Save values so the interrupt know where to search.
        m_pinBounceTIMER32_1.m_pEvenPort = NULL;
        m_pinBounceTIMER32_1.m_pOddPort = i_pPort;
        m_pinBounceTIMER32_1.m_u16Bit = i_u16Bit;

        l_u32Prescale = calculatePrescale(i_fBounceMillis);
        l_u32CountValue = calculateValue(i_fBounceMillis, l_u32Prescale);
        TIMER32_1->LOAD = l_u32CountValue;
        TIMER32_1->CONTROL = TIMER32_CONTROL_ENABLE | TIMER32_CONTROL_IE | l_u32Prescale |
                TIMER32_CONTROL_SIZE | TIMER32_CONTROL_ONESHOT;//Ask this to professor.
        o_bTimerAssigned = 1;



        NVIC_SetPriority(T32_INT1_IRQn,1);
        NVIC_EnableIRQ(T32_INT1_IRQn);


    }
    else if(m_bTimer2Available){
        m_bTimer2Available = 0;

        //Save values so the interrupt know where to search.
        m_pinBounceTIMER32_2.m_pEvenPort = NULL;
        m_pinBounceTIMER32_2.m_pOddPort = i_pPort;
        m_pinBounceTIMER32_2.m_u16Bit = i_u16Bit;

        l_u32Prescale = calculatePrescale(i_fBounceMillis);
        l_u32CountValue = calculateValue(i_fBounceMillis, l_u32Prescale);
        TIMER32_2->LOAD = l_u32CountValue;
        TIMER32_2->CONTROL = TIMER32_CONTROL_ENABLE | TIMER32_CONTROL_IE | l_u32Prescale |
                TIMER32_CONTROL_SIZE | TIMER32_CONTROL_ONESHOT;//Ask this to professor.
        o_bTimerAssigned = 1;
        NVIC_SetPriority(T32_INT2_IRQn,1);
        NVIC_EnableIRQ(T32_INT2_IRQn);
    }
    else {
        o_bTimerAssigned = 0;
        // No timer available.
    }
    return o_bTimerAssigned;

}
uint8_t Timer32::calculatePrescale(float i_fBounceMillis){ //Asumming a 32 Bit counter
    uint8_t o_u8Prescale;

    if(i_fBounceMillis <= 1431655.765)
        o_u8Prescale = TIMER32_CONTROL_PRESCALE_0;
    else if(i_fBounceMillis <= 22906492.24)
        o_u8Prescale = TIMER32_CONTROL_PRESCALE_1;
    else if(i_fBounceMillis <= 366503875.8)
        o_u8Prescale = TIMER32_CONTROL_PRESCALE_2; //This will overflow in a byte.
    else
        o_u8Prescale = 0; //This cant happen. Put not enable condition.

    return o_u8Prescale;
}
uint32_t Timer32::calculateValue(float i_fBounceMillis, uint8_t i_u8Prescale){
    uint32_t o_u32Value;
    o_u32Value = (uint32_t) i_fBounceMillis/( 1000 *( 1<< i_u8Prescale)/MCLK_F); //milliseconds/clock in milliseconds
    return o_u32Value;
}

/*
Timer32::Timer32()
{
    // TODO Auto-generated constructor stub

}

Timer32::~Timer32()
{
    // TODO Auto-generated destructor stub
}

*/

