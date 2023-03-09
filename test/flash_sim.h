#ifndef FLASH_SIM_H
#define FLASH_SIM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <emb_ext_flash.h>
#include <stdint.h>
#include <string.h>

// Flash simulation JEDEC ID
#define FLASH_SIM_JEDEC_ID 0x1F4410

// Flash simulation memory size
#define FLASH_SIM_MEM_SIZE 0x40000

// Parse the command, return 0 if successful, -1 if not.
int flash_sim_parse_cmd( uint8_t cmd );

// Flash simulation address setter
int flash_sim_set_addr( uint8_t next_byte );

// Flash simulation jedec id getter
uint8_t flash_sim_get_jedec_id( void );

// Flash simulation state machine
uint8_t flash_sim_sm( uint8_t next_byte );

// Interface select method
void _select();

// Interface deselect method
void _deselect();

// Interface buffer write method, returns 0 if successful, -1 if not.
int _write( uint8_t *data, uint16_t len );

// Interface buffer read method, returns 0 if successful, -1 if not.
int _read( uint8_t *data, uint16_t len );

// Interface delay method for a specified duration in microseconds.
void _delay_us( uint32_t duration );

#ifdef __cplusplus
}
#endif

#endif /* FLASH_SIM_H */