#include "sysclocks.h"

uint32_t SystemCoreClock = 96000000ul;

void SystemClock_Config(void) {
    // Enable HSE (High Speed External oscillator)
    RCC_CR |= RCC_CR_HSEON;
    while(!(RCC_CR & RCC_CR_HSERDY)); // Wait for HSE ready
    
    // Configure Flash latency for 96MHz operation
    // Clear latency bits first
    FLASH_ACR &= ~(0x07);
    // Set 3 wait states for 96MHz (required for STM32F4 at this frequency)
    FLASH_ACR |= FLASH_ACR_LATENCY_3WS;
    
    // Configure PLL
    // Clear PLL configuration bits
    RCC_PLLCFGR &= ~((0x3F << 0)   // PLLM mask (bits 5:0)
                   | (0x1FF << 6)  // PLLN mask (bits 14:6)
                   | (0x3 << 16)   // PLLP mask (bits 17:16)
                   | (0xF << 24)   // PLLQ mask (bits 27:24)
                   | (1 << 22));   // PLLSRC bit
    
    // Set PLL parameters: HSE/PLLM * PLLN / PLLP = 8MHz/8 * 192 / 2 = 96MHz
    RCC_PLLCFGR |= RCC_PLLM      
                |  RCC_PLLN      
                |  RCC_PLLP      
                |  RCC_PLLQ      
                |  RCC_PLLSRC;   
    
    // Enable PLL
    RCC_CR |= RCC_CR_PLLON;
    while(!(RCC_CR & RCC_CR_PLLRDY)); // Wait for PLL ready
    
    // Clear prescaler bits
    RCC_CFGR &= ~((0x0F << 4)   // HPRE mask (AHB prescaler)
                | (0x07 << 10)  // PPRE1 mask (APB1 prescaler)
                | (0x07 << 13)); // PPRE2 mask (APB2 prescaler)
    
    // Set prescalers
    RCC_CFGR |= RCC_HPRE        // AHB prescaler = 1 (96MHz)
             |  RCC_PPRE1       // APB1 prescaler = 2 (48MHz - max for APB1)
             |  RCC_PPRE2;      // APB2 prescaler = 2 (48MHz)
    
    // Switch system clock to PLL
    RCC_CFGR = (RCC_CFGR & ~(0x03)) | RCC_SW; // Clear SW bits and set to PLL
    
    // Wait for PLL to be used as system clock
    while((RCC_CFGR & RCC_SWS_MASK) != RCC_SWS_PLL);
    

    // Update SystemCoreClock variable
    SystemCoreClock = 96000000ul;
}