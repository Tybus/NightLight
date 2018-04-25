/*
 * NightLight.h
 *
 *  Created on: Apr 24, 2018
 *      Author: david
 */

#ifndef NIGHTLIGHT_H_
#define NIGHTLIGHT_H_
#include <LightSensor.h>
#include <Timer32.h>
#include <stdint.h>

#define LIGHTAMM 3
#define TURNON 1
#define TURNOFF 0
#define OFF 0
#define ON 1

namespace NightLight{
    //!Used to configure the Button as a interrupt
    void configurePORT3_5Button(void);
    //! Used to Configure the I2C bus, The ADC port for the mic and the Light Sensor periferal
    //! Displays the function confirmation (3ple blink :) )
    void functionConfirmation(void);
    //! Configure the lights in order to stablish, one two or three lights.
    void configureLights(void);
    //! Toggle LEDs
    void toggleLEDs(bool i_bONOFF);
    //! Determines the initial light state.
    bool initState(void);
    //!Configure the light sensor for the wanted task.
    void configureLightSensor( LightSensor *i_pLightSensor);
    //!Configures the RTC
    void configureRTC(void);
    //!Temporal configuration for the MIC (The ADC class was failing.
    void configureMic(void);
}


#endif /* NIGHTLIGHT_H_ */
