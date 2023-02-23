#include "bl_version.h"
#include "deps/embedded-external-flash-memory/emb_ext_flash.h"
#include "optiboot.h"
#include "user_hooks.h"

void jump()
{
    // Pointer to the application space in memory
    void ( *application_code_entry )( void );

    // Call the user hook to prepare for the jump
    user_prep_for_application_jump();

    // Set the application entry pointer to the address, then jump!
    application_code_entry = ( void ( * )( void ) )(unsigned *)( *(unsigned *)( APP_START_ADDR + 4 ) );
    application_code_entry();
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
    uint8_t flex_image_header[12];
    if( emb_ext_flash_read( &_ext_flash_intf, FLASH_IMAGE_START, flex_image_header, 12 ) != 12 )
        return OPTIBOOT_ERR_FLASH_READ_FAIL;

    // Check the header key
    for( int i = 0; i < sizeof( _header_key ); i++ ) {
        if( flex_image_header[i] != _header_key[i] )
            return OPTIBOOT_ERR_INVALID_HEADER;
    }

    // Grab the image size and validate
    uint32_t imagesize = ( flex_image_header[7] << 24 ) | ( flex_image_header[8] << 16 ) |
                         ( flex_image_header[9] << 8 ) | flex_image_header[10];

    // Get the availble flash size
    uint8_t manf, mem_type, mem_cap;
    if( emb_ext_flash_get_jedec_id( &_ext_flash_intf, &manf, &mem_type, &mem_cap ) != 0 )
        return OPTIBOOT_ERR_FLASH_READ_FAIL;
    uint32_t mem_size = 1 << mem_cap;

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
    }

    // Erase the external flash image
    uint32_t addr = FLASH_IMAGE_START;
    for( ; addr < imagesize; addr += 4096 ) {
        if( emb_ext_flash_erase( &_ext_flash_intf, addr, 4096 ) != 0 )
            return OPTIBOOT_ERR_FLASH_ERASE_FAIL;
    }

    // Return OK
    return OPTIBOOT_OK;
}

// Main function
int main()
{
    // Attempt to initialize the external flash and check for an image
    if( optiboot_init() == OPTIBOOT_OK && check_optiboot_image() == OPTIBOOT_OK ) {
        // If the image is valid, jump to it
        jump();
    }

    // Check if there is an application present, if so, jump to it
    if( user_check_application() == 0 ) {
        jump();
    }

    // TODO: Otherwise sit in the serial bootloader
    return 0;
}