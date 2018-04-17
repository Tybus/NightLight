#include <FUNCTIONS_MAIN.h>

/*!Global Variables of the main file.
 *          These variables are needed to hold its results along the main code.
 *
 * 1. g_iPresentState:
 *          This variable is intended to carry the present state, according to the state machine
 *          defined before the development start.
 * 2. g_iSecCounter:
 *          This variable is intended to carry the seconds elapsed since it was settled to zero.
 * 3. g_iNSSummary:
 *          This variable is used during the Noise sensor function, the intention of this variable
 *          is to decide if the noise was enough to turn on, hold on or turn off the LEDs.
 * 4. g_iSecCounterONstate:
 *          This variable counts the seconds since the state ON started, when this variable is above
 *          30 seconds (simulating 30 min) the Noise Sensor function is activated to see if is necessary
 *          to switch again to OFF state or hold ON state.
 */
int g_iPresentState = 0;      // State selector, settled default at initial state
int g_iSecCounter = 0;        // Used for the noise sensor to compare the noise along 5 seconds with the noise at the last second
int g_iNSSummary = 0;         // Variable to decide about the noise to change between ON and OFF states
int g_iSecCounterONstate = 0; // Variable to count when the ON state reached the 30 min boundary

void SetupLeds(){
    P2DIR |= BIT4 | BIT6;
    P5DIR |= BIT7;
    P2OUT &= ~(BIT4 | BIT6);
    P5OUT &= ~BIT7;
}

/*!Function to setup the button:
 *         This function sets the P3.5 and P5.1 as inputs, these inputs are the buttons on the boosterpack MKII */
void SetupButtons(){
    P3DIR &= ~BIT5;
    P5DIR &= ~BIT1;
    //P3IN = ;
    //P5IN = ;
}

/*!Function to turn ON the LEDs:
 *          Sets to HIGH the P2.4, P2.6 and P5.7 outputs, which means to turn ON the LEDs on the boosterpack MKII*/
void LEDON(){
    P2OUT |= BIT4 | BIT6;
    P5OUT |= BIT7;
}

/*!Function to turn OFF the LEDs:
 *          Sets to LOW the P2.4, P2.6 and P5.7 outputs, which means to turn OFF the LEDs on the boosterpack MKII*/
void LEDOFF(){
    P2OUT &= ~(BIT4|BIT6);
    P5OUT &= ~BIT7;
}

/*!Function to sense the noise:
 *          This function uses the class named ADC to configure the microphone (create an object type ADC)
 *          and returns the microphone value using the read function on the class ADC.*/
int MicSense(){
    ADC Mic = ADC(P4,BIT3);
    Mic.quick_setup(1);
    Mic.init();
    return Mic.read(P4, BIT3);
}

/*!Function to calculate the noise.
 *          This function returns 0 if the noise RMS at 5 s is higher than the noise at the last 1 s, also returns 1 if otherwise.*/
int NoiseSensor(){
    printf("Noise sensor started.\n");
    int l_iMicResult = 0;
    float l_fAvNoise5 = 0;
    float l_fAvNoise1 = 0;
    int l_iN5 = 0;
    int l_iN1 = 0;
    g_iSecCounter = 0;
    while(g_iSecCounter < 5){
        while(g_iSecCounter < 4){
            printf("We are on second: %d\n", g_iSecCounter);
            l_iMicResult = MicSense();
            l_fAvNoise5 = l_fAvNoise5 + l_iMicResult*l_iMicResult;
            l_iN5++;
        }
        printf("We are on second: %d \n", g_iSecCounter);
        l_iMicResult = MicSense();
        l_fAvNoise5 = l_fAvNoise5 + l_iMicResult*l_iMicResult;
        l_fAvNoise1 = l_fAvNoise1 + l_iMicResult*l_iMicResult;
        l_iN5++;
        l_iN1++;
    }
    l_fAvNoise5 = sqrt(l_fAvNoise5/l_iN5);
    l_fAvNoise1 = sqrt(l_fAvNoise1/l_iN1);
    printf("Noise at 5 sec is %f and noise at 1 sec is %f \n", l_fAvNoise5, l_fAvNoise1);
    l_fAvNoise5 = l_fAvNoise5 + 0.1*l_fAvNoise5;
    //g_iSecCounter = 0;
    if(l_fAvNoise1 > l_fAvNoise5){
        return 1;
    }
    else{
        return 0;
    }

}

/*!States of the code flow
 *          InitState():
 *                      This is the initial state, here the LEDs blink 3 times with a period of 2 seconds (1 second ON and 1 second off),
 *                      then gives the control to the OFF state.
 *          OFFState():
 *                      This is the OFF state, here the device start to find if there is a condition to jump to ON state (LEDs ON) or hold on
 *                      OFF state (LEDs OFF). To get into this state there are 2 ways, first coming from InitState() and last coming from ONState().
 *          ONState():
 *                      This is the ON state, here the device turn on the LEDs, counts until 30 min and then start to find the OFF condition.
 * */
void InitState(void){
    SetupLeds();
    if(g_iSecCounter <= 6 && g_iSecCounter %2 == 0){
        LEDOFF();
    }
    if(g_iSecCounter <= 6 && g_iSecCounter %2 != 0){
        LEDON();
    }
    if(g_iSecCounter > 6){
        g_iPresentState = 1;
    }
}

void OFFState(void){
    LEDOFF();
    g_iNSSummary = NoiseSensor();
    if(g_iNSSummary == 0){
        g_iPresentState = 1;
    }
    if(g_iNSSummary == 1){
        g_iPresentState = 2;
        g_iSecCounterONstate = 0;
    }
}

void ONState(void){
    LEDON();
    while(g_iSecCounterONstate >= 30 && g_iPresentState == 2){
        g_iNSSummary = NoiseSensor();
        if(g_iNSSummary == 0){
            g_iPresentState = 1;
        }
        if(g_iNSSummary == 1){
            g_iPresentState = 2;
        }
    }

}

int StateChanger(){
    switch(g_iPresentState){
            case 0:
                return 0;
            case 1:
                return 1;
            case 2:
                return 2;
            }
    return 0; //Default OFF state
}

void TimeCounter(){
    g_iSecCounter++;
            if (g_iPresentState == 2){
                g_iSecCounterONstate++;
            }
}

void RTCConfiguration(void){
    printf("RTC Configuration ongoing.\n");
    RTCCTL0_H = RTCKEY_H;                 // Unlock RTC key protected registers
    RTCCTL1 = RTCBCD;                     // RTC enable, BCD mode
    RTCCTL3 |= RTCCALF_3;                 // Set the RTC calibration frequency to 1 Hz
    RTCPS1CTL |= RT1IP_6 | RT1PSIE;       // Prescale timer 0 interrupt interval divide by 128 | Prescale timer 0 interrupt enable
    RTCPS1CTL &= ~RT1PSIFG;               // Prescale timer 0 interrupt flag down
    RTCCTL0_H = 0;                        // Lock the RTC registers
    NVIC_EnableIRQ(RTC_C_IRQn);           // Interruption enabled for RTC interruption
    NVIC_SetPriority(RTC_C_IRQn,1);       // Priority of the RTC interruption
    printf("RTC Configuration done.\n");
}
