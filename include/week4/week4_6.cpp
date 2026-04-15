#include "stm32f4xx_hal.h"

/*
 * Week 4 - Exercise 6
 * - Continuous distance update using Input Capture (TIM2_CH1, PA0)
 * - PWM LED brightness control (TIM3_CH1, PA6)
 *   Closer object -> brighter LED
 */

volatile uint8_t waiting_falling_edge = 0U;
volatile uint8_t measurement_ready = 0U;
volatile uint32_t t_rise = 0U;
volatile uint32_t t_fall = 0U;
volatile uint32_t duration_us = 0U;
volatile uint32_t distance_cm = 0U;

void init_hcsr04_pins(void)
{
   RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN;

   /* PA0 -> TIM2_CH1 (Echo) */
   GPIOA->MODER &= ~(3U << (2U * 0U));
   GPIOA->MODER |= (2U << (2U * 0U));
   GPIOA->AFR[0] &= ~(0xFU << (4U * 0U));
   GPIOA->AFR[0] |= (0x1U << (4U * 0U));

   /* PB6 -> Trigger output */
   GPIOB->MODER &= ~(3U << (2U * 6U));
   GPIOB->MODER |= (1U << (2U * 6U));
   GPIOB->BSRR = (1U << (6U + 16U));
}

void init_pwm_led_pa6(void)
{
   RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

   /* PA6 -> TIM3_CH1 (AF2) */
   GPIOA->MODER &= ~(3U << (2U * 6U));
   GPIOA->MODER |= (2U << (2U * 6U));
   GPIOA->AFR[0] &= ~(0xFU << (4U * 6U));
   GPIOA->AFR[0] |= (0x2U << (4U * 6U));
}

void tim2_init_input_capture(void)
{
   RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

   TIM2->CR1 = 0;
   TIM2->PSC = 16U - 1U; /* 1 MHz -> 1 us resolution */
   TIM2->ARR = 0xFFFFFFFFU;
   TIM2->CNT = 0;

   TIM2->CCMR1 &= ~(TIM_CCMR1_CC1S);
   TIM2->CCMR1 |= TIM_CCMR1_CC1S_0; /* CC1 from TI1 */

   TIM2->CCER &= ~(TIM_CCER_CC1P | TIM_CCER_CC1NP); /* Rising edge */
   TIM2->CCER |= TIM_CCER_CC1E;

   TIM2->DIER |= TIM_DIER_CC1IE;
   TIM2->EGR = TIM_EGR_UG;
   TIM2->SR = 0;

   NVIC_EnableIRQ(TIM2_IRQn);

   TIM2->CR1 |= TIM_CR1_CEN;
}

void tim3_init_pwm_1khz(void)
{
   RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

   TIM3->CR1 = 0;
   TIM3->PSC = 16U - 1U;   /* 1 MHz */
   TIM3->ARR = 1000U - 1U; /* 1 kHz */
   TIM3->CCR1 = 0;

   TIM3->CCMR1 &= ~(TIM_CCMR1_CC1S | TIM_CCMR1_OC1M);
   TIM3->CCMR1 |= (6U << 4U); /* PWM mode 1 */
   TIM3->CCMR1 |= TIM_CCMR1_OC1PE;

   TIM3->CCER |= TIM_CCER_CC1E;
   TIM3->CR1 |= TIM_CR1_ARPE;

   TIM3->EGR = TIM_EGR_UG;
   TIM3->CR1 |= TIM_CR1_CEN;
}

void hcsr04_send_trigger(void)
{
   uint32_t start;

   GPIOB->BSRR = (1U << 6U);
   start = TIM2->CNT;

   while ((uint32_t)(TIM2->CNT - start) < 10U)
   {
   }

   GPIOB->BSRR = (1U << (6U + 16U));
}

uint32_t distance_to_duty_percent(uint32_t d_cm)
{
   const uint32_t near_cm = 5U;
   const uint32_t far_cm = 50U;

   if (d_cm <= near_cm)
   {
      return 100U;
   }

   if (d_cm >= far_cm)
   {
      return 0U;
   }

   return ((far_cm - d_cm) * 100U) / (far_cm - near_cm);
}

void set_pwm_duty_percent(uint32_t duty_percent)
{
   if (duty_percent > 100U)
   {
      duty_percent = 100U;
   }

   TIM3->CCR1 = ((TIM3->ARR + 1U) * duty_percent) / 100U;
}

void TIM2_IRQHandler(void)
{
   if (TIM2->SR & TIM_SR_CC1IF)
   {
      uint32_t captured = TIM2->CCR1;
      TIM2->SR &= ~TIM_SR_CC1IF;

      if (waiting_falling_edge == 0U)
      {
         t_rise = captured;
         waiting_falling_edge = 1U;
         TIM2->CCER |= TIM_CCER_CC1P; /* Falling edge next */
      }
      else
      {
         t_fall = captured;

         if (t_fall >= t_rise)
         {
            duration_us = t_fall - t_rise;
         }
         else
         {
            duration_us = (0xFFFFFFFFU - t_rise) + t_fall + 1U;
         }

         distance_cm = (uint32_t)((float)duration_us / 2.0f / 29.412f);
         measurement_ready = 1U;

         waiting_falling_edge = 0U;
         TIM2->CCER &= ~TIM_CCER_CC1P; /* Back to rising edge */
      }
   }
}

int main(void)
{
   HAL_Init();

   init_hcsr04_pins();
   init_pwm_led_pa6();
   tim2_init_input_capture();
   tim3_init_pwm_1khz();

   while (1)
   {
      hcsr04_send_trigger();
      HAL_Delay(60);

      if (measurement_ready)
      {
         uint32_t duty;

         measurement_ready = 0U;
         duty = distance_to_duty_percent(distance_cm);
         set_pwm_duty_percent(duty);
      }
   }
}
