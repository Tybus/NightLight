#include "msp.h"
#include <stdint.h>
#include <stdio.h>
//#include "ADC.h"
#define DAY_NIGHT_THR 10
#define true 1
#define false 0
/**
 * main.c
 */
void configure_adc(DIO_PORT_Even_Interruptable_Type * i_pPort, uint16_t i_u16Bit){

    i_pPort->SEL0 |= i_u16Bit;
    i_pPort->SEL1 |= i_u16Bit;

    ADC14->CTL0 = ADC14_CTL0_SHP | ADC14_CTL0_SHT0_0 | ADC14_CTL0_ON;
    ADC14->CTL0 |= ADC14_CTL0_CONSEQ_2 | ADC14_CTL0_MSC;
    ADC14->CTL1 |= ADC14_CTL1_RES_3;

    //ADC14->CTL0 = ADC14_CTL0_CONSEQ_2 | ADC14_CTL0_MSC | ADC14_CTL0_ENC | ADC14_CTL0_ON;
    ADC14->MCTL[0] = ADC14_MCTLN_INCH_10; //ADC14_MCTLN_VRSEL_14;
    ADC14->CTL1 |= ADC14_CTL1_DF; // Signed intergers.

    ADC14->CTL0 |= ADC14_CTL0_ENC;
    ADC14->IER0 = ADC14_IER0_IE0;

}
void configure_led(DIO_PORT_Even_Interruptable_Type * i_DPEITport, uint16_t i_u8bit){
    i_DPEITport->DIR |= i_u8bit;
}
void init_timer(void){
    printf("Hello World\n");
}

void setup(void){
    configure_adc(P4, BIT3);
    //configure_led();
    init_timer();
}

void main_loop(void){
    int i = 0 ;
    int j = 0;
    int16_t adc_result;
    while(true){
        //for(i = 0; i<100000; i++){
            //printf("adentrod el for");
        //}
        adc_result = ADC14->MEM[0];
        adc_result >>= 2;

        if(adc_result & 0x2000){
            adc_result |= 0xC000;
            //printf("negativo \n");
            //adc_result |= 0x0003;
        }
        int n = adc_result;
        for(j = 0; j<16; j++){
            if (n & 0x8000)
                printf("1");
            else
                printf("0");

            n <<= 1;
        }
        printf("\n");
        printf("%d \n \n",adc_result);
        //printf("%X \n",adc_result);
        //ADC14->CTL0 |= ADC14_CTL0_SC;
    }
}
void main(void){
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog time
	setup();
	//startup();
	ADC14->CTL0 |= ADC14_CTL0_SC;
	main_loop();
}



