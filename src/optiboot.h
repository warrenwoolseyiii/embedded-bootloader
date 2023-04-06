/*********************************************************************************
*  DISCLAIMER:
*
*  This code is protected under the MIT open source license. The code is provided
*  "as is" without warranty of any kind, either express or implied, including but
*  not limited to the implied warranties of merchantability, fitness for a particular
*  purpose, or non-infringement. In no event shall the author or any other party be
*  liable for any direct, indirect, incidental, special, exemplary, or consequential
*  damages, however caused and on any theory of liability, whether in contract,
*  strict liability, or tort (including negligence or otherwise), arising in any way
*  out of the use of this code or performance or use of the results of this code. By
*  using this code, you agree to hold the author and any other party harmless from
*  any and all liability and to use the code at your own risk.
*
*  This code was written by GitHub user: budgettsfrog
*  Contact: budgettsfrog@protonmail.com
*  GitHub: https://github.com/warrenwoolseyiii
*********************************************************************************/

#ifndef OPTIBOOT_H
#define OPTIBOOT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "user/user_settings.h"
#include <stdint.h>

// Max allowable programe size
#ifndef MAX_PROG_SIZE
#define MAX_PROG_SIZE    0x40000
#endif

// Max cache size to save on RAM
#ifndef MAX_CACHE_SIZE
#define MAX_CACHE_SIZE    1024
#endif

// Program start address
#ifndef APP_START_ADDR
#define APP_START_ADDR    0x8000
#endif

// Boot loader start address
#ifndef BL_START_ADDR
#define BL_START_ADDR    0
#endif

// Flash image offset
#ifndef FLASH_IMAGE_OFFSET
#define FLASH_IMAGE_OFFSET    12
#endif

// Flash image start
#ifndef FLASH_IMAGE_START
#define FLASH_IMAGE_START    0
#endif

// Optiboot error codes
#define OPTIBOOT_OK                               0
#define OPTIBOOT_ERR_INVALID_HEADER               -1
#define OPTIBOOT_ERR_INVALID_SIZE                 -2
#define OPTIBOOT_ERR_FLASH_READ_FAIL              -3
#define OPTIBOOT_ERR_INVALID_CACHE_SIZE           -4
#define OPTIBOOT_ERR_INTERNAL_FLASH_ERASE_FAIL    -5
#define OPTIBOOT_ERR_INTERNAL_FLASH_WRITE_FAIL    -6
#define OPTIBOOT_ERR_FLASH_ERASE_FAIL             -7
#define OPTIBOOT_ERR_FLASH_INIT_FAIL              -8

// Symbols for the linker
extern uint32_t __bootloader__;
extern uint32_t __application__;

int optiboot();

#ifdef __cplusplus
}
#endif

#endif /* OPTIBOOT_H */
