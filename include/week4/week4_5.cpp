#include "stm32f4xx_hal.h"

/*
 * Week 4 - Exercise 5
 * TIM2_CH1 PWM Mode 1 on PA5
 * PWM frequency: 1 kHz
 * Duty cycle: 0% -> 100% -> 0%
 */

void init_pa5_tim2_ch1(void)
{
   RCC->AHB1ENR |= (1 << 0);

   GPIOA->MODER &= ~(3 << (2 * 5));
   GPIOA->MODER |= (2 << (2 * 5)); /* Alternate function */

   GPIOA->AFR[0] &= ~(0xF << (4 * 5));
   GPIOA->AFR[0] |= (0x1 << (4 * 5)); /* AF1 = TIM2 */
}

void tim2_init_pwm_1khz(void)
{
   RCC->APB1ENR |= (1 << 0);

   TIM2->CR1 = 0;
   TIM2->PSC = 16 - 1;   /* 16 MHz / 16 = 1 MHz */
   TIM2->ARR = 1000 - 1; /* 1 MHz / 1000 = 1 kHz */
   TIM2->CCR1 = 0;

   TIM2->CCMR1 &= ~((3 << 0) | (7 << 4));
   TIM2->CCMR1 |= (6 << 4); /* PWM mode 1 */
   TIM2->CCMR1 |= (1 << 3);

   TIM2->CCER |= (1 << 0);
   TIM2->CR1 |= (1 << 7);

   TIM2->EGR = (1 << 0);
   TIM2->CR1 |= (1 << 0);
}

void set_pwm_duty_percent(uint32_t duty_percent)
{
   if (duty_percent > 100)
   {
      duty_percent = 100;
   }

   TIM2->CCR1 = ((TIM2->ARR + 1) * duty_percent) / 100;
}

int main(void)
{
   HAL_Init();

   init_pa5_tim2_ch1();
   tim2_init_pwm_1khz();

   int32_t duty = 0;
   int32_t step = 2;

   while (1)
   {
      set_pwm_duty_percent((uint32_t)duty);
      HAL_Delay(15);

      duty += step;

      if (duty >= 100)
      {
         duty = 100;
         step = -2;
      }
      else if (duty <= 0)
      {
         duty = 0;
         step = 2;
      }
   }
}
