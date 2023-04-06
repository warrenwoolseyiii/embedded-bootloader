/*
 * Code generated from Atmel Start.
 *
 * This file will be overwritten when reconfiguring your Atmel Start project.
 * Please copy examples or other code you want to keep to a separate file
 * to avoid losing it when reconfiguring.
 */
#ifndef ATMEL_START_PINS_H_INCLUDED
#define ATMEL_START_PINS_H_INCLUDED

#include <hal_gpio.h>

// SAMD20 has 8 pin functions

#define GPIO_PIN_FUNCTION_A    0
#define GPIO_PIN_FUNCTION_B    1
#define GPIO_PIN_FUNCTION_C    2
#define GPIO_PIN_FUNCTION_D    3
#define GPIO_PIN_FUNCTION_E    4
#define GPIO_PIN_FUNCTION_F    5
#define GPIO_PIN_FUNCTION_G    6
#define GPIO_PIN_FUNCTION_H    7

#define LED_GREEN              GPIO(GPIO_PORTA, 2)
#define LED_RED                GPIO(GPIO_PORTA, 4)
#define FLASH_CS               GPIO(GPIO_PORTA, 9)
#define RTD3_CS                GPIO(GPIO_PORTA, 10)
#define RTD2_CS                GPIO(GPIO_PORTA, 11)
#define RTD1_CS                GPIO(GPIO_PORTA, 16)
#define PA17                   GPIO(GPIO_PORTA, 17)
#define PA18                   GPIO(GPIO_PORTA, 18)
#define PA19                   GPIO(GPIO_PORTA, 19)
#define AC4                    GPIO(GPIO_PORTA, 22)
#define AC3                    GPIO(GPIO_PORTA, 23)
#define PA24                   GPIO(GPIO_PORTA, 24)
#define PA25                   GPIO(GPIO_PORTA, 25)
#define AC2                    GPIO(GPIO_PORTA, 27)
#define AC1                    GPIO(GPIO_PORTA, 28)

#endif // ATMEL_START_PINS_H_INCLUDED
