#ifndef HEX_RECORD_WRITER_H
#define HEX_RECORD_WRITER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <emb_ext_flash.h>
#include <user_settings.h>

#define HEX_RECORD_WRITER_ERROR_NONE                              0
#define HEX_RECORD_WRITER_ERROR_INVALID_RECORD_TYPE               -1
#define HEX_RECORD_WRITER_ERROR_INVALID_RECORD_LENGTH             -2
#define HEX_RECORD_WRITER_ERROR_INVALID_CHECKSUM                  -3
#define HEX_RECORD_WRITER_ERROR_INVALID_FORMAT                    -4
#define HEX_RECORD_WRITER_ERROR_NULL_PTR                          -5

#define HEX_RECORD_WRITER_RECORD_TYPE_DATA                        0
#define HEX_RECORD_WRITER_RECORD_TYPE_EOF                         1
#define HEX_RECORD_WRITER_RECORD_TYPE_EXTENDED_SEGMENT_ADDRESS    2
#define HEX_RECORD_WRITER_RECORD_TYPE_START_SEGMENT_ADDRESS       3
#define HEX_RECORD_WRITER_RECORD_TYPE_EXTENDED_LINEAR_ADDRESS     4
#define HEX_RECORD_WRITER_RECORD_TYPE_START_LINEAR_ADDRESS        5

/**
 * @brief Intel HEX record type
 */
typedef struct
{
   uint8_t  record_type, record_length, checksum;
   uint16_t address;
   uint8_t  data[256];
} intel_hex_record_t;

/**
 * @brief Convert a hex character to a byte
 * @param str String containing the hex character
 * @return Byte value of the hex character
 */
uint8_t hex_to_byte(char *str);

/**
 * @brief Parse a string containing an Intel HEX record
 * @param str String containing the Intel HEX record
 * @param record Pointer to a structure to store the parsed record
 * @return 0 on success, negative error code on failure
 */
int parse_string_hex_record(const char *str, intel_hex_record_t *record);

/**
 * @brief Parse a byte stream containing an Intel HEX record
 * @param stream Byte stream containing the Intel HEX record
 * @param stream_len Length of the byte stream
 * @param record Pointer to a structure to store the parsed record
 * @return 0 on success, negative error code on failure
 */
int parse_byte_stream_hex_record(const uint8_t *stream, int stream_len, intel_hex_record_t *record);

/**
 * @brief Calculate the checksum of an Intel HEX record
 * @param len Length of the record
 * @param addr Address of the record
 * @param type Type of the record
 * @param data Pointer to the data of the Intel HEX record
 * @param length Length of the data
 * @return Checksum
 */
uint8_t calculate_checksum(uint8_t len, uint16_t addr, uint8_t type, uint8_t *data, uint8_t length);

/**
 * @brief Write an Intel HEX record to flash
 *
 * @param intf pointer to the flash interface handle
 * @param flash_addr flash address to write to
 * @param byte_stream the byte stream to write
 * @param byte_stream_len length of the byte stream
 * @param is_string boolean indicating if the byte stream is a string or a byte stream
 * @return int, number of bytes written to flash memory
 */
int write_hex_record_to_flash(emb_flash_intf_handle_t *intf, uint32_t flash_addr, uint8_t *byte_stream, int byte_stream_len, bool is_string);

#ifdef __cplusplus
}
#endif

#endif /* HEX_RECORD_WRITER_H */
