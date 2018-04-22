/*
 * I2C.cpp
 *
 *  Created on: Apr 11, 2018
 *      Author: David Martínez Gacía.
 *      Email: ing.davidmartinezgarcia@gmail.com
 */

#include <I2C.h>
#include "msp.h"
//#include <math.h>

//Some member static Variables
bool I2C::m_bActive = 0;
uint8_t * I2C::m_su8ByteValues = (uint8_t*)malloc(sizeof(uint8_t)*MAX_BYTES); // HIghly doubt it
size_t I2C::m_sstByteArraySize = 0;
uint32_t I2C::m_sstIterator = 0;
uint8_t I2C::m_u8RetryCount =0;
uint8_t * I2C::m_spRxBuff = NULL;
size_t I2C::m_sstRxBuffSize = 0;
bool * I2C::m_sbReadDone = NULL;
bool I2C::m_sbBusReady = 1;
bool * I2C::m_sbFailedTransmition = NULL;
bool * I2C::m_sbDone = NULL;


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
            UCB1CTLW0 = UCSWRST | UCSYNC| UCMST | UCMODE_3 | UCSSEL_3; //Using SMCLK.
            UCB1BRW = 30; //Predivide the 3MHz Clk in Order to make a 100 Khz Clock
            UCB1CTLW1 = UCASTP_2; //Automatic Stop please.

            UCB1CTLW0 &= ~UCSWRST; //Unholds the USCI

            UCB1I2CSA = m_u16SlaveAddress;
            UCB1IE = UCNACKIE | UCTXIE0 | UCRXIE0 | UCBCNTIE; //Interrupts when Starts.
            // Lets accept interrupts.
            NVIC_SetPriority(EUSCIB1_IRQn,1);
            NVIC_EnableIRQ(EUSCIB1_IRQn);
         }
    }

}

bool I2C::send(uint8_t *i_pByteValues, size_t i_stSize, bool * i_pSendDone, bool * i_pSendFail){
    bool o_bCorrectlySent = 0;
    if(m_sbBusReady){//First check the conditions in which you can start sending.
        //Store some member variables
        m_su8ByteValues = i_pByteValues;
        m_sstByteArraySize = i_stSize;
        m_sstIterator = 0;
        m_u8RetryCount = 0;
        m_sbDone = i_pSendDone;
        m_sbFailedTransmition = i_pSendFail;

        m_sbBusReady = 0;
        *m_sbFailedTransmition = 0;
        *m_sbDone = 0;


        //Do Hardware Stuff
        UCB1CTLW0 = UCSWRST | UCSYNC; //Holds USCI in reset state
        //Do I have to wait here?
        UCB1CTLW0 = UCSWRST | UCSYNC| UCMST | UCMODE_3 | UCSSEL_3; //Using SMCLK.
        UCB1BRW = 30; //Predivide the 3MHz Clk in Order to make a 100 Khz Clock
        UCB1CTLW1 = UCASTP_2; //Automatic Stop please.
        UCB1TBCNT = (uint16_t) i_stSize; //Set the automatic stop limit.
        UCB1CTLW0 &= ~UCSWRST; //Unholds the USCI

        UCB1IE = UCNACKIE | UCTXIE0 | UCRXIE0 | UCBCNTIE;

        UCB1I2CSA = m_u16SlaveAddress;//Set the slave addres.
        UCB1CTLW0 |= UCTR; // Selecting Transmitter mode.
        UCB1CTLW0 |= UCTXSTT;//Starts the transmision.

        o_bCorrectlySent = 1;

    }
    return o_bCorrectlySent;
}
bool I2C::read(uint8_t * i_pRxBuff, size_t i_stReadAmmount, bool * i_pReadDone, bool * i_pReadFail){
    bool o_bCorrectlyRead = 0;
    //Conditions to Start Reading
    if(m_sbBusReady){
        //Store Some variables
        m_sstRxBuffSize = i_stReadAmmount;
        m_sbReadDone = i_pReadDone;
        m_spRxBuff = i_pRxBuff;
        m_sstIterator = 0;
        m_u8RetryCount = 0;
        m_sbDone = i_pReadDone;
        m_sbFailedTransmition = i_pReadFail;

        *m_sbDone = 0;
        *m_sbFailedTransmition =0;
        m_sbBusReady = 0;

        //Do Hardware Stuff.

        UCB1CTLW0 = UCSWRST | UCSYNC; //Holds USCI in reset state
        //Do I have to wait here?
        UCB1CTLW0 = UCSWRST | UCSYNC| UCMST | UCMODE_3 | UCSSEL_3; //Using SMCLK.
        UCB1BRW = 30; //Predivide the 3MHz Clk in Order to make a 100 Khz Clock
        UCB1CTLW1 = UCASTP_2; //Automatic Stop please.
        UCB1TBCNT = (uint16_t) i_stReadAmmount;
        UCB1CTLW0 &= ~UCSWRST; //Unholds the USCI

        UCB1IE = UCNACKIE | UCTXIE0 | UCRXIE0 | UCBCNTIE;


        UCB1I2CSA |= m_u16SlaveAddress; //Sets the Slave Address
        UCB1CTLW0 &= ~UCTR; //States Receiver Mode.
        UCB1CTLW0 |= UCTXSTT; // Starts Receiving

        o_bCorrectlyRead = 1;
    }
    return o_bCorrectlyRead;

}
I2C::~I2C()
{
    // TODO Auto-generated destructor stub
}
extern "C" {
void EUSCIB1_IRQHandler(void){
    __disable_irq();
    if(UCB1CTLW0 & UCTR){ //If it is in Send Mode
        if(UCB1IFG & UCNACKIFG){ // If UCNACKIFG is set
            UCB1CTLW0 |= UCTXSTP; //Discard the transmition
            I2C::m_sbBusReady = 1;
            *I2C::m_sbFailedTransmition =1;
            *I2C::m_sbDone = 1;
            UCB1IFG &= ~ UCNACKIFG;
        }
        else if(UCB1IFG & UCBCNTIFG){
            I2C::m_sbBusReady = 1;
            *I2C::m_sbDone = 1;
            UCB1IFG &= ~ UCBCNTIFG;
        }
        else if(UCB1IFG & UCTXIFG0){
            UCB1TXBUF = I2C::m_su8ByteValues[I2C::m_sstIterator];
            I2C::m_sstIterator++;
            UCB1IFG &= ~ UCTXIFG0;
        }
    }
    else{//If it is in Receive mode
        if(UCB1IFG & UCNACKIFG){
            UCB1CTLW0 |= UCTXSTP;//Discard the transmition
            I2C::m_sbBusReady = 1;
            *I2C::m_sbFailedTransmition =1;
            *I2C::m_sbDone = 1;
            UCB1IFG &= ~ UCNACKIFG;
        }
        else if(UCB1IFG & UCBCNTIFG){
            I2C::m_sbBusReady = 1;
            *I2C::m_sbDone = 1;
            UCB1IFG &= ~ UCBCNTIFG;
        }
        else if(UCB1IFG & UCRXIFG0){
            I2C::m_spRxBuff[I2C::m_sstIterator] =(uint8_t) UCB1RXBUF;
            I2C::m_sstIterator++;
            UCB1IFG &= ~ UCRXIFG;
        }
    }
    __enable_irq();
}
}
