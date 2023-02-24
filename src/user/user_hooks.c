#include "user_hooks.h"

// TODO: User must implement this function, see user_hooks.h for details.
int user_read_internal_flash( uint32_t address, uint8_t *data, uint32_t length )
{
    return 0;
}

// TODO: User must implement this function, see user_hooks.h for details.
int user_write_internal_flash( uint32_t address, uint8_t *data, uint32_t length )
{
    return 0;
}

// TODO: User must implement this function, see user_hooks.h for details.
int user_erase_internal_flash( uint32_t address, uint32_t length )
{
    return 0;
}

// TODO: User must implement this function, see user_hooks.h for details.
int user_get_internal_flash_page_size( void )
{
    return 0;
}

// TODO: User must implement this function, see user_hooks.h for details.
void user_select_ext_flash()
{
    // If your chip is not an SPI chip this function can be left empty.
}

// TODO: User must implement this function, see user_hooks.h for details.
void user_deselect_ext_flash()
{
    // If your chip is not an SPI chip this function can be left empty.
}

// TODO: User must implement this function, see user_hooks.h for details.
int user_write_ext_flash( uint8_t *data, uint16_t len )
{
    return -1;
}

// TODO: User must implement this function, see user_hooks.h for details.
int user_read_ext_flash( uint8_t *data, uint16_t len )
{
    return -1;
}

// TODO: User must implement this function, see user_hooks.h for details.
void user_delay_us( uint32_t duration )
{
}

// TODO: User must implement this function, see user_hooks.h for details.
void user_prep_for_application_jump( void )
{
}

// TODO: User must implement this function, see user_hooks.h for details.
int user_check_for_application( void )
{
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
}
