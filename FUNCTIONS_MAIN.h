/*
 * FUNCTIONS_MAIN.h
 *
 *  Created on: Apr 16, 2018
 *      Author: K.Trejos
 */

#include "msp.h"
#include <stdint.h>
#include <stdio.h>
#include <ADC.h>
#include <math.h>


/*!Function to setup the LEDs:
 *          This function sets the P2.4, P2.6 and P5.7 as outputs, these outputs are the LEDs on the boosterpack MKII*/
void SetupLeds();

/*!Function to setup the button:
 *         This function sets the P3.5 and P5.1 as inputs, these inputs are the buttons on the boosterpack MKII */
void SetupButtons();

/*!Function to turn ON the LEDs:
 *          Sets to HIGH the P2.4, P2.6 and P5.7 outputs, which means to turn ON the LEDs on the boosterpack MKII*/
void LEDON();

/*!Function to turn OFF the LEDs:
 *          Sets to LOW the P2.4, P2.6 and P5.7 outputs, which means to turn OFF the LEDs on the boosterpack MKII*/
void LEDOFF();

/*!Function to sense the noise:
 *          This function uses the class named ADC to configure the microphone (create an object type ADC)
 *          and returns the microphone value using the read function on the class ADC.*/
int MicSense();

/*!Function to calculate the noise.
 *          This function returns 0 if the noise RMS at 5 s is higher than the noise at the last 1 s, also returns 1 if otherwise.*/
int NoiseSensor();

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
void InitState(void);

void OFFState(void);

void ONState(void);

/*! Function intended to configure the RTC to manage the interruptions each 1 second */
void RTCConfiguration(void);

/*!Function implemented to change the states*/
int StateChanger();

/*!Function intended to count each second, depending on the state where it is*/
void TimeCounter();
