#include "stm32f4xx_hal.h"

/*
 * Week 4 - Exercise 6
 * - Continuous distance update using Input Capture (TIM2_CH1, PA0)
 * - PWM LED brightness control (TIM3_CH1, PA6)
 *   Closer object -> brighter LED
 */

volatile uint8_t waiting_falling_edge = 0;
volatile uint8_t measurement_ready = 0;
volatile uint32_t t_rise = 0;
volatile uint32_t t_fall = 0;
volatile uint32_t duration_us = 0;
volatile uint32_t distance_cm = 0;

void init_hcsr04_pins(void)
{
   RCC->AHB1ENR |= (1 << 0) | (1 << 1);

   /* PA0 -> TIM2_CH1 (Echo) */
   GPIOA->MODER &= ~(3 << (2 * 0));
   GPIOA->MODER |= (2 << (2 * 0));
   GPIOA->AFR[0] &= ~(0xF << (4 * 0));
   GPIOA->AFR[0] |= (0x1 << (4 * 0));

   /* PB6 -> Trigger output */
   GPIOB->MODER &= ~(3 << (2 * 6));
   GPIOB->MODER |= (1 << (2 * 6));
   GPIOB->BSRR = (1 << (6 + 16));
}

void init_pwm_led_pa6(void)
{
   RCC->AHB1ENR |= (1 << 0);

   /* PA6 -> TIM3_CH1 (AF2) */
   GPIOA->MODER &= ~(3 << (2 * 6));
   GPIOA->MODER |= (2 << (2 * 6));
   GPIOA->AFR[0] &= ~(0xF << (4 * 6));
   GPIOA->AFR[0] |= (0x2 << (4 * 6));
}

void tim2_init_input_capture(void)
{
   RCC->APB1ENR |= (1 << 0);

   TIM2->CR1 = 0;
   TIM2->PSC = 16 - 1; /* 1 MHz -> 1 us resolution */
   TIM2->ARR = 0xFFFFFFFF;
   TIM2->CNT = 0;

   TIM2->CCMR1 &= ~((3 << 0));
   TIM2->CCMR1 |= (1 << 0); /* CC1 from TI1 */

   TIM2->CCER &= ~((1 << 1) | (1 << 3)); /* Rising edge */
   TIM2->CCER |= (1 << 0);

   TIM2->DIER |= (1 << 1);
   TIM2->EGR = (1 << 0);
   TIM2->SR = 0;

   NVIC_EnableIRQ(TIM2_IRQn);

   TIM2->CR1 |= (1 << 0);
}

void tim3_init_pwm_1khz(void)
{
   RCC->APB1ENR |= (1 << 1);

   TIM3->CR1 = 0;
   TIM3->PSC = 16 - 1;   /* 1 MHz */
   TIM3->ARR = 1000 - 1; /* 1 kHz */
   TIM3->CCR1 = 0;

   TIM3->CCMR1 &= ~((3 << 0) | (7 << 4));
   TIM3->CCMR1 |= (6 << 4); /* PWM mode 1 */
   TIM3->CCMR1 |= (1 << 3);

   TIM3->CCER |= (1 << 0);
   TIM3->CR1 |= (1 << 7);

   TIM3->EGR = (1 << 0);
   TIM3->CR1 |= (1 << 0);
}

void hcsr04_send_trigger(void)
{
   uint32_t start;

   GPIOB->BSRR = (1 << 6);
   start = TIM2->CNT;

   while ((uint32_t)(TIM2->CNT - start) < 10)
   {
   }

   GPIOB->BSRR = (1 << (6 + 16));
}

uint32_t distance_to_duty_percent(uint32_t d_cm)
{
   const uint32_t near_cm = 5;
   const uint32_t far_cm = 50;

   if (d_cm <= near_cm)
   {
      return 100;
   }

   if (d_cm >= far_cm)
   {
      return 0;
   }

   return ((far_cm - d_cm) * 100) / (far_cm - near_cm);
}

void set_pwm_duty_percent(uint32_t duty_percent)
{
   if (duty_percent > 100)
   {
      duty_percent = 100;
   }

   TIM3->CCR1 = ((TIM3->ARR + 1) * duty_percent) / 100;
}

void TIM2_IRQHandler(void)
{
   if (TIM2->SR & (1 << 1))
   {
      uint32_t captured = TIM2->CCR1;
      TIM2->SR &= ~(1 << 1);

      if (waiting_falling_edge == 0)
      {
         t_rise = captured;
         waiting_falling_edge = 1;
         TIM2->CCER |= (1 << 1); /* Falling edge next */
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
            duration_us = (0xFFFFFFFF - t_rise) + t_fall + 1;
         }

         distance_cm = (uint32_t)((float)duration_us / 2.0f / 29.412f);
         measurement_ready = 1;

         waiting_falling_edge = 0;
         TIM2->CCER &= ~(1 << 1); /* Back to rising edge */
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

         measurement_ready = 0;
         duty = distance_to_duty_percent(distance_cm);
         set_pwm_duty_percent(duty);
      }
   }
}
