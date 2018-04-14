/*
 * I2C.cpp
 *
 *  Created on: Apr 11, 2018
 *      Author: David Martínez Gacía.
 *      Email: ing.davidmartinezgarcia@gmail.com
 */

#include <I2C.h>
#include "msp.h"


//Some member static Variables

uint8_t * I2C::m_su8ByteValues = (uint8_t*)malloc(sizeof(uint8_t)*MAX_BYTES); // HIghly doubt it
size_t I2C::m_sstByteArraySize = 0;
uint32_t I2C::m_sstIterator = 0;
uint8_t I2C::m_u8RetryCount =0;
uint8_t * I2C::m_spRxBuff = NULL;
size_t I2C::m_sstRxBuffSize = 0;
bool * I2C::m_sbReadDone = NULL;

//Member variables and functions
bool I2C::m_bActive = 0;
I2C::I2C(uint8_t iu8SlaveAddress){
    if(iu8SlaveAddress > 127)
       //Wont let me throw exceptions. GG
        // throw std::invalid_argument("Slave Address Most be lower than 128")
        printf("Exception Occurred: invalid_argument: Slave Address most be lower than 128");
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
    if(m_sstIterator == m_sstByteArraySize){//First check the conditions in which you can start sending.
        UCB1I2CSA |= m_u16SlaveAddress;//Set the slave addres.
        UCB1CTLW0 |= UCTR; // Selecting Transmitter mode.
        UCB1CTLW0 |= UCTXSTT;//Starts the transmision.

        //Store some member variables
        m_su8ByteValues = i_pByteValues;
        m_sstByteArraySize = i_stSize;
        m_sstIterator = 0;
        m_u8RetryCount = 0;
        o_bCorrectlySent = 1;
    }
    return o_bCorrectlySent;
}
bool I2C::read(uint8_t * i_pRxBuff, size_t i_stReadAmmount, bool * i_pReadDone){
    bool o_bCorrectlyRead = 0;
    //Conditions to Start Reading
    if(1){
        UCB1I2CSA |= m_u16SlaveAddress; //Sets the Slave Address
        UCB1CTLW0 &= ~UCTR; //States Receiver Mode.
        UCB1CTLW0 |= UCTXSTT; // Starts Receiving

        //Store Some variables
        m_sstRxBuffSize = i_stReadAmmount;
        m_sbReadDone = i_pReadDone;
        m_spRxBuff = i_pRxBuff;
        m_sstIterator = 0;
        o_bCorrectlyRead = 1;
        m_u8RetryCount = 0;
    }
    return o_bCorrectlyRead;

}
I2C::~I2C()
{
    // TODO Auto-generated destructor stub
}
extern "C" {
void EUSCIA1_IRQHandler(void){
    __disable_irq();
    if(UCB1CTLW0 & UCTR){ //If it is in Send Mode
        switch(UCB1IFG){
        case(UCTXIFG0):
            //Clear the flag.
            UCB1IFG |= STARTIF;
            UCB1IFG &= ~STARTIF;
            //Set the value
            if(I2C::m_sstIterator < I2C::m_sstByteArraySize) {
                UCB1TXBUF = I2C::m_su8ByteValues[I2C::m_sstIterator]; //Starts sending subsequetial values
               I2C::m_sstIterator++;
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
                UCB1TXBUF = I2C::m_su8ByteValues[I2C::m_sstIterator - 1]; //restarts the transmition where it ended.
                I2C::m_u8RetryCount++;
            }
            else //Send a Stop
                UCB1CTLW0 |= UCTXSTP;
            break;
        }
    }
    else{ // if it is in Receive Mode
        switch(UCB1IFG){
        case(STARTIF):
            //Clear the flag.
            UCB1IFG |= STARTIF;
            UCB1IFG &= ~STARTIF;
            break;
        case(UCSTPIFG):
            if(I2C::m_sstIterator == I2C::m_sstRxBuffSize -1){//Iterator at the end of the buffer
                UCB1CTLW0 |= UCTXSTP; //Send Stop
                *I2C::m_sbReadDone = 1; //Announce the caller that I2C was read.
            }
            I2C::m_spRxBuff[I2C::m_sstIterator] = (uint8_t) UCB1RXBUF; //This should Totally Count as as UCRB1RXBUF Read.
            I2C::m_sstIterator++;
            break;
        case(UCNACKIFG):
            //Clear the flag.
            UCB1IFG |= UCNACKIFG;
            UCB1IFG &= ~UCNACKIFG;
            if(I2C::m_u8RetryCount < MAX_RETRY){
                UCB1CTLW0 &= ~UCTR; // Selecting Receiving mode.
                UCB1CTLW0 |= UCTXSTT;//Starts the Reception.
                I2C::m_sstIterator --; //restarts the reception where it ended.
                I2C::m_u8RetryCount++;
            }
            else //Send a Stop
                UCB1CTLW0 |= UCTXSTP;
            break;
        }
    }

    __enable_irq();
}
}
