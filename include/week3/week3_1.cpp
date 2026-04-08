#include "stm32f4xx_hal.h"

void init_button(void)
{
   RCC->AHB1ENR |= 1 << 2; //clock for GPIOC

   GPIOC->MODER &= ~(3 << (2 * 13));
   GPIOC->PUPDR &= ~(3 << (2 * 13));

   GPIOC->MODER &= ~(3 << (2 * 8));
   GPIOC->PUPDR &= ~(3 << (2 * 8));
}

void init_led(void)
{
   RCC->AHB1ENR |= 1 << 0; //clock for GPIOA
   GPIOA->MODER |= 1 << (2 * 5);
}

void init_interrupt(void)
{
   RCC->APB2ENR |= 1 << 14;                         // enable SYSCFG clock

   SYSCFG->EXTICR[(13 / 4)] |= 2 << (4 * (13 % 4)); // set EXTI13 to be connected to PC13
   EXTI->IMR |= 1 << 13;                            // unmask EXTI13
   EXTI->FTSR |= 1 << 13;                           // trigger on falling edge for EXTI13
   NVIC_EnableIRQ(EXTI15_10_IRQn);                  // enable EXTI15_10 interrupt in NVIC

   SYSCFG->EXTICR[(8 / 4)] |= 2 << (4 * (8 % 4));   // set EXTI8 to be connected to PC8
   EXTI->IMR |= 1 << 8;                             // unmask EXTI8
   EXTI->FTSR |= 1 << 8;                            // trigger on falling edge for EXTI8
   NVIC_EnableIRQ(EXTI9_5_IRQn);                    // enable EXTI9_5 interrupt in NVIC
}

void EXTI15_10_IRQHandler(void)
{
   if (EXTI->PR & (1 << 13))
   {                                 // check if the interrupt is from EXTI13
      EXTI->PR |= (1 << 13);          // write 1 to clear pending bit
      GPIOA->BSRR = (1 << (5 + 16)); // turn on LED 
      for(volatile int i = 0; i < 100000; i++);

   }
}

void EXTI9_5_IRQHandler(void)
{
   if (EXTI->PR & (1 << 8))
   {                        // check if the interrupt is from EXTI8
      EXTI->PR |= (1 << 8);  // write 1 to clear pending bit
      GPIOA->BSRR = 1 << 5; // turn off LED 
      for(volatile int i = 0; i < 100000; i++);
     
   }
}

int main(void)
{
   init_button();
   init_led();
   init_interrupt();
   while (1)
   {
      GPIOA->ODR ^= 1 << 5;
      for (volatile int i = 0; i < 100000; i++);
   }
}