#include "msp.h"
#include <stdint.h>
#include <stdio.h>
#include <ADC.h>
#include <math.h>
#include <FUNCTIONS_MAIN.h>

uint8_t g_iState = 0;

void main(void){
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog time

	RTCConfiguration();

	/*!States transition routine: Used to pass between the different states*/
	while(1){
	    g_iState = StateChanger();
	    switch(g_iState){
	    case 0:
	        InitState();
	        break;
	    case 1:
	        OFFState();
	        break;
	    case 2:
	        ONState();
	        break;
	    }
	}
}

/*!For interruptions management*/
extern "C" {

    //Interruption for RTC each 1 second
    void RTC_C_IRQHandler(void) {
        __disable_irq();
        RTCCTL0_H = RTCKEY_H ;
        RTCCTL0_L &= ~RTCTEVIFG;
        RTCPS1CTL &= ~RT1PSIFG;
        RTCCTL0_H = 0;
        __enable_irq();
        TimeCounter();
    }
}
