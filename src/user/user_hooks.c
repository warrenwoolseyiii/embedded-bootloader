
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
}
