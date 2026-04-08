#include "stm32f4xx_hal.h"

// Cấu hình SW1, SW2, SW3 trên PC0, PC1, PC2
// LED 1, 2, 3 trên PB13, PB14, PB15

static void delay_loop(uint32_t count)
{
   for (volatile uint32_t i = 0; i < count; i++);
}

void init_button(void)
{
   RCC->AHB1ENR |= 1 << 2; // clock for GPIOC

   // SW1 on PC0
   GPIOC->MODER &= ~(3 << (2 * 0));
   GPIOC->PUPDR &= ~(3 << (2 * 0));


   // SW2 on PC1
   GPIOC->MODER &= ~(3 << (2 * 1));
   GPIOC->PUPDR &= ~(3 << (2 * 1));


   // SW3 on PC2
   GPIOC->MODER &= ~(3 << (2 * 2));
   GPIOC->PUPDR &= ~(3 << (2 * 2));

}

void init_led(void)
{
   RCC->AHB1ENR |= 1 << 1; // clock for GPIOB

   // LED 1 on PB13
   GPIOB->MODER &= ~(3 << (2 * 13));
   GPIOB->MODER |= 1 << (2 * 13);

   // LED 2 on PB14
   GPIOB->MODER &= ~(3 << (2 * 14));
   GPIOB->MODER |= 1 << (2 * 14);

   // LED 3 on PB15
   GPIOB->MODER &= ~(3 << (2 * 15));
   GPIOB->MODER |= 1 << (2 * 15);
}

void init_interrupt(void)
{
   RCC->APB2ENR |= 1 << 14; 

   // SW1 -> PC0 -> EXTI0
   SYSCFG->EXTICR[0] &= ~(0 << (4 * 0));
   SYSCFG->EXTICR[0] |= 2 << (4 * 0); // 2 means GPIOC

   // SW2 -> PC1 -> EXTI1
   SYSCFG->EXTICR[0] &= ~(0 << (4 * 1));
   SYSCFG->EXTICR[0] |= 2 << (4 * 1); 

   // SW3 -> PC2 -> EXTI2
   SYSCFG->EXTICR[0] &= ~(0 << (4 * 2));
   SYSCFG->EXTICR[0] |= 2 << (4 * 2); 

   EXTI->IMR |= (1 << 0) | (1 << 1) | (1 << 2);
   EXTI->FTSR |= (1 << 0) | (1 << 1) | (1 << 2);

   // Thiết lập mức ưu tiên ngắt trong NVIC theo thứ tự: Mức ưu tiên của SW3 > Mức ưu tiên của SW2 > Mức ưu tiên của SW1
   NVIC_SetPriority(EXTI2_IRQn, 0); // SW3 cao nhất
   NVIC_SetPriority(EXTI1_IRQn, 1); // SW2
   NVIC_SetPriority(EXTI0_IRQn, 2); // SW1 thấp nhất

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
      for(int i = 0; i < 6; i++)
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
