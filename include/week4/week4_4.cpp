#include "stm32f4xx_hal.h"

/*
 * Week 4 - Exercise 4
 * Input Capture with HC-SR04
 * - Trigger: PB6 (output)
 * - Echo:    PA0 (TIM2_CH1 input capture)
 */

volatile uint8_t waiting_falling_edge = 0U;
volatile uint8_t measurement_ready = 0U;
volatile uint32_t t_rise = 0U;
volatile uint32_t t_fall = 0U;
volatile uint32_t duration_us = 0U;
volatile uint32_t distance_cm = 0U;

void init_led_pa5(void)
{
   RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
   GPIOA->MODER &= ~(3U << (2U * 5U));
   GPIOA->MODER |= (1U << (2U * 5U));
}

void init_hcsr04_pins(void)
{
   RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN;

   /* PA0 -> TIM2_CH1 (AF1) */
   GPIOA->MODER &= ~(3U << (2U * 0U));
   GPIOA->MODER |= (2U << (2U * 0U));
   GPIOA->AFR[0] &= ~(0xFU << (4U * 0U));
   GPIOA->AFR[0] |= (0x1U << (4U * 0U));

   /* PB6 -> Trigger output */
   GPIOB->MODER &= ~(3U << (2U * 6U));
   GPIOB->MODER |= (1U << (2U * 6U));
   GPIOB->BSRR = (1U << (6U + 16U));
}

void tim2_init_input_capture(void)
{
   RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

   TIM2->CR1 = 0;
   TIM2->PSC = 16U - 1U; /* 16 MHz / 16 = 1 MHz -> 1 us/tick */
   TIM2->ARR = 0xFFFFFFFFU;
   TIM2->CNT = 0;

   TIM2->CCMR1 &= ~(TIM_CCMR1_CC1S);
   TIM2->CCMR1 |= TIM_CCMR1_CC1S_0; /* CC1 mapped to TI1 */

   TIM2->CCER &= ~(TIM_CCER_CC1P | TIM_CCER_CC1NP); /* Rising edge */
   TIM2->CCER |= TIM_CCER_CC1E;

   TIM2->DIER |= TIM_DIER_CC1IE;

   TIM2->EGR = TIM_EGR_UG;
   TIM2->SR = 0;

   NVIC_EnableIRQ(TIM2_IRQn);

   TIM2->CR1 |= TIM_CR1_CEN;
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

         /* Next capture on falling edge. */
         TIM2->CCER |= TIM_CCER_CC1P;
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

         waiting_falling_edge = 0U;
         measurement_ready = 1U;

         /* Return to rising edge capture. */
         TIM2->CCER &= ~TIM_CCER_CC1P;
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
         measurement_ready = 0U;

         /* Simple visual feedback: object closer than 20 cm -> LED ON */
         if (distance_cm < 20U)
         {
            GPIOA->BSRR = (1U << 5U);
         }
         else
         {
            GPIOA->BSRR = (1U << (5U + 16U));
         }
      }
   }
}
