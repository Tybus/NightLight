/*
 *  ======= main ========
 *
 *  Created on: Apr 13, 2018
 *  Author:     david
 */
#include "msp.h"
#include <LightSensor.h>
#include <Timer32.h>

void PORT3_5ButtonConfigure(void){
    P3IFG &= ~BIT5;
    P3SEL0 &= ~BIT5;
    P3SEL1 &= ~BIT5;

    P4DIR &= ~BIT5;
    P4REN &= ~BIT5;

    P3IES |= BIT5; //Interrupt in high to low transitions.

    P3IE |= BIT5; //Enables interrupts in the pin.
    //Let's accept interrupts.
    NVIC_SetPriority(PORT3_IRQn,1);
    NVIC_EnableIRQ(PORT3_IRQn);
}
int main(void){
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog time

    PORT3_5ButtonConfigure();
    while(1);

    /*
    uint16_t l_u16LowReadLimit, l_u16HighReadLimit;
    nibble l_nRange;
    l_nRange.value = 0b1100;
    LightSensor l_LSLightSensor = LightSensor(l_nRange,CT_100MS,CONT_CONVERSION, TRANSP_HYSTH,ACTIVE_HIGH);
    l_LSLightSensor.setLowLimit(LS_ONOFF_TH);
    l_LSLightSensor.setHighLimit(LS_ONOFF_TH);
    l_LSLightSensor.interruptConfigure();

    l_u16LowReadLimit = l_LSLightSensor.readRegister(LOWLIMIT_REGISTER);
    l_u16HighReadLimit = l_LSLightSensor.readRegister(HIGHLIMIT_REGISTER);
    printf("The low limit is: %X \n", l_u16LowReadLimit);
    printf("The high limit is: %X \n", l_u16HighReadLimit);

    //l_LSLightSensor = LightSensor(l_nRange,CT_100MS,CONT_CONVERSION, TRANSP_HYSTH,ACTIVE_HIGH); //Its failing when creating anotherone :s
    while(1){
        printf("The light sensor reads : %f \n", l_LSLightSensor.read());
        printf("The PORT4 reads: %X \n", P4IN);
    }

/*
    I2C lightSensor = I2C(LIGHT_SENSOR);
    uint8_t myArray [2];
    uint8_t registerAddress = 1;
    bool * ReadDone = (bool*) malloc(sizeof(bool));
    while(!lightSensor.send(&registerAddress,1));
    while(!lightSensor.read(myArray,2,ReadDone));
    while(!ReadDone);
    printf("Read is done \n");
    printf("%X \n",myArray[0]);
    printf("%X \n",myArray[1]);
    registerAddress = 0;
    while(1){
        while(!lightSensor.send(&registerAddress,1));
        while(!lightSensor.read(myArray,2,ReadDone));
        printf("Read is Done \n");
        printf("%X \n",myArray[0]);
        printf("%X \n",myArray[1]);
    }
*/

}
extern "C" {
void PORT4_IRQHandler(void){
    __disable_irq();
    LightSensor::PORT4_IRQHandler();
    __enable_irq();
}
void PORT3_IRQHandler(void){//Used for the button.
    __disable_irq();
    if(P3IFG & BIT5){
        Timer32::suppressBounce(P3,BIT5,1); //Set the suppression for 1ms
        P3IFG &= ~BIT5;
    }
    __enable_irq();
}
}
