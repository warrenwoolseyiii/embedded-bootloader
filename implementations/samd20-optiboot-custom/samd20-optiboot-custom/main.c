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

#include "optiboot.h"
#include <atmel_start.h>
#include <atmel_start_pins.h>

extern void user_atmel_start_init();

#if defined(TEST_APPLICATION)
void blinky()
{
   uint8_t level = 0;

   while (1)
   {
      gpio_set_pin_level(LED_GREEN, level);
      gpio_set_pin_level(LED_RED, level);
      delay_ms(500);
      level ^= 0x01;
   }
}

#endif /* TEST_APPLICATION */

int main(void)
{
   atmel_start_init();
   user_atmel_start_init();
#if defined(TEST_APPLICATION)
   blinky();
#else
   optiboot();
#endif /* TEST_APPLICATION */
}
