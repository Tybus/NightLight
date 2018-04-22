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
        //printf("The light sensor reads : %f \n", l_LSLightSensor.read());
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
