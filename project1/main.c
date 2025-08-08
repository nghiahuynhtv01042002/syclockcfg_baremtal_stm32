// main.c
#include <stdint.h>
#include "sysclocks.h"
#define GPIOD_BASE    0x40020C00
#define GPIOD_MODER   (*(volatile uint32_t *)(GPIOD_BASE + 0x00))
#define GPIOD_ODR     (*(volatile uint32_t *)(GPIOD_BASE + 0x14))

__attribute__((optimize("O0")))
void delay_ms(uint32_t ms) {
    volatile uint32_t count = ms * (SystemCoreClock / 8000);
    while (count--) {
        __asm("nop");
    }
}

int main(void) {
    // init sysclock
    SystemClock_Config();
    // 1. Enable clock for GPIOD
    RCC_AHB1ENR |= RCC_AHB1ENR_GPIOD_EN;

    // 2. Set PD12 as output (MODER12 = 01)
    GPIOD_MODER |= (1 << 26);
    GPIOD_MODER &= ~(1 <<27);
    
    while (1) {
        // 3. Toggle PD12
        GPIOD_ODR ^= (1 << 13);

        delay_ms(1000);
    }
}
