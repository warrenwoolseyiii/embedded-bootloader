#ifndef USER_SETTINGS_H
#define USER_SETTINGS_H

// User settings for the bootloader, use these to override any default settings in optiboot.h

// Max allowable programe size
#define MAX_PROG_SIZE 0x40000

// Max cache size to save on RAM
#define MAX_CACHE_SIZE 1024

// Program start address
#define APP_START_ADDR 0x800

// Flash image offset
#define FLASH_IMAGE_OFFSET 12
#define FLASH_IMAGE_START 0

#endif /* USER_SETTINGS_H */