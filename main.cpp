/*
 *  ======= main ========
 *
 *  Created on: Apr 13, 2018
 *  Author:     david
 */
#include "msp.h"
#include <LightSensor.h>
#include <Timer32.h>
#include <NightLight.h>
#include <math.h>
#include <stdio.h>

#define ONTIME 10
#define OVERNOISE 1.5

float g_f1sRMS = 0;
float g_f5sRMS = 0;
float g_f1sSquareSum = 0;
float g_f5sSquareSum = 0;

uint8_t g_u8SecondsPassed = 0;
uint32_t g_u32SampleAmmount1s = 0;
uint32_t g_u32SampleAmmount5s = 0;
uint32_t g_u32OnSeconds = 0;

bool g_bLampState = OFF;
int main(void){
    nibble l_nRange;
    l_nRange.value = 0b1100; //For configuration of the light sensor.

    LightSensor l_LSLightSensor = LightSensor(l_nRange,CT_100MS,CONT_CONVERSION, TRANSP_HYSTH,ACTIVE_HIGH);



    NightLight::configureLights();
    NightLight::configureLightSensor(&l_LSLightSensor);
    NightLight::functionConfirmation();
    g_bLampState = NightLight::initState();
    NightLight::configureRTC();
    NightLight::configureMic();
    NightLight::configurePORT3_5Button();

    while(1){
        if(g_bLampState == OFF){
            if(!l_LSLightSensor.aboveThreshold() && g_f1sRMS > OVERNOISE* g_f5sRMS){
                g_bLampState = 1;
                NightLight::toggleLEDs(ON);
            }
        }
    }
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
void ADC14_IRQHandler(void){
    __disable_irq();
    int16_t l_i16AnalogRead;
    int32_t l_i32SquaredRead;
    l_i16AnalogRead= ADC14->MEM[0];//Extend the sign. currently only valid for 14 bit conversions.
    l_i16AnalogRead >>= 2;
    if(l_i16AnalogRead & 0x2000){
        l_i16AnalogRead |= 0xC000;
    }
    l_i32SquaredRead = l_i16AnalogRead*l_i16AnalogRead;

    g_f1sSquareSum += l_i32SquaredRead;
    g_f5sSquareSum += l_i32SquaredRead;

    g_u32SampleAmmount1s ++;
    g_u32SampleAmmount5s ++;

    ADC14->CLRIFGR0 = ADC14_CLRIFGR0_CLRIFG0;
    ADC14->CTL0 |= ADC14_CTL0_SC;
    __enable_irq();
    return;
}
void RTC_C_IRQHandler(void) {
    __disable_irq();
    RTCCTL0_H = RTCKEY_H ;
    RTCCTL0_L &= ~RTCTEVIFG;
    RTCPS1CTL &= ~RT1PSIFG;
    RTCCTL0_H = 0;

    g_f1sRMS = sqrt(g_f1sSquareSum/g_u32SampleAmmount1s); //Calculate the RMS
    //int16_t l_i16AnalogValue = g_ADCMic.read(P4,BIT3);
   // printf("Loaded Value %d \n", l_i16AnalogValue);
    printf("The RMS value of 1 second is %f \n",g_f1sRMS);
    printf("We used a total of %d \n",g_u32SampleAmmount1s);
    g_f1sSquareSum = 0; //Reset the variables
    g_u32SampleAmmount1s = 0; //Reset the variables
    g_u8SecondsPassed ++; //Another second has passed
    if(g_u8SecondsPassed == 5){
        g_f5sRMS = sqrt(g_f5sSquareSum/g_u32SampleAmmount5s);
        printf("The RMS value of 5 seconds is %f \n", g_f5sRMS);
        printf("We used a total of %d \n", g_u32SampleAmmount5s);
        g_f5sSquareSum = 0; //Reset the variables
        g_u32SampleAmmount5s = 0; //Reset the variables
        g_u8SecondsPassed = 0;
    }
    if(g_bLampState == ON)
        g_u32OnSeconds++;
    else
        g_u32OnSeconds = 0;
    if(g_u32OnSeconds == ONTIME){
        g_u32OnSeconds = 0;
        g_bLampState = 0;
        NightLight::toggleLEDs(OFF);
    }
    __enable_irq();
    return;
}
void T32_INT1_IRQHandler(void){
    __disable_irq();

    TIMER32_1->INTCLR = 0; //Clear the interrupt
    Timer32::m_bTimer1Available = 1; //Make the timer Available Again.

    if(Timer32::m_pinBounceTIMER32_1.m_pOddPort != NULL
            && Timer32::m_pinBounceTIMER32_1.m_pEvenPort == NULL){//if its an Odd Port
        //Check the input value
        if(!(Timer32::m_pinBounceTIMER32_1.m_pOddPort->IN & Timer32::m_pinBounceTIMER32_1.m_u16Bit)){
            //The botton was pressed and it stabilized.
            NightLight::toggleLEDs(!g_bLampState);
            g_bLampState = !g_bLampState;
            g_u32OnSeconds = 0;
        }
        else{
            //False alarm!
        }
        //Re enable the interrupts from the button.
        Timer32::m_pinBounceTIMER32_1.m_pOddPort->IE |= Timer32::m_pinBounceTIMER32_1.m_u16Bit;
        Timer32::m_pinBounceTIMER32_1.m_pOddPort->IFG &= ~Timer32::m_pinBounceTIMER32_1.m_u16Bit;

    }
    else if(Timer32::m_pinBounceTIMER32_1.m_pEvenPort != NULL
            && Timer32::m_pinBounceTIMER32_1.m_pOddPort == NULL){//if its an Even Port
        //Check the input value
        if(!(Timer32::m_pinBounceTIMER32_1.m_pEvenPort->IN & Timer32::m_pinBounceTIMER32_1.m_u16Bit)){
            //The botton was pressed and it stabilized.
            NightLight::toggleLEDs(!g_bLampState);
            g_bLampState = !g_bLampState;
            g_u32OnSeconds = 0;
        }
        else{
            //False alarm!
        }
        //Re enable the interrupts from the button.
        Timer32::m_pinBounceTIMER32_1.m_pEvenPort->IE |= Timer32::m_pinBounceTIMER32_1.m_u16Bit;
        Timer32::m_pinBounceTIMER32_1.m_pEvenPort->IFG &= ~Timer32::m_pinBounceTIMER32_1.m_u16Bit;
    }
    else if(Timer32::m_pinBounceTIMER32_1.m_pOddPort != NULL //You should not enter here
            && Timer32::m_pinBounceTIMER32_1.m_pEvenPort != NULL){ //Both are non NULL
        //Enable interrupts from both pins
        Timer32::m_pinBounceTIMER32_1.m_pOddPort->IE |= Timer32::m_pinBounceTIMER32_1.m_u16Bit;
        Timer32::m_pinBounceTIMER32_1.m_pEvenPort->IE |= Timer32::m_pinBounceTIMER32_1.m_u16Bit;

    }
    TIMER32_1->CONTROL &= ~TIMER32_CONTROL_ENABLE & ~TIMER32_CONTROL_IE;// Dissable interrupts from timer.
    __enable_irq();
}
void T32_INT2_IRQHandler(void){
    __disable_irq();

    TIMER32_2->INTCLR = 0; //Clear the interrupt
    Timer32::m_bTimer1Available = 1; //Make the timer Available Again.

    if(Timer32::m_pinBounceTIMER32_2.m_pOddPort != NULL
            && Timer32::m_pinBounceTIMER32_2.m_pEvenPort == NULL){//if its an Odd Port
        //Check the input value
        if(!(Timer32::m_pinBounceTIMER32_2.m_pOddPort->IN & Timer32::m_pinBounceTIMER32_2.m_u16Bit)){
            //The botton was pressed and it stabilized.
        }
        else{
            //False alarm!
        }
        //Re enable the interrupts from the button.
        Timer32::m_pinBounceTIMER32_2.m_pOddPort->IE |= Timer32::m_pinBounceTIMER32_2.m_u16Bit;
        Timer32::m_pinBounceTIMER32_2.m_pOddPort->IFG &= ~Timer32::m_pinBounceTIMER32_2.m_u16Bit;
    }
    else if(Timer32::m_pinBounceTIMER32_2.m_pEvenPort != NULL
            && Timer32::m_pinBounceTIMER32_2.m_pOddPort == NULL){//if its an Even Port
        //Check the input value
        if(!(Timer32::m_pinBounceTIMER32_2.m_pEvenPort->IN & Timer32::m_pinBounceTIMER32_2.m_u16Bit)){
            //The botton was pressed and it stabilized.
        }
        else{
            //False alarm!
        }
        //Re enable the interrupts from the button.
        Timer32::m_pinBounceTIMER32_2.m_pEvenPort->IE |= Timer32::m_pinBounceTIMER32_2.m_u16Bit;
        Timer32::m_pinBounceTIMER32_2.m_pEvenPort->IFG &= ~Timer32::m_pinBounceTIMER32_2.m_u16Bit;
    }
    else if(Timer32::m_pinBounceTIMER32_2.m_pOddPort != NULL //You should not enter here
            && Timer32::m_pinBounceTIMER32_2.m_pEvenPort != NULL){ //Both are non NULL
        //Enable interrupts from both pins
        Timer32::m_pinBounceTIMER32_2.m_pOddPort->IE |= Timer32::m_pinBounceTIMER32_2.m_u16Bit;
        Timer32::m_pinBounceTIMER32_2.m_pEvenPort->IE |= Timer32::m_pinBounceTIMER32_2.m_u16Bit;

    }
    TIMER32_2->CONTROL &= ~TIMER32_CONTROL_ENABLE & ~TIMER32_CONTROL_IE;// Dissable interrupts from timer.
    __enable_irq();
}
}
