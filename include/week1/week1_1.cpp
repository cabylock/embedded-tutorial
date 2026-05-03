#include "stm32f4xx_hal.h"


int main(void)
{
	RCC->AHB1ENR |= 1 << 0; // enable clock for gpioA
	GPIOA->MODER |= 1 << (2*5); // user led pa5 output mode


	while(1)
	{      
        
        
        GPIOA -> BSRR = 1 << 5;
        for(volatile uint32_t i = 0; i < 100000; i++);
        GPIOA -> BSRR = 1 << (5+16);
        for(volatile uint32_t i = 0; i < 100000; i++);

	}
}