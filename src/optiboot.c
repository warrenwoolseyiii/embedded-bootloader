
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

#include "optiboot.h"
#include "bl_version.h"
#include "deps/embedded-external-flash-memory/emb_ext_flash.h"
#include "user_hooks.h"

#if defined( SERIAL_DEBUG_EN )
#include "printf.h"
#else
#define printf( ... ) (void)0
#endif /* SERIAL_DEBUG_EN */

void jump()
{
#ifndef UNIT_TEST
    // Pointer to the application space in memory
    void ( *application_code_entry )( void );

    // Call the user hook to prepare for the jump
    user_prep_for_application_jump();

    // Set the application entry pointer to the address, then jump!
    application_code_entry = ( void ( * )( void ) )(unsigned *)( *(unsigned *)( APP_START_ADDR + 4 ) );
    application_code_entry();
#endif /* UNIT_TEST */
}

// External flash interface struct
emb_flash_intf_handle_t _ext_flash_intf;

int optiboot_init()
{
    // Populate the global flash interface struct with the user implementations
    _ext_flash_intf.select = user_select_ext_flash;
    _ext_flash_intf.deselect = user_deselect_ext_flash;
    _ext_flash_intf.write = user_write_ext_flash;
    _ext_flash_intf.read = user_read_ext_flash;
    _ext_flash_intf.delay_us = user_delay_us;

    return emb_ext_flash_init_intf( &_ext_flash_intf ) == 0 ? OPTIBOOT_OK : OPTIBOOT_ERR_FLASH_INIT_FAIL;
}

// Flex image header, you can update this to your own header key
const char _header_key[] = "FLXIMG:";

