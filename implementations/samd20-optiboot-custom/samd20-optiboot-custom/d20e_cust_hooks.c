
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

#include "user_hooks.h"
#include "user_settings.h"
#include <atmel_start.h>
#include <atmel_start_pins.h>

struct io_descriptor *spi_io;
struct io_descriptor *uart_io;

void user_atmel_start_init()
{
    spi_m_sync_get_io_descriptor( &SPI_0, &spi_io );
    spi_m_sync_enable( &SPI_0 );

    usart_sync_get_io_descriptor( &USART_0, &uart_io );
    usart_sync_enable( &USART_0 );
}

// TODO: User must implement this function, see user_hooks.h for details.
int user_read_internal_flash( uint32_t address, uint8_t *data, uint32_t length )
{
    if( flash_read( &FLASH_0, address, data, length ) == ERR_NONE )
        return length;
    return 0;
}

// TODO: User must implement this function, see user_hooks.h for details.
int user_write_internal_flash( uint32_t address, uint8_t *data, uint32_t length )
{
    if( flash_write( &FLASH_0, address, data, length ) == ERR_NONE )
        return length;
    return 0;
}

// TODO: User must implement this function, see user_hooks.h for details.
int user_erase_internal_flash( uint32_t address, uint32_t length )
{
    int page_size = user_get_internal_flash_page_size();
    int num_pages = length / page_size;
    if( num_pages < 1 ) num_pages = 1;
    if( flash_erase( &FLASH_0, address, num_pages ) == ERR_NONE )
        return num_pages * page_size;
    return 0;
}

// TODO: User must implement this function, see user_hooks.h for details.
int user_get_internal_flash_page_size( void )
{
    return (int)flash_get_page_size( &FLASH_0 );
}

// TODO: User must implement this function, see user_hooks.h for details.
void user_select_ext_flash()
{
    gpio_set_pin_level( FLASH_CS, false );
}

// TODO: User must implement this function, see user_hooks.h for details.
void user_deselect_ext_flash()
{
    gpio_set_pin_level( FLASH_CS, true );
}

// TODO: User must implement this function, see user_hooks.h for details.
int user_write_ext_flash( uint8_t *data, uint16_t len )
{
    io_write( spi_io, data, len );
    return (int)len;
}

// TODO: User must implement this function, see user_hooks.h for details.
int user_read_ext_flash( uint8_t *data, uint16_t len )
{
    io_read( spi_io, data, len );
    return (int)len;
}

// TODO: User must implement this function, see user_hooks.h for details.
void user_delay_us( uint32_t duration )
{
    delay_us( duration );
}

// TODO: User must implement this function, see user_hooks.h for details.
void user_prep_for_application_jump( void )
{
}

// TODO: User must implement this function, see user_hooks.h for details.
int user_check_for_application( void )
{
    uint32_t start = 0;
    user_read_internal_flash( APP_START_ADDR, (uint8_t *)&start, sizeof( start ) );
    if( start != 0xFFFFFFFF ) return 0;
    return -1;
}

// TODO: User must implement this function, see user_hooks.h for details.
void user_soft_reboot( void )
{
}

// TODO: User must implement this function, see user_hooks.h for details.
int user_serial_bootloader( void )
{
    return -1;
}

// TODO: User must implement this function, see user_hooks.h for details.
void _putchar( char character )
{
    io_write( uart_io, (uint8_t *)&character, 1 );
}
