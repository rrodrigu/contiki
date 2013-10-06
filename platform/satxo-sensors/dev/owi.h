// This file has been prepared for Doxygen automatic documentation generation.
/*! \file ********************************************************************
*
* Atmel Corporation
*
* \li File:               OWIPolled.h
* \li Compiler:           IAR EWAAVR 3.20a
* \li Support mail:       avr@atmel.com
*
* \li Supported devices:  All AVRs.
*
* \li Application Note:   AVR318 - Dallas 1-Wire(R) master.
*                         
*
* \li Description:        Defines used in the polled 1-Wire(R) driver.
*
*                         $Revision: 1.7 $
*                         $Date: Thursday, August 19, 2004 14:27:18 UTC $
****************************************************************************/

#ifndef _ONEWIRE_H_
#define _ONEWIRE_H_

#include <avr/io.h>


/****************************************************************************
 ROM commands
****************************************************************************/
#define     OWI_ROM_READ    0x33    //!< READ ROM command code.
#define     OWI_ROM_SKIP    0xcc    //!< SKIP ROM command code.
#define     OWI_ROM_MATCH   0x55    //!< MATCH ROM command code.
#define     OWI_ROM_SEARCH  0xf0    //!< SEARCH ROM command code.


/****************************************************************************
 Return codes
****************************************************************************/
#define     OWI_ROM_SEARCH_FINISHED     0x00    //!< Search finished return code.
#define     OWI_ROM_SEARCH_FAILED       0xff    //!< Search failed return code.

#define     OWI_CRC_OK      0x00    //!< CRC check succeded
#define     OWI_CRC_ERROR   0x01    //!< CRC check failed

/*****************************************************************************
 The following defines only has an effect on the software only driver.
*****************************************************************************/


/*! \brief  Use internal pull-up resistor on 1-Wire buses.
 *
 *  If this symbol is defined, the internal pull-up resister on the GPIO pins 
 *  of the AVR will be used to generate the necessary pull-up on the bus. If 
 *  an external pull-up resistor is used, comment this define.
 */
//#define     OWI_USE_INTERNAL_PULLUP


// Port configuration registers for 1-Wire buses.
// Make sure that all three registers belong to the same port.
#define     OWI_PORT        PORTD   //!< 1-Wire PORT Data register.
#define     OWI_PIN         PIND    //!< 1-Wire Input pin register.
#define     OWI_DDR         DDRD    //!< 1-Wire Data direction register.


/*****************************************************************************
 Other defines
*****************************************************************************/
// Pin bitmasks.
#define     OWI_PIN_0       0x01
#define     OWI_PIN_1       0x02
#define     OWI_PIN_2       0x04
#define     OWI_PIN_3       0x08
#define     OWI_PIN_4       0x10
#define     OWI_PIN_5       0x20
#define     OWI_PIN_6       0x40
#define     OWI_PIN_7       0x80

#define     OWI_PIN_SERIAL  0x80


/*****************************************************************************
 Timing parameters
*****************************************************************************/

// Bit timing delays micro seconds
#define     OWI_DELAY_A_STD_MODE    6
#define     OWI_DELAY_B_STD_MODE    64
#define     OWI_DELAY_C_STD_MODE    60
#define     OWI_DELAY_D_STD_MODE    10
#define     OWI_DELAY_E_STD_MODE    9
#define     OWI_DELAY_F_STD_MODE    55
//#define     OWI_DELAY_G_STD_MODE  ((0   * CPU_FREQUENCY) - OWI_DELAY_OFFSET_CYCLES) 
#define     OWI_DELAY_H_STD_MODE    480
#define     OWI_DELAY_I_STD_MODE    70
#define     OWI_DELAY_J_STD_MODE    410

/****************************************************************************
 Headers for bit functions
****************************************************************************/

void owi_init(unsigned char pins);
void owi_write_bit1(unsigned char pins);
void owi_write_bit0(unsigned char pins);
unsigned char owi_read_bit(unsigned char pins);
unsigned char owi_detect_presence(unsigned char pins);

/****************************************************************************
 Headers for high level functions
****************************************************************************/

void owi_send_byte(unsigned char data, unsigned char pins);
unsigned char owi_receive_byte(unsigned char pin);
void owi_skip_rom(unsigned char pins);
void owi_read_rom(unsigned char * rom_value, unsigned char pins);
void owi_match_rom(unsigned char * rom_value, unsigned char pins);
unsigned char owi_search_rom(unsigned char * bit_pattern, unsigned char last_deviation, unsigned char pins);
unsigned char owi_compute_crc8(unsigned char in_data, unsigned char seed);
unsigned int owi_compute_crc16(unsigned char in_data, unsigned int seed);
unsigned char owi_check_rom_crc(unsigned char * rom_value);

/****************************************************************************
 Macros
****************************************************************************/
/*! \brief Pull 1-Wire bus low.
 *
 *  This macro sets the direction of the 1-Wire pin(s) to output and 
 *  pull the line(s) low.
 *  
 *  \param bitMask  A bitmask of the buses to pull low.
 */
#define OWI_PULL_BUS_LOW(bitmask) \
            OWI_DDR |= bitmask; \
            OWI_PORT &= ~bitmask;
            

/*! \def    OWI_RELEASE_BUS(bitmask)
 *
 *  \brief  Release the bus. 
 *
 *  This macro releases the bus and enables the internal pull-up if
 *  it is used.
 *
 *  \param  bitmask A bitmask of the buses to release.
 */
#ifdef OWI_USE_INTERNAL_PULLUP            
// Set 1-Wire pin(s) to input and enable internal pull-up resistor.
#define OWI_RELEASE_BUS(bitmask) \
            OWI_DDR &= ~bitmask; \
            OWI_PORT |= bitmask; 

#else
// Set 1-Wire pin(s) to input mode. No internal pull-up enabled.
#define OWI_RELEASE_BUS(bitmask) \
            OWI_DDR &= ~bitmask; \
            OWI_PORT &= ~bitmask;

#endif // OWI_USE_INTERNAL_PULLUP //

#endif
