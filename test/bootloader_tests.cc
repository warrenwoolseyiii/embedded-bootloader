
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
#include "flash_sim.h"


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
        //_flash_sim_wel = false;
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

TEST_F( bootloader_tests, simulated_optiboot_string_write )
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
        flash_addr += write_hex_record_to_flash( &_intf, flash_addr, (uint8_t *)line_cstr, sizeof( line_cstr ), true );

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

TEST_F( bootloader_tests, parse_byte_record )
{
    // Open the file
    std::ifstream file( "example_app_0x6000.hex" );

    // Read the file contents into a string
    std::string contents( ( std::istreambuf_iterator<char>( file ) ), std::istreambuf_iterator<char>() );

    // Read a single line from the file, convert it into a char *
    std::string line = contents.substr( 0, contents.find_first_of( "\r" ) );
    const char *line_cstr = line.c_str();

    // Create a byte array the length of the line / 2
    uint8_t byte_array[line.length() / 2 + 1];

    byte_array[0] = line_cstr[0];

    // Convert the string into a byte array one byte at a time
    for( int i = 0; i < line.length() / 2; i++ ) {
        byte_array[i + 1] = hex_to_byte( (char *)&line_cstr[1 + i * 2] );
    }

    // Parse the byte array into a hex record
    intel_hex_record_t record;
    ASSERT_EQ( parse_byte_stream_hex_record( byte_array, sizeof( byte_array ), &record ), HEX_RECORD_WRITER_ERROR_NONE );
}

TEST_F( bootloader_tests, parse_byte_record_null )
{
    // Parse a null byte stream into a hex record and verify that we get an error
    intel_hex_record_t record;
    ASSERT_EQ( parse_byte_stream_hex_record( NULL, 0, &record ), HEX_RECORD_WRITER_ERROR_NULL_PTR );
}

TEST_F( bootloader_tests, parse_byte_record_bad_length )
{
    uint8_t byte_array[] = { ':', 0, 0, 0x1F };

    // Parse a byte stream with a bad length into a hex record and verify that we get an error
    intel_hex_record_t record;
    ASSERT_EQ( parse_byte_stream_hex_record( byte_array, sizeof( byte_array ), &record ), HEX_RECORD_WRITER_ERROR_INVALID_RECORD_LENGTH );
}

TEST_F( bootloader_tests, parse_byte_record_bad_checksum )
{
    uint8_t byte_array[] = { ':', 0x00, 0x00, 0x00, 0x01, 0xFB };

    // Parse a byte stream with a bad checksum into a hex record and verify that we get an error
    intel_hex_record_t record;
    ASSERT_EQ( parse_byte_stream_hex_record( byte_array, sizeof( byte_array ), &record ), HEX_RECORD_WRITER_ERROR_INVALID_CHECKSUM );
}

TEST_F( bootloader_tests, parse_byte_record_bad_start )
{
    uint8_t byte_array[] = { 0x00, 0x00, 0x00, 0x01, 0xFB, 0x00 };

    // Parse a byte stream with a bad start into a hex record and verify that we get an error
    intel_hex_record_t record;
    ASSERT_EQ( parse_byte_stream_hex_record( byte_array, sizeof( byte_array ), &record ), HEX_RECORD_WRITER_ERROR_INVALID_FORMAT );
}

TEST_F( bootloader_tests, parse_byte_record_bad_type )
{
    uint8_t byte_array[] = { ':', 0x00, 0x00, 0x00, 0x06, 0xFB };

    // Parse a byte stream with a bad type into a hex record and verify that we get an error
    intel_hex_record_t record;
    ASSERT_EQ( parse_byte_stream_hex_record( byte_array, sizeof( byte_array ), &record ), HEX_RECORD_WRITER_ERROR_INVALID_RECORD_TYPE );
}

TEST_F( bootloader_tests, parse_byte_record_bad_record_len )
{
    uint8_t byte_array[] = { ':', 0x10, 0x00, 0x00, 0x01, 0xFB };

    // Parse a byte stream with a bad address into a hex record and verify that we get an error
    intel_hex_record_t record;
    ASSERT_EQ( parse_byte_stream_hex_record( byte_array, sizeof( byte_array ), &record ), HEX_RECORD_WRITER_ERROR_INVALID_RECORD_LENGTH );
}

