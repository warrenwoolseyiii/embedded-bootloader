
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

#ifndef USER_SETTINGS_H
#define USER_SETTINGS_H

// User settings for the bootloader, use these to override any default settings in optiboot.h

// Max cache size to save on RAM
#define MAX_CACHE_SIZE 1024

// Program start address - make sure to adjust this in the rom - LENGTH definition in samd20e18_flash.ld file.
#if defined( SERIAL_DEBUG_EN )
#define APP_START_ADDR 0x00006000
#else
#define APP_START_ADDR 0x00002000
#endif

// Max allowable program size
#define MAX_PROG_SIZE ( 0x40000 - APP_START_ADDR )

// Flash image offset
#define FLASH_IMAGE_OFFSET 12
#define FLASH_IMAGE_START 0

#endif /* USER_SETTINGS_H */
