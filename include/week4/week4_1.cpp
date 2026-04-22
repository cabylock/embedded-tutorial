#include "stm32f4xx_hal.h"

/*
 * Week 4 - Exercise 1
 * TIM2 polling mode (UIF flag):
 * - Upcounter mode: LED PA5 toggles every 1s
 * - Downcounter mode: LED PA5 toggles every 2s
 */

void init_led_pa5(void)
{
   RCC->AHB1ENR |= (1 << 0);

   GPIOA->MODER &= ~(3 << (2 * 5));
   GPIOA->MODER |= (1 << (2 * 5));
}

void toggle_led_pa5(void)
{
   GPIOA->ODR ^= (1 << 5);
}

void tim2_config_period_ms(uint32_t period_ms, uint8_t downcounter)
{
   RCC->APB1ENR |= (1 << 0);

   TIM2->CR1 = 0;
   TIM2->PSC = 16000 - 1; /* 16 MHz / 16000 = 1 kHz -> 1 ms/tick */
   TIM2->ARR = period_ms - 1;

   if (downcounter)
   {
      TIM2->CR1 |= (1 << 4);
      TIM2->CNT = TIM2->ARR;
   }
   else
   {
      TIM2->CR1 &= ~(1 << 4);
      TIM2->CNT = 0;
   }

   TIM2->EGR = (1 << 0);
   TIM2->SR &= ~(1 << 0);
   TIM2->CR1 |= (1 << 0);
}

void delay_period_tim2(void)
{
   while ((TIM2->SR & (1 << 0)) == 0)
   {
   }

   TIM2->SR &= ~(1 << 0);
}

int main(void)
{
   init_led_pa5();

   while (1)
   {
      tim2_config_period_ms(1000, 0);
      for (uint8_t i = 0; i < 5; i++)
      {
         delay_period_tim2();
         toggle_led_pa5();
      }

      tim2_config_period_ms(2000, 1);
      for (uint8_t i = 0; i < 5; i++)
      {
         delay_period_tim2();
         toggle_led_pa5();
      }
   }
}
