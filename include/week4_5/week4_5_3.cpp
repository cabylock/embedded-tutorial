#include "stm32f4xx_hal.h"

// cấu hình sw1, sw2, sw3 trên pc0, pc1, pc2
// led 1, 2, 3 trên pb13, pb14, pb15

void delay_loop(uint32_t count)
{
   for (volatile uint32_t i = 0; i < count; i++)
      ;
}

void init_button(void)
{
   RCC->AHB1ENR |= 1 << 2; // clock for gpioc

   // sw1 on pc0
   GPIOC->MODER &= ~(3 << (2 * 0));
   GPIOC->PUPDR &= ~(3 << (2 * 0));

   // sw2 on pc1
   GPIOC->MODER &= ~(3 << (2 * 1));
   GPIOC->PUPDR &= ~(3 << (2 * 1));

   // sw3 on pc2
   GPIOC->MODER &= ~(3 << (2 * 2));
   GPIOC->PUPDR &= ~(3 << (2 * 2));
}

void init_led(void)
{
   RCC->AHB1ENR |= 1 << 1; // clock for gpiob

   // led 1 on pb13
   GPIOB->MODER &= ~(3 << (2 * 13));
   GPIOB->MODER |= 1 << (2 * 13);

   // led 2 on pb14
   GPIOB->MODER &= ~(3 << (2 * 14));
   GPIOB->MODER |= 1 << (2 * 14);

   // led 3 on pb15
   GPIOB->MODER &= ~(3 << (2 * 15));
   GPIOB->MODER |= 1 << (2 * 15);
}

void init_interrupt(void)
{
   RCC->APB2ENR |= 1 << 14;

   // sw1 -> pc0 -> exti0
   SYSCFG->EXTICR[0] &= ~(0xF << (4 * 0));
   SYSCFG->EXTICR[0] |= 2 << (4 * 0); // 2 means gpioc

   // sw2 -> pc1 -> exti1
   SYSCFG->EXTICR[0] &= ~(0xF << (4 * 1));
   SYSCFG->EXTICR[0] |= 2 << (4 * 1);

   // sw3 -> pc2 -> exti2
   SYSCFG->EXTICR[0] &= ~(0xF << (4 * 2));
   SYSCFG->EXTICR[0] |= 2 << (4 * 2);

   EXTI->IMR |= (1 << 0) | (1 << 1) | (1 << 2);
   EXTI->FTSR |= (1 << 0) | (1 << 1) | (1 << 2);

   // thiết lập mức ưu tiên ngắt trong nvic theo thứ tự: mức ưu tiên của sw3 > mức ưu tiên của sw2 > mức ưu tiên của sw1
   NVIC_SetPriority(EXTI2_IRQn, 0); // sw3 cao nhất
   NVIC_SetPriority(EXTI1_IRQn, 1); // sw2
   NVIC_SetPriority(EXTI0_IRQn, 2); // sw1 thấp nhất

   NVIC_EnableIRQ(EXTI0_IRQn);
   NVIC_EnableIRQ(EXTI1_IRQn);
   NVIC_EnableIRQ(EXTI2_IRQn);
}

void EXTI0_IRQHandler(void)
{
   if (EXTI->PR & (1 << 0))
   {
      EXTI->PR = (1 << 0); // write 1 to clear pending bit
      for (int i = 0; i < 2; i++)
      {
         GPIOB->ODR |= (1 << 13);
         delay_loop(500000);
         GPIOB->ODR &= ~(1 << 13);
         delay_loop(500000);
      }
   }
}

void EXTI1_IRQHandler(void)
{
   if (EXTI->PR & (1 << 1))
   {
      EXTI->PR = (1 << 1);
      for (int i = 0; i < 4; i++)
      {
         GPIOB->ODR |= (1 << 14);
         delay_loop(500000);
         GPIOB->ODR &= ~(1 << 14);
         delay_loop(500000);
      }
   }
}

void EXTI2_IRQHandler(void)
{
   if (EXTI->PR & (1 << 2))
   {
      EXTI->PR = (1 << 2);
      for (int i = 0; i < 6; i++)
      {
         GPIOB->ODR |= (1 << 15);
         delay_loop(500000);
         GPIOB->ODR &= ~(1 << 15);
         delay_loop(500000);
      }
   }
}

int main(void)
{
   init_button();
   init_led();
   init_interrupt();

   while (1)
   {
   }
}
