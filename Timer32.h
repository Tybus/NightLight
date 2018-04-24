/*
 * Timer32.h
 *
 *  Created on: Apr 22, 2018
 *      Author: david
 */

#ifndef TIMER32_H_
#define TIMER32_H_

#include "msp.h"
#include <stdint.h>
#include <cstddef>

#define DCOCLK 3000000
#define MCLK DCOCLK
#define MCLK_F (float) MCLK
#define MCLK_MILLIS (float) 1000/(MCLK_F)

//! 32 Bit Timer Powered by the MCLK of the MSP432 Launchpad.
class Timer32{
    /*!Structure used to encapsule pins, I should consider moving it outside the timer 32
    * but its the first time I'm using it.
    */
    struct pin{
        DIO_PORT_Even_Interruptable_Type * m_pEvenPort;
        DIO_PORT_Odd_Interruptable_Type * m_pOddPort;
        uint16_t m_u16Bit;
    };
public:
    /*! used  to suppress the bounces of the buttons declared in the BoosterPack. Can work with any other
    * Interrupt capable button. (Even Ports)
    */
    bool suppressBounce(DIO_PORT_Even_Interruptable_Type * i_pPort, uint16_t i_u16Bit, float i_fBounceMillis); //For the P3.5(J4.32), P5.1 (J4.33), P4.1 (J1.5).
    /*! used  to suppress the bounces of the buttons declared in the BoosterPack. Can work with any other
    * Interrupt capable button. (Odd Ports)
    */
    bool suppressBounce(DIO_PORT_Odd_Interruptable_Type * i_pPort, uint16_t i_u16Bit, float i_fBounceMillis);
    /*!
     * Declares the pins from which the Bounce is being suppresed (TIMER32_1)
     */
    static pin m_pinBounceTIMER32_1;
    /*!
     * Declares the pins from which the Bounce is being suppresed (TIMER32_1)
     */
    static pin m_pinBounceTIMER32_2;
    /*!
     * Decides whether a timer is available or not. (Timer1)
     */
    static bool m_bTimer1Available;
    /*!
     * Decides wheter a timer is available or not. (Timer2)
     */
    static bool m_bTimer2Available;

private:
    //!Calculates the prescale of the Timer32Configuration.
    uint8_t calculatePrescale(float i_fBounceMillis);
    //!Calculates the value which will be sent to the Timer32 to start decreasing.
    uint32_t calculateValue(float i_fBounceMillis, uint8_t i_u8Prescale);
};

#endif /* TIMER32_H_ */
