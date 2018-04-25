/*
 * NightLight.cpp
 *
 *  Created on: Apr 24, 2018
 *      Author: david
 */
#include <NightLight.h>
void NightLight::configurePORT3_5Button(void){
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
void NightLight::functionConfirmation(void){
    for(uint8_t i = 0; i < 3; i++){
        toggleLEDs(TURNON);
        for(uint32_t l_u32TimeDelay = 60000; l_u32TimeDelay != 0; l_u32TimeDelay --);
        toggleLEDs(TURNOFF);
        for(uint32_t l_u32TimeDelay = 60000; l_u32TimeDelay != 0; l_u32TimeDelay --);
    }

}
void NightLight::configureLights(void){ //P5.6, P2.4 and P2.6
    P5SEL0 &= ~BIT6; //Set P5.6 as GPIO
    P5SEL1 &= ~BIT6;
    P5DIR |= BIT6; //Set as Output
    P5OUT &=~BIT6;

    if(LIGHTAMM >= 2){ //More than 1 light Green
        P2SEL0 &= ~BIT4; //Set P2.6 as GPIO
        P2SEL1 &= ~BIT4;
        P2DIR |= BIT4; //Set as Output
        P2OUT &=~BIT4;
    }
    if(LIGHTAMM == 3){ // Not turning on? Re
        P2SEL0 &= ~BIT6; //Set P2.6 as GPIO
        P2SEL1 &= ~BIT6;
        P2DIR |= BIT6; //Set as Output
        P2OUT &=~BIT6;
    }
}

void NightLight::toggleLEDs(bool i_bONOFF){
    switch(i_bONOFF){
    case TURNON:
        P5OUT |= BIT6;
        if(LIGHTAMM>=2)
            P2OUT |= BIT4;
        if(LIGHTAMM == 3)
            P2OUT |= BIT6;
        break;
    case TURNOFF:
        P5OUT &= ~BIT6;
        if(LIGHTAMM>=2)
            P2OUT &= ~BIT4;
        if(LIGHTAMM == 3)
            P2OUT &= ~BIT6;
        break;
    }
}
void NightLight::configureLightSensor(LightSensor * i_pLightSensor){
    //Setup and Start the Light sensor in Transparent Hystheresis Mode.
    i_pLightSensor->setLowLimit(LS_ONOFF_TH);
    i_pLightSensor->setHighLimit(LS_ONOFF_TH);
    P4SEL0 &= ~BIT6; // Selects P4.6 as GPIO
    P4SEL1 &= ~BIT6;

    P4DIR &= ~BIT6; //Selects the P4.6 as a input pin.
    P4REN &= ~BIT6; //Dissables the pull up/down resistors for P4.6
}
bool NightLight::initState(void){
    bool o_bLampState = OFF;
    if(P4IN & BIT6){// Light level is high
        toggleLEDs(TURNOFF);
        o_bLampState = OFF;
    }
    else{
        toggleLEDs(TURNON);
        o_bLampState = ON;
    }
    return o_bLampState;
}
void NightLight::configureRTC(void){
    RTCCTL0_H = RTCKEY_H;                 // Unlock RTC key protected registers
    RTCCTL1 = RTCBCD;                     // RTC enable, BCD mode
    RTCCTL3 |= RTCCALF_3;                 // Set the RTC calibration frequency to 1 Hz
    RTCPS1CTL |= RT1IP_6 | RT1PSIE;       // Prescale timer 0 interrupt interval divide by 128 | Prescale timer 0 interrupt enable
    RTCPS1CTL &= ~RT1PSIFG;               // Prescale timer 0 interrupt flag down
    RTCCTL0_H = 0;                        // Lock the RTC registers
    NVIC_EnableIRQ(RTC_C_IRQn);           // Interruption enabled for RTC interruption
    NVIC_SetPriority(RTC_C_IRQn,1);       // Priority of the RTC interruption
}

void NightLight::configureMic(void){
    P4->SEL0 = BIT3;
    P4->SEL1 = BIT3;
    P4->DIR &= ~BIT3;

    ADC14->CTL0 = ADC14_CTL0_DIV_2 |  ADC14_CTL0_SSEL__MCLK | ADC14_CTL0_ON | ADC14_CTL0_SHP;
    ADC14->CTL1 = ADC14_CTL1_RES__14BIT | ADC14_CTL1_DF; //Signed intergers 14 bit resolution.

    ADC14->MCTL[0] = ADC14_MCTLN_INCH_10;
    ADC14->CTL0 |= ADC14_CTL0_ENC;
    ADC14->IER0 = ADC14_IER0_IE0;


    NVIC_SetPriority(ADC14_IRQn,1);
    NVIC_EnableIRQ(ADC14_IRQn);
    ADC14->CTL0 |= ADC14_CTL0_SC;
}



