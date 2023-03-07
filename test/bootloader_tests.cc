
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

#include <gtest/gtest.h>
#include <emb_ext_flash.h>
#include <hex_record_writer.h>
#include <optiboot.h>
#include <bl_version.h>
#include <fstream>

// Flash simulation JEDEC ID
#define FLASH_SIM_JEDEC_ID 0x1F4401

// Flash simulation memory size
#define FLASH_SIM_MEM_SIZE 0x40000

// Flash sim memory bank, default to 0xFF
uint8_t _flash_sim_mem[FLASH_SIM_MEM_SIZE] = { 0xFF };

// Flash simulation state enumaration
enum flash_sim_state
{
    FLASH_SIM_STATE_IDLE,
    FLASH_SIM_SET_ADDR,
    FLASH_SIM_STATE_READ,
    FLASH_SIM_STATE_WRITE,
    FLASH_SIM_STATUS_REG_READ,
    FLASH_SIM_STATUS_REG_WRITE,
    FLASH_SIM_ERASE,
    FLASH_SIM_GET_JEDEC_ID,
};

// Flash simulation state
int _flash_sim_state = FLASH_SIM_STATE_IDLE;

// Flash simulation current address
uint32_t _flash_sim_addr = 0;

// Flash simulation write enable latch
bool _flash_sim_wel = false;

// Flash simulation status register
uint8_t _flash_sim_status_reg = 0;

// Flash simulation erase length setting
uint32_t _flash_sim_erase_len = 0;

// Parse the command, return 0 if successful, -1 if not.
int flash_sim_parse_cmd( uint8_t cmd )
{
    // Switch based on the command
    switch( cmd ) {
        case EXT_FLASH_CMD_WRITE_ENABLE:
            // Set the write enable latch
            _flash_sim_wel = true;
            // Set the state to address setting
            _flash_sim_state = FLASH_SIM_SET_ADDR;
            // Set the status register to EXT_FLASH_STATUS_REG_WEL
            _flash_sim_status_reg |= EXT_FLASH_STATUS_REG_WEL;
            break;
        case EXT_FLASH_CMD_WRITE_DISABLE:
            // Clear the write enable latch
            _flash_sim_wel = false;
            break;
        case EXT_FLASH_CMD_READ_STATUS_REG:
            // Set the state to read the status register
            _flash_sim_state = FLASH_SIM_STATUS_REG_READ;
            break;
        case EXT_FLASH_CMD_WRITE_STATUS_REG:
            // Set the state to write the status register
            _flash_sim_state = FLASH_SIM_STATUS_REG_WRITE;
            break;
        case EXT_FLASH_CMD_READ_DATA:
            // Set the state to address setting
            _flash_sim_state = FLASH_SIM_SET_ADDR;
            break;
        case EXT_FLASH_CMD_FAST_READ:
            // Set the state to address setting
            _flash_sim_state = FLASH_SIM_SET_ADDR;
            break;
        case EXT_FLASH_CMD_PAGE_PROGRAM:
            // If write enable latch is set, set the state to address setting, otherwise just ignore the command
            if( _flash_sim_wel ) {
                _flash_sim_state = FLASH_SIM_SET_ADDR;
            }
            else {
                _flash_sim_state = FLASH_SIM_STATE_IDLE;
            }
            break;
        case EXT_FLASH_CMD_SECTOR_ERASE:
            // If write enable latch is set, set the state to address setting, otherwise just ignore the command
            if( _flash_sim_wel ) {
                _flash_sim_state = FLASH_SIM_SET_ADDR;
                _flash_sim_erase_len = 4096;
            }
            else {
                _flash_sim_state = FLASH_SIM_STATE_IDLE;
                _flash_sim_erase_len = 0;
            }
            break;
        case EXT_FLASH_CMD_BLOCK_ERASE_32K:
            // If write enable latch is set, set the state to address setting, otherwise just ignore the command
            if( _flash_sim_wel ) {
                _flash_sim_state = FLASH_SIM_SET_ADDR;
                _flash_sim_erase_len = 32768;
            }
            else {
                _flash_sim_state = FLASH_SIM_STATE_IDLE;
                _flash_sim_erase_len = 0;
            }
            break;
        case EXT_FLASH_CMD_BLOCK_ERASE_64K:
            // If write enable latch is set, set the state to address setting, otherwise just ignore the command
            if( _flash_sim_wel ) {
                _flash_sim_state = FLASH_SIM_SET_ADDR;
                _flash_sim_erase_len = 65536;
            }
            else {
                _flash_sim_state = FLASH_SIM_STATE_IDLE;
                _flash_sim_erase_len = 0;
            }
            break;
        case EXT_FLASH_CMD_CHIP_ERASE:
            // If write enable latch is set, set the state to address setting, otherwise just ignore the command
            if( _flash_sim_wel ) {
                _flash_sim_erase_len = FLASH_SIM_MEM_SIZE;
                memset( _flash_sim_mem, 0xFF, FLASH_SIM_MEM_SIZE );
            }
            else {
                _flash_sim_state = FLASH_SIM_STATE_IDLE;
                _flash_sim_erase_len = 0;
            }
            break;
        case EXT_FLASH_CMD_POWER_DOWN:
            // Do nothing
            break;
        case EXT_FLASH_CMD_RELEASE_POWER_DOWN:
            // Do nothing
            break;
        case EXT_FLASH_CMD_JEDEC_ID:
            // Set the state to get the JEDEC ID
            _flash_sim_state = FLASH_SIM_GET_JEDEC_ID;
            break;
        default:
            // Unknown command, set to IDLE
            _flash_sim_state = FLASH_SIM_STATE_IDLE;
            // Return an error
            return -1;
    }
    // Return success
    return 0;
}

