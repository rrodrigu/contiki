// This file has been prepared for Doxygen automatic documentation generation.
/*! \file ********************************************************************
*
* Atmel Corporation
*
* \li File:               OWISWBitFunctions.c
* \li Compiler:           IAR EWAAVR 3.20a
* \li Support mail:       avr@atmel.com
*
* \li Supported devices:  All AVRs.
*
* \li Application Note:   AVR318 - Dallas 1-Wire(R) master.
*                         
*
* \li Description:        Polled software only implementation of the basic 
*                         bit-level signalling in the 1-Wire(R) protocol.
*
*                         $Revision: 1.7 $
*                         $Date: Thursday, August 19, 2004 14:27:18 UTC $
****************************************************************************/

#include "owi.h"

#include <ioavr.h>
#include <inavr.h>

#include <avr/io.h>
#include <util/delay.h>

/****************************************************************************
 Definitions for bit functions
****************************************************************************/


/*! \brief Initialization of the one wire bus(es). (Software only driver)
 *  
 *  This function initializes the 1-Wire bus(es) by releasing it and
 *  waiting until any presence sinals are finished.
 *
 *  \param  pins    A bitmask of the buses to initialize.
 */
void owi_init(unsigned char pins)
{
    OWI_RELEASE_BUS(pins);
    // The first rising edge can be interpreted by a slave as the end of a
    // Reset pulse. Delay for the required reset recovery time (H) to be 
    // sure that the real reset is interpreted correctly.
    _delay_us(OWI_DELAY_H_STD_MODE);
}


/*! \brief  Write a '1' bit to the bus(es). (Software only driver)
 *
 *  Generates the waveform for transmission of a '1' bit on the 1-Wire
 *  bus.
 *
 *  \param  pins    A bitmask of the buses to write to.
 */
void owi_write_bit1(unsigned char pins)
{
    // Disable interrupts.
    cli();
        
    // Drive bus low and delay.
    OWI_PULL_BUS_LOW(pins);
    _delay_us(OWI_DELAY_A_STD_MODE);
    
    // Release bus and delay.
    OWI_RELEASE_BUS(pins);
    _delay_us(OWI_DELAY_B_STD_MODE);
    
    // Restore interrupts.
    sei();
}


/*! \brief  Write a '0' to the bus(es). (Software only driver)
 *
 *  Generates the waveform for transmission of a '0' bit on the 1-Wire(R)
 *  bus.
 *
 *  \param  pins    A bitmask of the buses to write to.
 */
void owi_write_bit0(unsigned char pins)
{
    // Disable interrupts.
    cli();
    
    // Drive bus low and delay.
    OWI_PULL_BUS_LOW(pins);
    _delay_us(OWI_DELAY_C_STD_MODE);
    
    // Release bus and delay.
    OWI_RELEASE_BUS(pins);
    _delay_us(OWI_DELAY_D_STD_MODE);
    
    // Restore interrupts.
    sei();
}


/*! \brief  Read a bit from the bus(es). (Software only driver)
 *
 *  Generates the waveform for reception of a bit on the 1-Wire(R) bus(es).
 *
 *  \param  pins    A bitmask of the bus(es) to read from.
 *
 *  \return A bitmask of the buses where a '1' was read.
 */
unsigned char owi_read_bit(unsigned char pins)
{
    unsigned char bits_read;
    
    // Disable interrupts.
    cli();
    
    // Drive bus low and delay.
    OWI_PULL_BUS_LOW(pins);
    _delay_us(OWI_DELAY_A_STD_MODE);
    
    // Release bus and delay.
    OWI_RELEASE_BUS(pins);
    _delay_us(OWI_DELAY_E_STD_MODE);
    
    // Sample bus and delay.
    bits_read = OWI_PIN & pins;
    _delay_us(OWI_DELAY_F_STD_MODE);
    
    // Restore interrupts.
    sei();
    
    return bits_read;
}


/*! \brief  Send a Reset signal and listen for Presence signal. (software
 *  only driver)
 *
 *  Generates the waveform for transmission of a Reset pulse on the 
 *  1-Wire(R) bus and listens for presence signals.
 *
 *  \param  pins    A bitmask of the buses to send the Reset signal on.
 *
 *  \return A bitmask of the buses where a presence signal was detected.
 */
unsigned char owi_detect_presence(unsigned char pins)
{
    unsigned char presence_detected;
    
    // Disable interrupts.
    cli();
    
    // Drive bus low and delay.
    OWI_PULL_BUS_LOW(pins);
    _delay_us(OWI_DELAY_H_STD_MODE);
    
    // Release bus and delay.
    OWI_RELEASE_BUS(pins);
    _delay_us(OWI_DELAY_I_STD_MODE);
    
    // Sample bus to detect presence signal and delay.
    presence_detected = ((~OWI_PIN) & pins);
    _delay_us(OWI_DELAY_J_STD_MODE);
    
    // Restore interrupts.
    sei();
    
    return presence_detected;
}


