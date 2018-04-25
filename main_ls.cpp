/*
 *  ======= main ========
 *
 *  Created on: Apr 13, 2018
 *  Author:     david
 */
#include "msp.h"
#include <I2C.h>
#define CONFIGURATION_HIGH 0b11001100
#define CONFIGURATION_LOW 0b00000000

uint16_t read_register(I2C * i_pI2CSlave, uint8_t i_u8Register){
    //Start Writing the register.
    uint8_t l_u8ReadValue[2];
    uint16_t o_u16retval;
    bool * l_pReadReady = new bool(0);
    printf("Entrado al primer while\n");
     //No está entrando la interrupción.
    while(!i_pI2CSlave->send(&i_u8Register,1)) {
        printf("Waiting for Send#1\n");
    }
    printf("Entrando al segundo While");
    while(!i_pI2CSlave->read(l_u8ReadValue,2,l_pReadReady)){
        printf("Waiting to Start to Read\n");
    }
    while(1){
    }
    while(!l_pReadReady){
        printf("Waiting for read Done\n");
    }
    printf("ReadDone\n");
    o_u16retval = (((uint16_t) l_u8ReadValue[0]) <<8) + ((uint16_t) l_u8ReadValue[1]);
    return o_u16retval;

}
int main(void){
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog time
    printf("Inicializando, Creando el objeto \n");
    I2C l_I2CLightSensor = I2C(LIGHT_SENSOR);
    printf("Objecto Creado, entrando a la funcion\n");
    uint16_t l_u16RegValue = read_register(&l_I2CLightSensor,1);
    while(1){
        printf("%d /n",l_u16RegValue);
    }
    return (0);
}
