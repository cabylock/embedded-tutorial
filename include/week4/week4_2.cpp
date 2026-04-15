#include "stm32f4xx_hal.h"

/*
 * Week 4 - Exercise 2
 * TIM3 interrupt every 500 ms:
 * - PA5 toggles every 1 second in TIM3 IRQ
 * - PB7 keeps blinking in main loop with software delay
 */


void init_leds(void)
{
   RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN;

   GPIOA->MODER &= ~(3U << (2U * 5U));
   GPIOA->MODER |= (1U << (2U * 5U));

   GPIOB->MODER &= ~(3U << (2U * 7U));
   GPIOB->MODER |= (1U << (2U * 7U));
}

void delay_ms(uint32_t ms)
{
    RCC->APB1ENR |= (1 << 0); // Enable TIM2

    TIM2->PSC = 16000 - 1;   // 1ms tick (16MHz / 16000 = 1kHz)
    TIM2->ARR = ms - 1;

    TIM2->CNT = 0; 
    TIM2->SR = 0;            // Clear flag

    TIM2->CR1 |= 1;          // Start

    while (!(TIM2->SR & 1)); // Wait UIF

    TIM2->CR1 &= ~1;         // Stop timer
    TIM2->SR &= ~1;          // Clear flag
}


void tim3_init_1000ms(void)
{
   RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

   TIM3->CR1 = 0;
   TIM3->PSC = 16000U - 1U; /* 1 kHz */
   TIM3->ARR = 1000U - 1U;   /* 1000 ms update */
   TIM3->CNT = 0;
   TIM3->DIER |= TIM_DIER_UIE;
   TIM3->EGR = TIM_EGR_UG;
   TIM3->SR &= ~TIM_SR_UIF;

   NVIC_EnableIRQ(TIM3_IRQn);

   TIM3->CR1 |= 1;
}

void TIM3_IRQHandler(void)
{

   if (TIM3->SR & 1)
   {
      TIM3->SR &= ~1;

     
         GPIOA->ODR ^= (1U << 5U); /* PA5 toggles every 1 s */


      
   }
}

int main(void)
{
   init_leds();
   tim3_init_1000ms();

   while (1)
   {
      GPIOB->ODR ^= (1U << 7U);
      delay_ms(500);
   }
}
