#ifndef OPTIBOOT_H
#define OPTIBOOT_H

#include <stdint.h>

// Max allowable programe size
#define MAX_PROG_SIZE 0x40000

// Max cache size to save on RAM
#define MAX_CACHE_SIZE 1024

// Program start address
#define APP_START_ADDR 0x800

// Flash image offset
#define FLASH_IMAGE_OFFSET 12
#define FLASH_IMAGE_START 0

// Optiboot error codes
#define OPTIBOOT_OK 0
#define OPTIBOOT_ERR_INVALID_HEADER -1
#define OPTIBOOT_ERR_INVALID_SIZE -2
#define OPTIBOOT_ERR_FLASH_READ_FAIL -3
#define OPTIBOOT_ERR_INVALID_CACHE_SIZE -4
#define OPTIBOOT_ERR_INTERNAL_FLASH_ERASE_FAIL -5
#define OPTIBOOT_ERR_INTERNAL_FLASH_WRITE_FAIL -6
#define OPTIBOOT_ERR_FLASH_ERASE_FAIL -7
#define OPTIBOOT_ERR_FLASH_INIT_FAIL -8

#endif /* OPTIBOOT_H */