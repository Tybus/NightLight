/*
 * I2C.cpp
 *
 *  Created on: Apr 11, 2018
 *      Author: david
 */

#include <I2C.h>
#include "msp.h"


//Some member static Variables
uint8_t * I2C::m_su8ByteValues = (uint8_t*)malloc(sizeof(uint8_t)*MAX_BYTES); // HIghly doubt it
size_t I2C::m_sstByteArraySize = 0;
uint32_t I2C::m_su32Iterator = 0;
uint8_t I2C::m_u8RetryCount =0;
//member variables and functions
bool I2C::m_bActive = 0;
I2C::I2C(uint8_t iu8SlaveAddress){
    if(iu8SlaveAddress > 127)
       //Wont let me throw exceptions. GG
        // throw std::invalid_argument("Slave Address Most be lower than 128")
        printf("Exception Occurred: invalid_argument: Slave Address most be lowe than 128");
    else {
        m_u16SlaveAddress = (uint16_t) iu8SlaveAddress;
        if(!m_bActive){
            m_bActive = 1;
            P6->SEL0 |= BIT4 | BIT5; //Configures the ports as SDA and SCL
            P6->SEL1 &= ~BIT4 & ~BIT5;

            UCB1CTLW0 = UCSWRST | UCSYNC; //Holds USCI in reset state
            //Do I have to wait here?
            UCB1CTLW0 = UCSWRST | UCSYNC| UCMST | UCMODE_3 | UCSSEL_3;

            UCB1CTLW1 = 0; //Leaving it to default.

            UCB1CTLW0 &= ~UCSWRST; //Unholds the USCI

            UCB1I2CSA = m_u16SlaveAddress;
            UCB1IE = UCSTTIE; //Interrupts when Starts.
            // Lets accept interrupts.
            NVIC_SetPriority(EUSCIA1_IRQn,1);
            NVIC_EnableIRQ(EUSCIA1_IRQn);
         }
    }

}

bool I2C::send(uint8_t *i_pByteValues, size_t i_stSize){
    bool o_bCorrectlySent = 0;
    if(m_su32Iterator == m_sstByteArraySize){//First check the conditions in which you can start sending.
        UCB1I2CSA |= m_u16SlaveAddress;//Set the slave addres.
        UCB1CTLW0 |= UCTR; // Selecting Transmitter mode.
        UCB1CTLW0 |= UCTXSTT;//Starts the transmision.

        //Store some member variables
        m_su8ByteValues = i_pByteValues;
        m_sstByteArraySize = i_stSize;
        m_su32Iterator = 0;
        o_bCorrectlySent = 1;
    }
    return o_bCorrectlySent;
}
I2C::~I2C()
{
    // TODO Auto-generated destructor stub
}
extern "C" {
void EUSCIA1_IRQHandler(void){
    __disable_irq();
    switch(UCB1IFG){
    case(STARTIF):
        //Clear the flag.
        UCB1IFG |= STARTIF;
        UCB1IFG &= ~STARTIF;
        //Set the value
        if(I2C::m_su32Iterator < I2C::m_sstByteArraySize) {
            UCB1TXBUF = I2C::m_su8ByteValues[I2C::m_su32Iterator]; //Starts sending subsequetial values
            I2C::m_su32Iterator++;
        }
        else //Send a Stop
            UCB1CTLW0 |= UCTXSTP;
        break;
    case(UCNACKIFG):
        //Clear the flag.
        UCB1IFG |= UCNACKIFG;
        UCB1IFG &= ~UCNACKIFG;
        if(I2C::m_u8RetryCount < MAX_RETRY){
            UCB1CTLW0 |= UCTR; // Selecting Transmitter mode.
            UCB1CTLW0 |= UCTXSTT;//Starts the transmision.
            UCB1TXBUF = I2C::m_su8ByteValues[I2C::m_su32Iterator - 1]; //restarts the transmition where it ended.
            I2C::m_u8RetryCount++;
        }
        else //Send a Stop
            UCB1CTLW0 |= UCTXSTP;
        break;
    }
    __enable_irq();
}
}