/****************************************************************************
 Definitions for high level functions
****************************************************************************/


/*! \brief  Sends one byte of data on the 1-Wire(R) bus(es).
 *  
 *  This function automates the task of sending a complete byte
 *  of data on the 1-Wire bus(es).
 *
 *  \param  data    The data to send on the bus(es).
 *  
 *  \param  pins    A bitmask of the buses to send the data to.
 */
void owi_send_byte(unsigned char data, unsigned char pins)
{
    unsigned char temp;
    unsigned char i;
    
    // Do once for each bit
    for (i = 0; i < 8; i++)
    {
        // Determine if lsb is '0' or '1' and transmit corresponding
        // waveform on the bus.
        temp = data & 0x01;
        if (temp)
        {
            owi_write_bit1(pins);
        }
        else
        {
            owi_write_bit0(pins);
        }
        // Right shift the data to get next bit.
        data >>= 1;
    }
}


/*! \brief  Receives one byte of data from the 1-Wire(R) bus.
 *
 *  This function automates the task of receiving a complete byte 
 *  of data from the 1-Wire bus.
 *
 *  \param  pin     A bitmask of the bus to read from.
 *  
 *  \return     The byte read from the bus.
 */
unsigned char owi_receive_byte(unsigned char pin)
{
    unsigned char data;
    unsigned char i;

    // Clear the temporary input variable.
    data = 0x00;
    
    // Do once for each bit
    for (i = 0; i < 8; i++)
    {
        // Shift temporary input variable right.
        data >>= 1;
        // Set the msb if a '1' value is read from the bus.
        // Leave as it is ('0') else.
        if (owi_read_bit(pin))
        {
            // Set msb
            data |= 0x80;
        }
    }
    return data;
}


/*! \brief  Sends the SKIP ROM command to the 1-Wire bus(es).
 *
 *  \param  pins    A bitmask of the buses to send the SKIP ROM command to.
 */
void owi_skip_rom(unsigned char pins)
{
    // Send the SKIP ROM command on the bus.
    owi_send_byte(OWI_ROM_SKIP, pins);
}


/*! \brief  Sends the READ ROM command and reads back the ROM id.
 *
 *  \param  rom_value    A pointer where the id will be placed.
 *
 *  \param  pin     A bitmask of the bus to read from.
 */
void owi_read_rom(unsigned char * rom_value, unsigned char pin)
{
    unsigned char bytes_feft = 8;

    // Send the READ ROM command on the bus.
    owi_send_byte(OWI_ROM_READ, pin);
    
    // Do 8 times.
    while (bytes_left > 0)
    {
        // Place the received data in memory.
        *rom_value++ = owi_receive_byte(pin);
        bytes_left--;
    }
}


/*! \brief  Sends the MATCH ROM command and the ROM id to match against.
 *
 *  \param  rom_value    A pointer to the ID to match against.
 *
 *  \param  pins    A bitmask of the buses to perform the MATCH ROM command on.
 */
void owi_match_rom(unsigned char * rom_value, unsigned char pins)
{
    unsigned char bytes_left = 8;   
    
    // Send the MATCH ROM command.
    owi_send_byte(OWI_ROM_MATCH, pins);

    // Do once for each byte.
    while (bytes_left > 0)
    {
        // Transmit 1 byte of the ID to match.
        owi_send_byte(*rom_value++, pins);
        bytes_left--;
    }
}


/*! \brief  Sends the SEARCH ROM command and returns 1 id found on the 
 *          1-Wire(R) bus.
 *
 *  \param  bit_pattern      A pointer to an 8 byte char array where the 
 *                          discovered identifier will be placed. When 
 *                          searching for several slaves, a copy of the 
 *                          last found identifier should be supplied in 
 *                          the array, or the search will fail.
 *
 *  \param  last_deviation   The bit position where the algorithm made a 
 *                          choice the last time it was run. This argument 
 *                          should be 0 when a search is initiated. Supplying 
 *                          the return argument of this function when calling 
 *                          repeatedly will go through the complete slave 
 *                          search.
 *
 *  \param  pin             A bit-mask of the bus to perform a ROM search on.
 *
 *  \return The last bit position where there was a discrepancy between slave addresses the last time this function was run. Returns OWI_ROM_SEARCH_FAILED if an error was detected (e.g. a device was connected to the bus during the search), or OWI_ROM_SEARCH_FINISHED when there are no more devices to be discovered.
 *
 *  \note   See main.c for an example of how to utilize this function.
 */
