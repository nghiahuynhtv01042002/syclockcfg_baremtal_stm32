// main.c
#include <stdint.h>
#include "sysclocks.h"
#define GPIOD_BASE    0x40020C00
#define GPIOD_MODER   (*(volatile uint32_t *)(GPIOD_BASE + 0x00))
#define GPIOD_ODR     (*(volatile uint32_t *)(GPIOD_BASE + 0x14))
extern uint32_t SystemCoreClock;

static inline void delay_ms(uint32_t ms) {
    uint32_t cycles = (SystemCoreClock / 3000) * ms;
    __asm__ volatile (
        "1: subs %[cycles], %[cycles], #1\n"
        "   bne 1b\n"
        : [cycles] "+r" (cycles)
        :
        : "cc"
    );
}

extern void config_MCO(void);
int main(void) {
    // init sysclock
    SystemClock_Config();
    // config MCO
    config_MCO();
    // Enable clock for GPIOD
    RCC_AHB1ENR |= RCC_AHB1ENR_GPIOD_EN;

    // Set PD12 as output (10)
    GPIOD_MODER |= (1 << 26);
    GPIOD_MODER &= ~(1 <<27);
    
    while (1) {
        // Toggle PD12
        GPIOD_ODR ^= (1 << 13);
        delay_ms(2000);
    }
}
