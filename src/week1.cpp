#include "stm32f4xx_hal.h"
#include "week1.h"




void week1(void)
{
	RCC->AHB1ENR |= 1 << 0;
	GPIOA->MODER |= 1 << (2*5);

    RCC->AHB1ENR |= 1 << 2;
    GPIOC->MODER |= 0 << (2*13); 
    GPIOC->PUPDR |= 0 << (2*13);
    
    uint16_t is_pressed; 
	while(1)
	{      
        is_pressed = GPIOC->IDR &= (1<<13);
        if (is_pressed == 0) {

            GPIOA -> BSRR = 1 << 5;
        }
        else 
        {
            GPIOA -> BSRR = 1 << (5+16);

        }
		
	}
}