unsigned char owi_search_rom(unsigned char * bit_pattern, unsigned char last_deviation, unsigned char pin)
{
    unsigned char current_bit = 1;
    unsigned char new_deviation = 0;
    unsigned char bitmask = 0x01;
    unsigned char bita;
    unsigned char bitb;

    // Send SEARCH ROM command on the bus.
    owi_send_byte(OWI_ROM_SEARCH, pin);
    
    // Walk through all 64 bits.
    while (current_bit <= 64)
    {
        // Read bit from bus twice.
        bita = owi_read_bit(pin);
        bitb = owi_read_bit(pin);

        if (bita && bitb)
        {
            // Both bits 1 (Error).
            new_deviation = OWI_ROM_SEARCH_FAILED;
            return;
        }
        else if (bita ^ bitb)
        {
            // Bits A and B are different. All devices have the same bit here.
            // Set the bit in bitPattern to this value.
            if (bita)
            {
                (*bit_pattern) |= bitmask;
            }
            else
            {
                (*bit_pattern) &= ~bitmask;
            }
        }
        else // Both bits 0
        {
            // If this is where a choice was made the last time,
            // a '1' bit is selected this time.
            if (current_bit == last_deviation)
            {
                (*bit_pattern) |= bitmask;
            }
            // For the rest of the id, '0' bits are selected when
            // discrepancies occur.
            else if (current_bit > last_deviation)
            {
                (*bit_pattern) &= ~bitmask;
                new_deviation = current_bit;
            }
            // If current bit in bit pattern = 0, then this is
            // out new deviation.
            else if ( !(*bit_pattern & bitmask)) 
            {
                new_deviation = current_bit;
            }
            // IF the bit is already 1, do nothing.
            else
            {
            
            }
        }
                
        
        // Send the selected bit to the bus.
        if ((*bit_pattern) & bitmask)
        {
            owi_write_bit1(pin);
        }
        else
        {
            owi_write_bit0(pin);
        }

        // Increment current bit.    
        current_bit++;

        // Adjust bitMask and bitPattern pointer.    
        bitmask <<= 1;
        if (!bitmask)
        {
            bitmask = 0x01;
            bit_pattern++;
        }
    }
    return new_deviation;
}


/****************************************************************************
 Definitions for CRC functions
****************************************************************************/


/*! \brief  Compute the CRC8 value of a data set.
 *
 *  This function will compute the CRC8 or DOW-CRC of inData using seed
 *  as inital value for the CRC.
 *
 *  \param  in_data  One byte of data to compute CRC from.
 *
 *  \param  seed    The starting value of the CRC.
 *
 *  \return The CRC8 of inData with seed as initial value.
 *
 *  \note   Setting seed to 0 computes the crc8 of the inData.
 *
 *  \note   Constantly passing the return value of this function 
 *          As the seed argument computes the CRC8 value of a
 *          longer string of data.
 */
unsigned char owi_compute_crc8(unsigned char in_data, unsigned char seed)
{
    unsigned char bits_left;
    unsigned char temp;

    for (bits_left = 8; bits_left > 0; bits_left--)
    {
        temp = ((seed ^ in_data) & 0x01);
        if (temp == 0)
        {
            seed >>= 1;
        }
        else
        {
            seed ^= 0x18;
            seed >>= 1;
            seed |= 0x80;
        }
        in_data >>= 1;
    }
    return seed;    
}


/*! \brief  Compute the CRC16 value of a data set.
 *
 *  This function will compute the CRC16 of inData using seed
 *  as inital value for the CRC.
 *
 *  \param  in_data  One byte of data to compute CRC from.
 *
 *  \param  seed    The starting value of the CRC.
 *
 *  \return The CRC16 of inData with seed as initial value.
 *
 *  \note   Setting seed to 0 computes the crc16 of the inData.
 *
 *  \note   Constantly passing the return value of this function 
 *          As the seed argument computes the CRC16 value of a
 *          longer string of data.
 */
unsigned int owi_compute_crc16(unsigned char in_data, unsigned int seed)
{
    unsigned char bits_left;
    unsigned char temp;

    for (bits_left = 8; bits_left > 0; bits_left--)
    {
        temp = ((seed ^ in_data) & 0x01);
        if (temp == 0)
        {
            seed >>= 1;
        }
        else
        {
            seed ^= 0x4002;
            seed >>= 1;
            seed |= 0x8000;
        }
        in_data >>= 1;
    }
    return seed;    
}


/*! \brief  Calculate and check the CRC of a 64 bit ROM identifier.
 *  
 *  This function computes the CRC8 value of the first 56 bits of a
 *  64 bit identifier. It then checks the calculated value against the
 *  CRC value stored in ROM.
 *
 *  \param  romvalue    A pointer to an array holding a 64 bit identifier.
 *
 *  \retval OWI_CRC_OK      The CRC's matched.
 *  \retval OWI_CRC_ERROR   There was a discrepancy between the calculated and the stored CRC.
 */
unsigned char owi_check_rom_crc(unsigned char * rom_value)
{
    unsigned char i;
    unsigned char crc8 = 0;
    
    for (i = 0; i < 7; i++)
    {
        crc8 = owi_compute_crc8(*rom_value, crc8);
        rom_value++;
    }
    if (crc8 == (*rom_value))
    {
        return OWI_CRC_OK;
    }
    return OWI_CRC_ERROR;
}