// Flash simulation address setter
int flash_sim_set_addr( uint8_t next_byte )
{
    // Each address is 3 bytes long, passed in a byte at a time. The first byte is the most significant byte.
    // The address is stored in the flash_sim_addr variable.
    // Shift the address left by 8 bits and add the next byte, if its the first byte, clear the address first.
    static uint8_t addr_byte = 0;
    if( addr_byte == 0 ) {
        _flash_sim_addr = 0;
    }
    _flash_sim_addr = ( _flash_sim_addr << 8 ) | next_byte;
    addr_byte++;

    // If the address is 3 bytes long, return success
    if( addr_byte == 3 ) {
        addr_byte = 0;
        // Make sure the address is within the flash memory range by modulating it
        _flash_sim_addr %= FLASH_SIM_MEM_SIZE;
        return 0;
    }

    // Otherwise return failure
    return -1;
}

// Flash simulation jedec id getter
uint8_t flash_sim_get_jedec_id( void )
{
    // The jedec id is a 3 byte value. Return the next byte in the sequence, and reset the state when done.
    static uint8_t jedec_id_byte = 0;
    uint8_t        ret = FLASH_SIM_JEDEC_ID >> ( 8 * ( 2 - jedec_id_byte++ ) );
    if( jedec_id_byte == 3 ) {
        jedec_id_byte = 0;
    }
    return ret;
}

// Flash simulation state machine
uint8_t flash_sim_sm( uint8_t next_byte )
{
    // Switch based on the state
    switch( _flash_sim_state ) {
        case FLASH_SIM_STATE_IDLE:
            // Parse the command, regardless of the result return 0xFF
            flash_sim_parse_cmd( next_byte );
            return 0xFF;
            break;
        case FLASH_SIM_SET_ADDR:
            // Parse the address, if its successful set the state to read or write depending on the WEL, return 0xFF regardless
            if( flash_sim_set_addr( next_byte ) == 0 ) {
                if( _flash_sim_wel && _flash_sim_erase_len == 0 ) {
                    _flash_sim_state = FLASH_SIM_STATE_WRITE;
                }
                else if( _flash_sim_wel && _flash_sim_erase_len > 0 ) {
                    // Erase the address space specified by the current address and the erase length
                    for( uint32_t i = 0; i < _flash_sim_erase_len; i++ ) {
                        _flash_sim_mem[( _flash_sim_addr + i ) % FLASH_SIM_MEM_SIZE] = 0xFF;
                    }
                    // Set the status register to busy
                    _flash_sim_status_reg |= EXT_FLASH_STATUS_REG_BUSY;
                    // Clear the WEL in the status register
                    _flash_sim_status_reg &= ~EXT_FLASH_STATUS_REG_WEL;
                    _flash_sim_wel = false;
                }
                else {
                    _flash_sim_state = FLASH_SIM_STATE_READ;
                }
            }
            return 0xFF;
            break;
        case FLASH_SIM_STATE_READ: {
            // Return the next byte of the read
            uint8_t ret = _flash_sim_mem[_flash_sim_addr];
            // Increment the address, protect against overflow
            _flash_sim_addr = ( _flash_sim_addr + 1 ) % FLASH_SIM_MEM_SIZE;
            return ret;
        } break;
        case FLASH_SIM_STATE_WRITE:
            // Write the next byte to the memory by AND'ing it with the byte
            _flash_sim_mem[_flash_sim_addr] &= next_byte;
            // Increment the address, protect against overflow
            _flash_sim_addr = ( _flash_sim_addr + 1 ) % FLASH_SIM_MEM_SIZE;
            // Make sure the status byte is set to write in progress
            _flash_sim_status_reg |= EXT_FLASH_STATUS_REG_BUSY;
            // Clear the WEL in the status register
            _flash_sim_status_reg &= ~EXT_FLASH_STATUS_REG_WEL;
            _flash_sim_wel = false;
            return 0xFF;
            break;
        case FLASH_SIM_STATUS_REG_READ:
            // Return the status register
            return _flash_sim_status_reg;
            break;
        case FLASH_SIM_STATUS_REG_WRITE:
            // Write the next byte to the status register
            _flash_sim_status_reg = next_byte;
            break;
        case FLASH_SIM_GET_JEDEC_ID:
            // Return the next byte of the JEDEC ID
            return flash_sim_get_jedec_id();
            break;
        default:
            // Unknown state, set to IDLE
            _flash_sim_state = FLASH_SIM_STATE_IDLE;
            break;
    }

    // Return the next byte to send
    return 0xFF;
}

