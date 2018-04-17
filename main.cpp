/*
 *  ======= main ========
 *
 *  Created on: Apr 13, 2018
 *  Author:     david
 */
#include "msp.h"
#include <LightSensor.h>

int main(void){
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog time
    nibble l_nRange;
    l_nRange.value = 0b1100;
    LightSensor l_LSLightSensor = LightSensor(l_nRange,CT_100MS,CONT_CONVERSION, TRANSP_HYSTH,ACTIVE_HIGH);
    while(1){
        printf("%f \n", l_LSLightSensor.read());
    }
}
