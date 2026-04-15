#include "stm32f4xx_hal.h"

/*
 * Week 4 - Exercise 1
 * TIM2 polling mode (UIF flag):
 * - Upcounter mode: LED PA5 toggles every 1s
 * - Downcounter mode: LED PA5 toggles every 2s
 */

void init_led_pa5(void)
{
   RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

   GPIOA->MODER &= ~(3U << (2U * 5U));
   GPIOA->MODER |= (1U << (2U * 5U));
}

void toggle_led_pa5(void)
{
   GPIOA->ODR ^= (1U << 5U);
}

void tim2_config_period_ms(uint32_t period_ms, uint8_t downcounter)
{
   RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

   TIM2->CR1 = 0;
   TIM2->PSC = 16000U - 1U; /* 16 MHz / 16000 = 1 kHz -> 1 ms/tick */
   TIM2->ARR = period_ms - 1U;

   if (downcounter)
   {
      TIM2->CR1 |= TIM_CR1_DIR;
      TIM2->CNT = TIM2->ARR;
   }
   else
   {
      TIM2->CR1 &= ~TIM_CR1_DIR;
      TIM2->CNT = 0;
   }

   TIM2->EGR = TIM_EGR_UG;
   TIM2->SR &= ~TIM_SR_UIF;
   TIM2->CR1 |= TIM_CR1_CEN;
}

void delay_period_tim2(void)
{
   while ((TIM2->SR & TIM_SR_UIF) == 0U)
   {
   }

   TIM2->SR &= ~TIM_SR_UIF;
}

int main(void)
{
   init_led_pa5();

   while (1)
   {
      tim2_config_period_ms(1000U, 0U);
      for (uint8_t i = 0; i < 5U; i++)
      {
         delay_period_tim2();
         toggle_led_pa5();
      }

      tim2_config_period_ms(2000U, 1U);
      for (uint8_t i = 0; i < 5U; i++)
      {
         delay_period_tim2();
         toggle_led_pa5();
      }
   }
}