TEST_F( bootloader_tests, simulated_optiboot_byte_write )
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

        // Create a byte array the length of the line / 2
        uint8_t byte_array[line.length() / 2 + 1];

        byte_array[0] = line_cstr[0];

        // Convert the string into a byte array one byte at a time
        for( int i = 0; i < line.length() / 2; i++ ) {
            byte_array[i + 1] = hex_to_byte( (char *)&line_cstr[1 + i * 2] );
        }

        // Parse a record and store the data in a the copy buffer - we only store the data
        // records in flash so do the same here.
        intel_hex_record_t record;
        if( parse_byte_stream_hex_record( byte_array, sizeof( byte_array ), &record ) == HEX_RECORD_WRITER_ERROR_NONE ) {
            if( record.record_type == HEX_RECORD_WRITER_RECORD_TYPE_DATA ) {
                for( int i = 0; i < record.record_length; i++ ) {
                    copy_buffer[copy_buffer_index++] = record.data[i];
                }
            }
        }

        // Write the line to the flash
        flash_addr += write_hex_record_to_flash( &_intf, flash_addr, byte_array,
                                                 sizeof( byte_array ), false );

        // Remove the line from the file contents
        contents.erase( 0, line.length() + 2 );
    }

    // Now go back and write the header
    flash_addr = FLASH_IMAGE_START;
    char header[12] = { 'F', 'L', 'X', 'I', 'M', 'G', ':', (char)( copy_buffer_index >> 24 ),
                        (char)( copy_buffer_index >> 16 ), (char)( copy_buffer_index >> 8 ), (char)( copy_buffer_index >> 0 ), ':' };
    ASSERT_EQ( emb_ext_flash_write( &_intf, flash_addr, (uint8_t *)header, sizeof( header ) ),
               sizeof( header ) );

    // Read back the header and verify that it is correct
    uint8_t read_header[12];
    emb_ext_flash_read( &_intf, flash_addr, read_header, sizeof( read_header ) );
    ASSERT_EQ( memcmp( (uint8_t *)header, read_header, sizeof( header ) ), 0 );

    // Read back the contents of flash and compar eto the copy buffer
    uint8_t read_buffer[8192];
    emb_ext_flash_read( &_intf, FLASH_IMAGE_START + FLASH_IMAGE_OFFSET, read_buffer, 8192 );
    ASSERT_EQ( memcmp( copy_buffer, read_buffer, copy_buffer_index ), 0 );
}

void helper_write_image()
{
        // Open the file
    std::ifstream file( "example_app_0x6000.hex" );

    // Read the file contents into a string
    std::string contents( ( std::istreambuf_iterator<char>( file ) ), std::istreambuf_iterator<char>() );

    // Get the length of the file, get the flash ready to write
    uint32_t file_len = contents.length();
    uint32_t flash_addr = FLASH_IMAGE_START;
    for( ; flash_addr < file_len; flash_addr += 4096 ) {
        if( emb_ext_flash_erase( &_intf, flash_addr, 4096 ) != 0 ) {
            printf( "Failed to erase flash at 0x%08x\n", flash_addr );
            return;
        }
    }

    // Print the file contents line by line
    flash_addr = FLASH_IMAGE_START + FLASH_IMAGE_OFFSET;
    while( contents.length() > 0 ) {
        // Read a single line from the file, convert it into a char *
        std::string line = contents.substr( 0, contents.find_first_of( "\r" ) );
        const char *line_cstr = line.c_str();

        // Write the line to the flash
        flash_addr += write_hex_record_to_flash( &_intf, flash_addr, (uint8_t *)line_cstr, sizeof( line_cstr ), true );

        // Remove the line from the file contents
        contents.erase( 0, line.length() + 2 );
    }

    // Now go back and write the header
    uint32_t image_size = flash_addr - FLASH_IMAGE_OFFSET;
    flash_addr = FLASH_IMAGE_START;
    char header[12] = { 'F', 'L', 'X', 'I', 'M', 'G', ':', (char)( image_size >> 24 ), (char)( image_size >> 16 ), (char)( image_size >> 8 ), (char)( image_size >> 0 ), ':' };
    ASSERT_EQ( emb_ext_flash_write( &_intf, flash_addr, (uint8_t *)header, sizeof( header ) ), sizeof( header ) );
}

TEST_F( bootloader_tests, test_optiboot )
{
    helper_write_image();
    ASSERT_EQ( optiboot(), 0 );
}