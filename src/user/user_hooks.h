
/*********************************************************************************
DISCLAIMER:

This code is protected under the MIT open source license. The code is provided
"as is" without warranty of any kind, either express or implied, including but
not limited to the implied warranties of merchantability, fitness for a particular
purpose, or non-infringement. In no event shall the author or any other party be
liable for any direct, indirect, incidental, special, exemplary, or consequential
damages, however caused and on any theory of liability, whether in contract,
strict liability, or tort (including negligence or otherwise), arising in any way
out of the use of this code or performance or use of the results of this code. By
using this code, you agree to hold the author and any other party harmless from
any and all liability and to use the code at your own risk.

This code was written by GitHub user: budgettsfrog
Contact: budgettsfrog@protonmail.com
GitHub: https://github.com/warrenwoolseyiii
*********************************************************************************/

#ifndef USER_HOOKS_H
#define USER_HOOKS_H

#include <stdint.h>

/**
 * @brief user_read_internal_flash read internal flash memory of the microprocessor. Return the number of bytes read.
 *
 * @param address - address to read from
 * @param data - pointer to the buffer to store the data
 * @param length - number of bytes to read
 * @return int - number of bytes read
 */
int user_read_internal_flash( uint32_t address, uint8_t *data, uint32_t length );

/**
 * @brief user_write_internal_flash write internal flash memory of the microprocessor. Return the number of bytes written.
 *
 * @param address - address to write to
 * @param data - pointer to the buffer to write
 * @param length - number of bytes to write
 * @return int - number of bytes written
 */
int user_write_internal_flash( uint32_t address, uint8_t *data, uint32_t length );

/**
 * @brief user_erase_internal_flash erase internal flash memory of the microprocessor. Return the number of bytes erased.
 *
 * @param address - address to erase from
 * @param length - number of bytes to erase
 * @return int - number of bytes erased
 */
int user_erase_internal_flash( uint32_t address, uint32_t length );

/**
 * @brief user_get_internal_flash_page_size return the size of the internal flash page
 *
 * @return int - size of the internal flash page
 */
int user_get_internal_flash_page_size( void );

/**
 * @brief user_select_ext_flash select the external flash memory chip
 *
 */
void user_select_ext_flash();

/**
 * @brief user_deselect_ext_flash deselect the external flash memory chip
 *
 */
void user_deselect_ext_flash();

/**
 * @brief user_write_ext_flash write bytes to the external flash memory chip, returns 0 if successful, -1 if not.
 *
 * @param data - pointer to the buffer to write
 * @param len - number of bytes to write
 * @return int - 0 if successful, -1 if not
 */
int user_write_ext_flash( uint8_t *data, uint16_t len );

/**
 * @brief user_read_ext_flash read bytes from the external flash memory chip, returns 0 if successful, -1 if not.
 *
 * @param data - pointer to the buffer to store the data
 * @param len - number of bytes to read
 * @return int - 0 if successful, -1 if not
 */
int user_read_ext_flash( uint8_t *data, uint16_t len );

/**
 * @brief user_delay_us delay for a specified number of microseconds
 *
 * @param duration - number of microseconds to delay
 */
void user_delay_us( uint32_t duration );

/**
 * @brief user_prep_for_application_jump prepare the microprocessor for jumping to the application
 *
 */
void user_prep_for_application_jump( void );

/**
 * @brief user_check_for_application check if the application is present
 *
 * @return int - 0 if application is present, -1 if not
 */
int user_check_for_application( void );

/**
 * @brief user_soft_reboot the processor. This function should not return.
 * 
 */
void user_soft_reboot( void );

/**
 * @brief user_serial_bootloader should run the serial bootloader (if present).
 * 
 * @return int - 0 if the serial bootloader was run and succeeded, -1 if not
 */
int user_serial_bootloader( void );

/**
 * @brief _putchar optional _purchar function that will enable the printf library.
 * 
 * @param character - character to print
 */
void _putchar( char character );


/**
 *  @brief block interrupts on your device. This function is declared here only. You
 * have to write your custom implementation somewhere.
 */
void _block_interrupts();

/**
 * @brief unblock interrupts on your device. This function is declared here only.
 * You have to write your custom implementation somewhere.
 */
void _unblock_interrupts();

#endif /* USER_HOOKS_H */