int check_optiboot_image()
{
    /* Memory Layout
     ~~~ |0                   10                  20                  30 | ...
     ~~~ |0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1| ...
     ~~~ +---------------------------------------------------------------+
     ~~~ |F L X I M G|:|X X X X|:|                                       | ...
     ~~~ + - - - - - - - - - - - - - - - +-------------------------------+
     ~~~ | ID String |S|Len|S| Binary Image data                         | ...
     ~~~ +---------------------------------------------------------------+
     ~~~~~~ */

    // Read the flex image header
    uint8_t flex_image_header[12] = { 0 };
    if( emb_ext_flash_read( &_ext_flash_intf, FLASH_IMAGE_START, flex_image_header, 12 ) != 12 )
        return OPTIBOOT_ERR_FLASH_READ_FAIL;

    // Check the header key
    for( int i = 0; i < sizeof( _header_key ); i++ ) {
        if( flex_image_header[i] != _header_key[i] ) {
            printf( "Invalid header key: %s\n", flex_image_header );
            return OPTIBOOT_ERR_INVALID_HEADER;
        }
    }

    // Grab the image size and validate
    uint32_t imagesize = ( flex_image_header[7] << 24 ) | ( flex_image_header[8] << 16 ) |
                         ( flex_image_header[9] << 8 ) | flex_image_header[10];
    printf( "Image size: %d bytes\n", imagesize );

    // Get the availble flash size
    uint8_t manf, mem_type, mem_cap;
    if( emb_ext_flash_get_jedec_id( &_ext_flash_intf, &manf, &mem_type, &mem_cap ) != 0 )
        return OPTIBOOT_ERR_FLASH_READ_FAIL;
    uint32_t mem_size = 1 << mem_cap;
    printf( "Flash size: %d bytes\n", mem_size );

    // Make sure that the image size is valid
    if( imagesize > mem_size || imagesize == 0 || imagesize > MAX_PROG_SIZE )
        return OPTIBOOT_ERR_INVALID_SIZE;

    // Variables for moving the image to internal program space
    uint32_t program_space_addr = APP_START_ADDR;
    uint32_t internal_flash_page_size = user_get_internal_flash_page_size();
    uint16_t cache_index = 0;
    uint8_t  cache[MAX_CACHE_SIZE];

    // Ensure the cache is not smaller than the page size
    if( internal_flash_page_size > MAX_CACHE_SIZE )
        return OPTIBOOT_ERR_INVALID_CACHE_SIZE;

    // Copy the image to program space one page at a time
    for( uint32_t i = 0; i < imagesize; ) {
        // TODO: Optimize this for speed
        // Read out 1 byte at a time
        i += emb_ext_flash_read( &_ext_flash_intf, FLASH_IMAGE_START + FLASH_IMAGE_OFFSET + i, &cache[cache_index], 1 );
        cache_index++;

        // Once the cache is full of a page worth of data, copy it over to program space
        if( cache_index == internal_flash_page_size ) {
            if( user_erase_internal_flash( program_space_addr, internal_flash_page_size ) != internal_flash_page_size )
                return OPTIBOOT_ERR_INTERNAL_FLASH_ERASE_FAIL;
            if( user_write_internal_flash( program_space_addr, cache, internal_flash_page_size ) != internal_flash_page_size )
                return OPTIBOOT_ERR_INTERNAL_FLASH_WRITE_FAIL;
            printf( "Wrote record of %d bytes to addr 0x%08x\n", cache_index, program_space_addr );
#if 0 // Enable this to print the entire record that was written
            for( int j = 0; j < cache_index; j++ )
                printf("%02x ", cache[j]);
            printf("\n");
#endif
            program_space_addr += internal_flash_page_size;
            cache_index = 0;
        }
    }

    // If there is any data left in the cache, write it to program space
    if( cache_index > 0 ) {
        if( user_erase_internal_flash( program_space_addr, internal_flash_page_size ) != internal_flash_page_size )
            return OPTIBOOT_ERR_INTERNAL_FLASH_ERASE_FAIL;
        if( user_write_internal_flash( program_space_addr, cache, cache_index ) != cache_index )
            return OPTIBOOT_ERR_INTERNAL_FLASH_WRITE_FAIL;
        printf( "Wrote record of %d bytes to addr 0x%08x\n", cache_index, program_space_addr );
#if 0 // Enable this to print the entire record that was written
            for( int j = 0; j < cache_index; j++ )
                printf("%02x ", cache[j]);
            printf("\n");
#endif
    }

    // Erase the external flash image
    uint32_t addr = FLASH_IMAGE_START;
    for( ; addr < imagesize; addr += 4096 ) {
        if( emb_ext_flash_erase( &_ext_flash_intf, addr, 4096 ) != 0 )
            return OPTIBOOT_ERR_FLASH_ERASE_FAIL;
    }

    printf( "optiboot image copied to internal flash, total size %d bytes.\n", imagesize );

    // Return OK
    return OPTIBOOT_OK;
}

// Main function
int optiboot()
{
    // Print the welcome message
    printf( "Dual Optiboot Bootloader v%d.%d.%d\n", BL_VERSION_MAJOR, BL_VERSION_MINOR, BL_VERSION_REV );
    // Attempt to initialize the external flash and check for an image
    int ret = optiboot_init();
    if( ret != OPTIBOOT_OK )
        printf( "Error initializing external flash: %d\n", ret );
    else {
        ret = check_optiboot_image();
        if( ret != OPTIBOOT_OK )
            printf( "Error checking external flash image: %d\n", ret );
        else {
            printf( "External flash image is valid, jumping to program.\n" );
            jump();
            return 0;
        }
    }

    // Check if there is an application present, if so, jump to it
    ret = user_check_for_application();
    if( ret != 0 )
        printf( "Error checking application: %d\n", ret );
    else {
        printf( "Application is valid, jumping to program.\n" );
        jump();
        return 0;
    }

    // Run the serial bootloader if we haven't jumped to an application
    if( user_serial_bootloader() == 0 ) {
        jump();
        return 0;
    }

    // Otherwise soft reboot and try again
    user_soft_reboot();

    // Should never get here
    return -1;
}