// Interface "selected" flag
bool _is_selected = false;

// Interface select method
void _select()
{
    _is_selected = true;
}

// Interface deselect method
void _deselect()
{
    _is_selected = false;
    // Set the state to idle
    _flash_sim_state = FLASH_SIM_STATE_IDLE;
    // Set the erase length to 0
    _flash_sim_erase_len = 0;
    // Clear the busy bit in the status register
    _flash_sim_status_reg &= ~EXT_FLASH_STATUS_REG_BUSY;
    // Set the address to 0
    _flash_sim_addr = 0;
}

// Interface buffer write method, returns 0 if successful, -1 if not.
int _write( uint8_t *data, uint16_t len )
{
    // Run the flash simulation state machine for each byte in the buffer
    for( uint16_t i = 0; i < len; i++ ) {
        flash_sim_sm( data[i] );
    }
    return 0;
}

// Interface buffer read method, returns 0 if successful, -1 if not.
int _read( uint8_t *data, uint16_t len )
{
    // Run the flash simulation state machine for each byte in the buffer
    for( uint16_t i = 0; i < len; i++ ) {
        data[i] = flash_sim_sm( 0xFF );
    }
    return 0;
}

// Interface delay method for a specified duration in microseconds.
void _delay_us( uint32_t duration )
{
    // Do nothing
}

// Make a global interface struct
emb_flash_intf_handle_t _intf = {
    false,
    _select,
    _deselect,
    _write,
    _read,
    _delay_us };

// Class for facilitating bootloader tests
class bootloader_tests : public ::testing::Test
{
  public:
    bootloader_tests()
    {
        // initialization code here
    }

    void SetUp()
    {
        // code here will execute just before the test ensues
        // Force the interface struct to pass through
        emb_ext_flash_init_intf( &_intf );
        // Deselect the interface
        _intf.deselect();
        _flash_sim_wel = false;
    }

    void TearDown()
    {
        // code here will be called just after the test completes
        // ok to through exceptions from here if need be
    }

    ~bootloader_tests()
    {
        // cleanup any pending stuff, but no exceptions allowed
    }

    // put in any custom data members that you need
};

TEST_F( bootloader_tests, print_version )
{
    // Get the external flash dep version
    const char *version = emb_ext_flash_get_lib_ver();
    // Print the version
    printf( "Embedded bootloader version: %d.%d.%d\n", BL_VERSION_MAJOR, BL_VERSION_MINOR, BL_VERSION_REV );
    printf( "Embedded external flash dep version: %s\n", version );
}

TEST_F( bootloader_tests, parse_string_record )
{
    // Open the file
    std::ifstream file( "example_app_0x6000.hex" );

    // Read the file contents into a string
    std::string contents( ( std::istreambuf_iterator<char>( file ) ), std::istreambuf_iterator<char>() );

    // Read a single line from the file, convert it into a char *
    std::string line = contents.substr( 0, contents.find_first_of( "\r" ) );
    const char *line_cstr = line.c_str();

    // Parse the string into a hex record and verify that we parsed it properly
    intel_hex_record_t record;
    ASSERT_EQ( parse_string_hex_record( line_cstr, &record ), HEX_RECORD_WRITER_ERROR_NONE );
}

TEST_F( bootloader_tests, parse_string_record_null )
{
    // Parse a null string into a hex record and verify that we get an error
    intel_hex_record_t record;
    ASSERT_EQ( parse_string_hex_record( NULL, &record ), HEX_RECORD_WRITER_ERROR_NULL_PTR );
}

TEST_F( bootloader_tests, parse_string_record_bad_length )
{
    // Parse a string with a bad length into a hex record and verify that we get an error
    intel_hex_record_t record;
    ASSERT_EQ( parse_string_hex_record( ":00000001F", &record ), HEX_RECORD_WRITER_ERROR_INVALID_RECORD_LENGTH );
}

