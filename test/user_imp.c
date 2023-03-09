#include <user_hooks.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <user_settings.h>
#include "flash_sim.h"

// Internal simulated flash memory
uint8_t _internal_flash_sim_mem[FLASH_SIM_MEM_SIZE] = { 0xFF };

// TODO: User must implement this function, see user_hooks.h for details.
int user_read_internal_flash( uint32_t address, uint8_t *data, uint32_t length )
{
    for( uint32_t i = 0; i < length; i++ ) {
        data[i] = _internal_flash_sim_mem[( address + i ) % FLASH_SIM_MEM_SIZE];
    }

    return length;
}

// TODO: User must implement this function, see user_hooks.h for details.
int user_write_internal_flash( uint32_t address, uint8_t *data, uint32_t length )
{
    for( uint32_t i = 0; i < length; i++ ) {
        _internal_flash_sim_mem[( address + i ) % FLASH_SIM_MEM_SIZE] &= data[i];
    }

    return length;
}

// TODO: User must implement this function, see user_hooks.h for details.
int user_erase_internal_flash( uint32_t address, uint32_t length )
{
    for( uint32_t i = 0; i < length; i++ ) {
        _internal_flash_sim_mem[( address + i ) % FLASH_SIM_MEM_SIZE] = 0xFF;
    }

    return length;
}

// TODO: User must implement this function, see user_hooks.h for details.
int user_get_internal_flash_page_size( void )
{
    return 256;
}

// TODO: User must implement this function, see user_hooks.h for details.
void user_select_ext_flash()
{
    _select();
}

// TODO: User must implement this function, see user_hooks.h for details.
void user_deselect_ext_flash()
{
    _deselect();
}

// TODO: User must implement this function, see user_hooks.h for details.
int user_write_ext_flash( uint8_t *data, uint16_t len )
{
    return _write(data, len);
}

// TODO: User must implement this function, see user_hooks.h for details.
int user_read_ext_flash( uint8_t *data, uint16_t len )
{
    return _read(data, len);
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
    uint32_t app_start = 0xFFFFFFFF;
    user_read_internal_flash( APP_START_ADDR, (uint8_t *)&app_start, sizeof(app_start) );
    return app_start == 0xFFFFFFFF ? -1 : 0;
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
    printf( "%c", character );
}

// TODO: User must implement this function, see user_hooks.h for details.
void _block_interrupts()
{}

// TODO: User must implement this function, see user_hooks.h for details.
void _unblock_interrupts()
{}