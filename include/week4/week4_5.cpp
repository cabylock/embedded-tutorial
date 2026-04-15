#include "stm32f4xx_hal.h"

/*
 * Week 4 - Exercise 5
 * TIM2_CH1 PWM Mode 1 on PA5
 * PWM frequency: 1 kHz
 * Duty cycle: 0% -> 100% -> 0%
 */

void init_pa5_tim2_ch1(void)
{
   RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

   GPIOA->MODER &= ~(3U << (2U * 5U));
   GPIOA->MODER |= (2U << (2U * 5U)); /* Alternate function */

   GPIOA->AFR[0] &= ~(0xFU << (4U * 5U));
   GPIOA->AFR[0] |= (0x1U << (4U * 5U)); /* AF1 = TIM2 */
}

void tim2_init_pwm_1khz(void)
{
   RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

   TIM2->CR1 = 0;
   TIM2->PSC = 16U - 1U;   /* 16 MHz / 16 = 1 MHz */
   TIM2->ARR = 1000U - 1U; /* 1 MHz / 1000 = 1 kHz */
   TIM2->CCR1 = 0;

   TIM2->CCMR1 &= ~(TIM_CCMR1_CC1S | TIM_CCMR1_OC1M);
   TIM2->CCMR1 |= (6U << 4U); /* PWM mode 1 */
   TIM2->CCMR1 |= TIM_CCMR1_OC1PE;

   TIM2->CCER |= TIM_CCER_CC1E;
   TIM2->CR1 |= TIM_CR1_ARPE;

   TIM2->EGR = TIM_EGR_UG;
   TIM2->CR1 |= TIM_CR1_CEN;
}

void set_pwm_duty_percent(uint32_t duty_percent)
{
   if (duty_percent > 100U)
   {
      duty_percent = 100U;
   }

   TIM2->CCR1 = ((TIM2->ARR + 1U) * duty_percent) / 100U;
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