TEST_F( bootloader_tests, parse_string_record_bad_checksum )
{
    // Parse a string with a bad checksum into a hex record and verify that we get an error
    intel_hex_record_t record;
    ASSERT_EQ( parse_string_hex_record( ":00000001FB", &record ), HEX_RECORD_WRITER_ERROR_INVALID_CHECKSUM );
}

TEST_F( bootloader_tests, parse_string_record_bad_start )
{
    // Parse a string with a bad start into a hex record and verify that we get an error
    intel_hex_record_t record;
    ASSERT_EQ( parse_string_hex_record( "000000001FB", &record ), HEX_RECORD_WRITER_ERROR_INVALID_FORMAT );
}

TEST_F( bootloader_tests, parse_string_record_bad_type )
{
    // Parse a string with a bad type into a hex record and verify that we get an error
    intel_hex_record_t record;
    ASSERT_EQ( parse_string_hex_record( ":00000006FB", &record ), HEX_RECORD_WRITER_ERROR_INVALID_RECORD_TYPE );
}

TEST_F( bootloader_tests, parse_string_record_bad_record_len )
{
    // Parse a string with a bad address into a hex record and verify that we get an error
    intel_hex_record_t record;
    ASSERT_EQ( parse_string_hex_record( ":10000001FB", &record ), HEX_RECORD_WRITER_ERROR_INVALID_RECORD_LENGTH );
}

TEST_F( bootloader_tests, simulated_optiboot_write )
{
    // Open the file
    std::ifstream file( "example_app_0x6000.hex" );

    // Read the file contents into a string
    std::string contents( ( std::istreambuf_iterator<char>( file ) ), std::istreambuf_iterator<char>() );

    // Get the length of the file, get the flash ready to write
    uint32_t file_len = contents.length();
    uint32_t flash_addr = FLASH_IMAGE_START;
    for( ; flash_addr < file_len; flash_addr += 4096 ) {
        ASSERT_EQ( emb_ext_flash_erase( &_intf, flash_addr, 4096 ), 0 );
    }

    flash_addr = FLASH_IMAGE_START + FLASH_IMAGE_OFFSET;
    uint8_t copy_buffer[8192];
    int     copy_buffer_index = 0;

    // Assert that the file is not longer than the copy buffer
    ASSERT_LT( file_len / 2, sizeof( copy_buffer ) );

    // Print the file contents line by line
    while( contents.length() > 0 ) {
        // Read a single line from the file, convert it into a char *
        std::string line = contents.substr( 0, contents.find_first_of( "\r" ) );
        const char *line_cstr = line.c_str();

        // Parse a record and store the data in a the copy buffer - we only store the data records in flash so do the same here.
        intel_hex_record_t record;
        if( parse_string_hex_record( line_cstr, &record ) == HEX_RECORD_WRITER_ERROR_NONE ) {
            if( record.record_type == HEX_RECORD_WRITER_RECORD_TYPE_DATA ) {
                for( int i = 0; i < record.record_length; i++ ) {
                    copy_buffer[copy_buffer_index++] = record.data[i];
                }
            }
        }

        // Write the line to the flash
        flash_addr += write_hex_record_to_flash( &_intf, flash_addr, (uint8_t *)line_cstr, true );

        // Remove the line from the file contents
        contents.erase( 0, line.length() + 2 );
    }

    // Now go back and write the header
    flash_addr = FLASH_IMAGE_START;
    char header[12] = { 'F', 'L', 'X', 'I', 'M', 'G', ':', (char)( copy_buffer_index >> 24 ), (char)( copy_buffer_index >> 16 ), (char)( copy_buffer_index >> 8 ), (char)( copy_buffer_index >> 0 ), ':' };
    ASSERT_EQ( emb_ext_flash_write( &_intf, flash_addr, (uint8_t *)header, sizeof( header ) ), sizeof( header ) );

    // Read back the header and verify that it is correct
    uint8_t read_header[12];
    emb_ext_flash_read( &_intf, flash_addr, read_header, sizeof( read_header ) );
    ASSERT_EQ( memcmp( (uint8_t *)header, read_header, sizeof( header ) ), 0 );

    // Read back the contents of flash and compar eto the copy buffer
    uint8_t read_buffer[8192];
    emb_ext_flash_read( &_intf, FLASH_IMAGE_START + FLASH_IMAGE_OFFSET, read_buffer, 8192 );
    ASSERT_EQ( memcmp( copy_buffer, read_buffer, copy_buffer_index ), 0 );
}