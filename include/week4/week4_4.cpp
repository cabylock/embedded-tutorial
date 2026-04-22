#include "stm32f4xx_hal.h"

/*
 * Week 4 - Exercise 4
 * Input Capture with HC-SR04
 * - Trigger: PB6 (output)
 * - Echo:    PA0 (TIM2_CH1 input capture)
 */

volatile uint8_t waiting_falling_edge = 0;
volatile uint8_t measurement_ready = 0;
volatile uint32_t t_rise = 0;
volatile uint32_t t_fall = 0;
volatile uint32_t duration_us = 0;
volatile uint32_t distance_cm = 0;

void init_led_pa5(void)
{
   RCC->AHB1ENR |= (1 << 0);
   GPIOA->MODER &= ~(3 << (2 * 5));
   GPIOA->MODER |= (1 << (2 * 5));
}

void init_hcsr04_pins(void)
{
   RCC->AHB1ENR |= (1 << 0);

   /* PA0 -> TIM2_CH1 (AF1) */
   GPIOA->MODER &= ~(3 << (2 * 0));
   GPIOA->MODER |= (2 << (2 * 0));
   GPIOA->AFR[0] &= ~(0xF << (4 * 0));
   GPIOA->AFR[0] |= (0x1 << (4 * 0));

   /* PA1 -> Trigger output */
   GPIOA->MODER &= ~(3 << (2 * 1));
   GPIOA->MODER |= (1 << (2 * 1));
   GPIOA->BSRR = (1 << (1 + 16));
}

void tim2_init_input_capture(void)
{
   RCC->APB1ENR |= (1 << 0);

   TIM2->CR1 = 0;
   TIM2->PSC = 16 - 1; /* 16 MHz / 16 = 1 MHz -> 1 us/tick */
   TIM2->ARR = 0xFFFFFFFF;
   TIM2->CNT = 0;

   TIM2->CCMR1 &= ~(3 << 0);
   TIM2->CCMR1 |= (1 << 0); /* CC1 mapped to TI1 */

   TIM2->CCER &= ~((1 << 1) | (1 << 3)); /* Rising edge */
   TIM2->CCER |= (1 << 0);

   TIM2->DIER |= (1 << 1);

   TIM2->EGR = (1 << 0);
   TIM2->SR = 0;

   NVIC_EnableIRQ(TIM2_IRQn);

   TIM2->CR1 |= (1 << 0);
}

void hcsr04_send_trigger(void)
{
   uint32_t start;

   GPIOA->BSRR = (1 << 1);
   start = TIM2->CNT;

   while ((uint32_t)(TIM2->CNT - start) < 10)
   {
   }

   GPIOA->BSRR = (1 << (1 + 16));
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

         /* Next capture on falling edge. */
         TIM2->CCER |= (1 << 1);
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

         waiting_falling_edge = 0;
         measurement_ready = 1;

         /* Return to rising edge capture. */
         TIM2->CCER &= ~(1 << 1);
      }
   }
}

int main(void)
{
   HAL_Init();

   init_led_pa5();
   init_hcsr04_pins();
   tim2_init_input_capture();

   while (1)
   {
      hcsr04_send_trigger();
      HAL_Delay(60);

      if (measurement_ready)
      {
         measurement_ready = 0;

         /* Simple visual feedback: object closer than 20 cm -> LED ON */
         if (distance_cm < 20)
         {
            GPIOA->BSRR = (1 << 5);
         }
         else
         {
            GPIOA->BSRR = (1 << (5 + 16));
         }
      }
   }
}
