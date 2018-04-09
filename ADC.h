/*
 * ADC.h
 *
 *  Created on: Apr 8, 2018
 *      Author: david
 */

#ifndef ADC_H_
#define ADC_H_

#include "msp.h"
#include <vector>
#include <stdint.h>
#include <stdio.h>
using std::vector;
/*!
 * ADC class made for controlling the ADC ports in the MSP432 P401R LaunchPad
 * Development Kit.
 */
class ADC {
public:
    /*!
     * Standard constructor without paramenters. You'll have to add the ADC
     * port you want to use later on using ADC::addPort(DIO_PORT_Even_Interruptable_Type *, uint16_t)
     * or addPort(vector<DIO_PORT_Even_Interruptable_Type*>, vector<uint16_t>)
     *
     * After setting up the ports you have to use ADC::setup() to set the configuration parameters.
     */
    ADC();
    /*!
     * Standard constructor it will receive a @param[in] i_pPort and a @param[in] i_u16Bit to construct
     * a ADC based on the pin described.
     * You are needed to run a ADC::setup() or ADC::quick_setup(bool) in order to get it running.
     */
    ADC(DIO_PORT_Even_Interruptable_Type*, uint16_t);
    /*!
     * You can also make a @ref std::vector in order to implement a ADC based on two vectors of ports.
     * Keep in mind that multiple ports share the same configuration.
     */
    ADC(vector<DIO_PORT_Even_Interruptable_Type*>, vector<uint16_t>);
    /*!
     * You can enter the configuration values @param[in] i_u32CTL0 and @param[in] i_u32CTL1 in order to
     * get the ADC started with the described parameters. After succesfully running this you can run ADC::init(void)
     * in order to get the ADC started in the selected port with the selected parameters.
     * This method will activate the ADC interrupts.
     * This ADC construct will activate the interrupts from the ADC14_IRQHandler(void) automatically.
     */
    ADC(DIO_PORT_Even_Interruptable_Type*, uint16_t, uint32_t, uint32_t);
    /*!
     * Declaring multiple ports in the following constructor is easy by using std::vector, since configuration
     * files are shared between ADC conversions. You can ser @param[in] i_u32CTL0 and @param[in] i_u32CTL1 for once.
     * This ADC construct will activate the interrupts from the ADC14_IRQHandler(void) automatically.
     */
    ADC(vector<DIO_PORT_Even_Interruptable_Type*>, vector<uint16_t>, uint32_t, uint32_t);
    /*!
     * Copy constructor. Creates a copy of an already existing ADC object.
     */
    ADC(const ADC&);
    /*!
     * Destructor, TBD.
     */
    virtual ~ADC();
    /*!
     * Quick setup will start the ADC with the recommended values, use @param[in] i_bInterruptEnable in order to get
     * the interrupts running (or not)
     * run ADC::init(void) in order to start a conversion.
     */
    bool quick_setup(bool);
    /*!
     * Setup the ADC based on the already established configuration files. ADC::m_u32CTL0 and ADC::m_u32CTL1
     * if none has been selected, it will set it up using the ADC::quick_setup(bool) method.
     * use @param[in] i_bInterruptEnable in order to get the interrupts running (or not).
     * run ADC::init(void) in order to start a conversion.
     */
    bool setup(bool);
    /*!
     * Setup the ADC with new configuration files @param[in] i_u32CTL0 and @param[in] i_u32CTL1 in order to get
     * the ADC configured, run ADC::init(void) in order to start a conversion.
     *  use @param[in] i_bInterruptEnable in order to get the interrupts running (or not)
     */
    bool setup(uint32_t, uint32_t,bool);
    /*!
     * This method is used to change the setings of the ADC in midrun, remember that the configuration
     * parameters are the same for every channel you're using.
     */
    void change_settings(uint32_t, uint32_t);
    /*!
     * This method starts an ADC conversion. you'll have to wait untill it finishes and then use the
     * ADC::read() function in order to get the results. Or if handle the interrupts externally.
     */
    bool init(void);
    /*!
     * Adds additional ports to the ADC object, so you can controll more ports using a single object.
     */
    void addPort(DIO_PORT_Even_Interruptable_Type *, uint16_t);
    /*!
     * Adds additional ports using std::vector.
     */
    void addPort(vector<DIO_PORT_Even_Interruptable_Type*>, vector<uint16_t>);
    /*!
     * Returns a int16_t value, no matter the size of the convertion you configured, it is currently using
     * signed intergers as values. Will support other data types in the future.
     */
    int16_t read(DIO_PORT_Even_Interruptable_Type *, uint16_t);

private:
    /*!
     * Contains a static vector<IO_PORT_Even_Interruptable_Type *> which are ports. This is used in case
     * of creating multiple ADC objects, in order to not delete previously declared ports.
     */
    static vector<DIO_PORT_Even_Interruptable_Type *> m_vpStaticPorts;
    /*!
     * Contains a static std::vector<uint16_t> which are bits of the selected port. This is used in case
     * of creating multiple ADC objects, in order to not delete previously declared ports,
     */
    static vector<uint16_t> m_vu16StaticBit;
    /*!
     * Contains the Ports declared for this ADC.
     */
    vector<DIO_PORT_Even_Interruptable_Type *> m_vpPorts;
    /*!
     * Contains the Ports bits declared for this ADC.
     */
    vector<uint16_t> m_vu16Bit;
    /*!
     * Contains the universal ADC configuration for the ADC converter, since they're the same for every
     * ADC channel.
     */
    static uint16_t m_u32CTL0;
    /*!
     * Contains the universal ADC configuration for the ADC converter since they-re the same for every
     * ADC channel.
     */
    static uint16_t m_u32CTL1;
    /*!
     * This variable is set if a configured ADC already exists, in order to just use the already existing
     * ADC settings
     */
    static bool m_bStaticConfigured;
    /*!
     * This variable is used to keep a look at the ammount of used channels for the ADC.
     */
    static uint8_t m_u8AnalogChannels;
    /*!
     * This variable is set if the current ADC is configured. Keep in mind that if the ADC::m_bStaticConfigured
     * is set, this variable will be automatically set aswell.
     */
    bool m_configured;


};

#endif /* ADC_H_ */
