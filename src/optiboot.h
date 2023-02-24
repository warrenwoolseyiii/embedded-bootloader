#ifndef OPTIBOOT_H
#define OPTIBOOT_H

#include <stdint.h>
#include "user/user_settings.h"

// Max allowable programe size
#ifndef MAX_PROG_SIZE
#define MAX_PROG_SIZE 0x40000
#endif

// Max cache size to save on RAM
#ifndef MAX_CACHE_SIZE
#define MAX_CACHE_SIZE 1024
#endif

// Program start address
#ifndef APP_START_ADDR
#define APP_START_ADDR 0x800
#endif

// Flash image offset
#ifndef FLASH_IMAGE_OFFSET
#define FLASH_IMAGE_OFFSET 12
#endif

// Flash image start
#ifndef FLASH_IMAGE_START
#define FLASH_IMAGE_START 0
#endif

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