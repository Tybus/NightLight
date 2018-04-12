/*
 * I2C.h
 *
 *  Created on: Apr 11, 2018
 *      Author: david
 */

#ifndef I2C_H_
#define I2C_H_
/*!Class that manages the I2C port in the MSP432 P401R LaunchPad Development Kit.
 * using the P6.4 as UCB1SDA and the P6.5 as UCB1SCL
 */
class I2C{
public:

    I2C();
    virtual ~I2C();
};

#endif /* I2C_H_ */
