#include "stm32f4xx_hal.h"

volatile int32_t led_offset=1;
#define LED_INITIAL 13

void init_button(void)
{
   RCC->AHB1ENR |= 1 << 2; // clock for GPIOC

   GPIOC->MODER &= ~(3 << (2 * 5));
   GPIOC->PUPDR &= ~(3 << (2 * 5));

   GPIOC->MODER &= ~(3 << (2 * 6));
   GPIOC->PUPDR &= ~(3 << (2 * 6));

   GPIOC->MODER &= ~(3 << (2 * 13));
   GPIOC->PUPDR &= ~(3 << (2 * 13));
}

void init_led(void)
{
   RCC->AHB1ENR |= 1 << 1; // clock for GPIOB

   GPIOB->MODER |= 1 << (2 * 13);
   GPIOB->MODER |= 1 << (2 * 14);
   GPIOB->MODER |= 1 << (2 * 15);
}

void init_interrupt(void)
{
   RCC->APB2ENR |= 1 << 14; // enable SYSCFG clock

   SYSCFG->EXTICR[(5 / 4)] |= 2 << (4 * (5 % 4)); // EXTI5 <- PC5 (SW1)
   SYSCFG->EXTICR[(6 / 4)] |= 2 << (4 * (6 % 4)); // EXTI6 <- PC6 (SW2)
   SYSCFG->EXTICR[(13 / 4)] |= 2 << (4 * (13 % 4)); // EXTI13 <- PC13 (SW3)

   EXTI->IMR |= (1 << 5) | (1 << 6) | (1 << 13);
   EXTI->FTSR |= (1 << 5) | (1 << 6) | (1 << 13);
   NVIC_EnableIRQ(EXTI9_5_IRQn);
   NVIC_EnableIRQ(EXTI15_10_IRQn);
}

void EXTI9_5_IRQHandler(void)
{
   if (EXTI->PR & (1 << 5))
   { // SW1: vòng thuận LED1 -> LED2 -> LED3 -> LED1
      EXTI->PR |= (1 << 5);
      led_offset = 1;

   }
   if (EXTI->PR & (1 << 6))
   { // SW2: vòng ngược LED1 -> LED3 -> LED2 -> LED1
      EXTI->PR |= (1 << 6);
      led_offset = -1;
   }
}

void EXTI15_10_IRQHandler(void)
{
   if (EXTI->PR & (1 << 13))
   { // SW3: reset
      EXTI->PR |= (1 << 13);
      led_offset = 0;
   }
}


int main (void)
{
   init_button();
   init_led();
   init_interrupt();

   uint32_t current_led = LED_INITIAL; 
   while (1)
   {
      GPIOB->ODR |= 1 << current_led;
      for (volatile int i = 0; i < 500000; i++);
      GPIOB->ODR &= ~(1 << current_led);
      current_led = LED_INITIAL + (current_led - LED_INITIAL + led_offset + 3) % 3; 
   }
}
