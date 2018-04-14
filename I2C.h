/*
 * I2C.h
 *
 *  Created on: Apr 11, 2018
 *      Author: david
 */

#ifndef I2C_H_
#define I2C_H_
#include <stdint.h>
#include <vector>
extern "C" {
#include "msp.h"
}


#define STARTIF UCTXIFG0
#define MAX_BYTES 32
#define MAX_RETRY 5

#define TEMP_SENSOR ((uint8_t) 0b1000000)
#define LIGHT_SENSOR ((uint8_t) 0b1000100)

using namespace std;
/*!Class that manages the I2C port as a master in the MSP432 P401R LaunchPad Development Kit.
 * using the P6.4 as UCB1SDA and the P6.5 as UCB1SCL
 */
class I2C{
public:
    //! Creates a new I2C based on a slave address.
    I2C(uint8_t);
    /*!
     * Starts sending byte by byte using I2C.
     * @param[in] i_pByteValues Used to pass the pointer to a byte array.
     */

    bool send(uint8_t *, size_t);
    //! Reads data from the specified address.
    bool read(uint8_t *, size_t , bool *);
    //! Manages the Retry Counts in the Read and Write procedures
    static uint8_t m_u8RetryCount;
    //! Is used to store the Transmit Buffer
    static uint8_t * m_su8ByteValues;
    //! Stores the anmmount of data in the Transmit Buffer
    static size_t m_sstByteArraySize;
    //! Iterator used to iterate through the buffer values
    static uint32_t m_sstIterator;
    //! Saves the read buffer.
    static uint8_t * m_spRxBuff;
    //! Ammount of data to read,
    static size_t m_sstRxBuffSize;
    //! Looks if it is done reading
    static bool * m_sbReadDone;
    //! Destructor
    ~I2C();
private:
    //! Active if the I2C is already running.
    static bool m_bActive;

    //! Saves the slave address using it.
    uint16_t m_u16SlaveAddress;

};
#endif /* I2C_H_ */
