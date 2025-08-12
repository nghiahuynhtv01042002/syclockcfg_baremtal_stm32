#include "sysclocks.h"

uint32_t SystemCoreClock = 96000000ul;

void SystemClock_Config(void) {
    // Enable HSE 
    RCC_CR |= RCC_CR_HSEON;
    while(!(RCC_CR & RCC_CR_HSERDY)); 
    
    // Configure Flash latency for 96MHz operation
    // Clear latency bits first
    FLASH_ACR &= ~(0x07);
    // Set 3 wait states for 96MHz 
    // [***important***] because read and write spees is not as fast as System clock
    FLASH_ACR |= FLASH_ACR_LATENCY_3WS;
    FLASH_ACR |= (1 << 8) | (1 << 9) | (1 << 10);
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
    while(!(RCC_CR & RCC_CR_PLLRDY)); 
    
    // Clear prescaler bits
    RCC_CFGR &= ~((0x0F << 4)   // HPRE mask (AHB prescaler)
                | (0x07 << 10)  // PPRE1 mask (APB1 prescaler)
                | (0x07 << 13)); // PPRE2 mask (APB2 prescaler)
    
    // Set prescalers
    RCC_CFGR |= RCC_HPRE        // AHB prescaler = 1 (96MHz)
             |  RCC_PPRE1       // APB1 prescaler = 2 (48MHz - max for APB1)
             |  RCC_PPRE2;      // APB2 prescaler = 2 (48MHz)
    
    // Switch system clock to PLL
    //  clear SW bits
    RCC_CFGR &= ~ (0x03 << 0); // Clear SW bits 
    // Set to PLL
    RCC_CFGR |= RCC_SW;
    // Wait for PLL to be used as system clock
    while((RCC_CFGR & RCC_SWS_MASK) != RCC_SWS_PLL);
    // Update SystemCoreClock variable
    SystemCoreClock = 96000000ul;
}
/*
    this function is Config MCO to debug the sysclock but my logic analysis is a failure.
    Hence, i have to write and delay function to see the logic in GPIO 

*/

void config_MCO(void) {
    // Enable clock for GPIOA and GPIOC
    RCC_AHB1ENR |= RCC_AHB1ENR_GPIOA_EN | RCC_AHB1ENR_GPIOC_EN;
    
    // Configure PA8 as MCO1 (AF0)
    GPIOA_MODER &= ~(0x3 << 16);    // Clear PA8 mode
    GPIOA_MODER |= (0x2 << 16);     // Set AF mode
    GPIOA_AFRH &= ~(0xF << 0);      // Clear AF for PA8
    GPIOA_AFRH |= (0x0 << 0);       // AF0 = MCO1
    
    // Configure PC9 as MCO2 (AF0)
    GPIOC_MODER &= ~(0x3 << 18);    // Clear PC9 mode
    GPIOC_MODER |= (0x2 << 18);     // Set AF mode
    GPIOC_AFRH &= ~(0xF << 4);      // Clear AF for PC9
    GPIOC_AFRH |= (0x0 << 4);       // AF0 = MCO2
    
    // MCO1: Output HSI (16MHz) 
    // Clear MCO1 source and prescaler
    RCC_CFGR &= ~((0x3 << 21) | (0x7 << 24)); 
    // HSI source, no prescaler = 16MHz 
    RCC_CFGR |= (0x0 << 21) | (0x0 << 24);     
    
    // MCO2: Output SYSCLK/5 = 96MHz/5 = 19.2MHz
    // Clear MCO2 source and prescaler  
    RCC_CFGR &= ~((0x3 << 30) | (0x7 << 27));  
    // SYSCLK source, /5 prescaler
    RCC_CFGR |= (0x0 << 30) | (0x6 << 27);    
}
