#include "hex_record_writer.h"
#include "../optiboot.h"
#include "../user/user_hooks.h"
#include <string.h>

uint8_t hex_to_byte( char *str )
{
    uint8_t byte = 0;
    uint8_t i = 0;

    for( i = 0; i < 2; i++ ) {
        byte <<= 4;
        if( str[i] >= '0' && str[i] <= '9' )
            byte |= str[i] - '0';
        else if( str[i] >= 'A' && str[i] <= 'F' )
            byte |= str[i] - 'A' + 10;
        else if( str[i] >= 'a' && str[i] <= 'f' )
            byte |= str[i] - 'a' + 10;
    }

    return byte;
}

int parse_string_hex_record( const char *str, intel_hex_record_t *record )
{
    // Null check on the string and record
    if( str == NULL || record == NULL )
        return HEX_RECORD_WRITER_ERROR_NULL_PTR;

    // Check the string length
    if( strlen( str ) < 11 )
        return HEX_RECORD_WRITER_ERROR_INVALID_RECORD_LENGTH;

    // Check the start code
    if( str[0] != ':' )
        return HEX_RECORD_WRITER_ERROR_INVALID_FORMAT;

    // Check record length
    record->record_length = hex_to_byte( (char *)&str[1] );
    if( record->record_length != ( strlen( str ) - 11 ) / 2 )
        return HEX_RECORD_WRITER_ERROR_INVALID_RECORD_LENGTH;

    // Check the address
    record->address = hex_to_byte( (char *)&str[3] );
    record->address <<= 8;
    record->address |= hex_to_byte( (char *)&str[5] );

    // Check the record type
    record->record_type = hex_to_byte( (char *)&str[7] );
    if( record->record_type > HEX_RECORD_WRITER_RECORD_TYPE_START_LINEAR_ADDRESS )
        return HEX_RECORD_WRITER_ERROR_INVALID_RECORD_TYPE;

    // Get the checksum
    record->checksum = hex_to_byte( (char *)&str[strlen( str ) - 2] );

    // Get the data
    for( uint8_t i = 0; i < record->record_length; i++ )
        record->data[i] = hex_to_byte( (char *)&str[9 + i * 2] );

    // Check the checksum
    uint8_t checksum = calculate_checksum( record->record_length, record->address,
                                           record->record_type, record->data, record->record_length );
    if( checksum != record->checksum )
        return HEX_RECORD_WRITER_ERROR_INVALID_CHECKSUM;

    return HEX_RECORD_WRITER_ERROR_NONE;
}

int parse_byte_stream_hex_record( const uint8_t *stream, int stream_len, intel_hex_record_t *record )
{
    // Null check on the stream and record
    if( stream == NULL || record == NULL )
        return HEX_RECORD_WRITER_ERROR_NULL_PTR;

    // Check the start code
    if( stream[0] != ':' )
        return HEX_RECORD_WRITER_ERROR_INVALID_FORMAT;

    // Check record length
    record->record_length = stream[1];
    if( record->record_length != ( stream_len - 6 ) )
        return HEX_RECORD_WRITER_ERROR_INVALID_RECORD_LENGTH;

    // Check the address
    record->address = stream[2];
    record->address <<= 8;
    record->address |= stream[3];

    // Check the record type
    record->record_type = stream[4];
    if( record->record_type > HEX_RECORD_WRITER_RECORD_TYPE_START_LINEAR_ADDRESS )
        return HEX_RECORD_WRITER_ERROR_INVALID_RECORD_TYPE;

    // Get the checksum
    record->checksum = stream[5 + record->record_length];

    // Get the data
    for( uint8_t i = 0; i < record->record_length; i++ )
        record->data[i] = stream[5 + i];

    // Check the checksum
    uint8_t checksum = calculate_checksum( record->record_length, record->address,
                                           record->record_type, record->data, record->record_length );
    if( checksum != record->checksum )
        return HEX_RECORD_WRITER_ERROR_INVALID_CHECKSUM;

    return HEX_RECORD_WRITER_ERROR_NONE;
}

uint8_t calculate_checksum( uint8_t len, uint16_t addr, uint8_t type, uint8_t *data, uint8_t length )
{
    /*
        A record's checksum byte is the two's complement of the least significant byte (LSB)
        of the sum of all decoded byte values in the record preceding the checksum. It is computed
        by summing the decoded byte values and extracting the LSB of the sum (i.e., the data checksum),
        and then calculating the two's complement of the LSB (e.g., by inverting its bits and adding one).
        For example, in the case of the record :0300300002337A1E, the sum of the decoded byte values
        is 03 + 00 + 30 + 00 + 02 + 33 + 7A = E2, which has LSB value E2. The two's complement of E2
        is 1E, which is the checksum byte appearing at the end of the record.
    */
    uint8_t checksum = len + type + ( addr >> 8 ) + ( addr & 0xFF );
    uint8_t i = 0;

    for( i = 0; i < length; i++ )
        checksum += data[i];

    checksum = ~checksum + 1;
    return checksum;
}

int write_hex_record_to_flash( emb_flash_intf_handle_t *intf, uint32_t flash_addr, uint8_t *byte_stream, int byte_stream_len, bool is_string )
{
    // Parse a record from the byte stream
    intel_hex_record_t record;
    if( is_string ) {
        if( parse_string_hex_record( (const char *)byte_stream, &record ) != HEX_RECORD_WRITER_ERROR_NONE )
            return 0;
    }
    else {
        if( parse_byte_stream_hex_record( byte_stream, byte_stream_len, &record ) != HEX_RECORD_WRITER_ERROR_NONE )
            return 0;
    }

    // Check the record type
    if( record.record_type == HEX_RECORD_WRITER_RECORD_TYPE_DATA ) {
        // Write the data to flash
        if( emb_ext_flash_write( intf, flash_addr, record.data, (uint32_t)record.record_length ) != record.record_length )
            return 0;
    }

    return record.record_length;
}