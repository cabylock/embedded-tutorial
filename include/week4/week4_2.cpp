#include "stm32f4xx_hal.h"

/*
 * Week 4 - Exercise 2
 * TIM3 interrupt every 500 ms:
 * - PA5 toggles every 1 second in TIM3 IRQ
 * - PB7 keeps blinking in main loop with software delay
 */

volatile uint16_t pb7_delay_ms_value = 80U;

void init_leds(void)
{
   RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN;

   GPIOA->MODER &= ~(3U << (2U * 5U));
   GPIOA->MODER |= (1U << (2U * 5U));

   GPIOB->MODER &= ~(3U << (2U * 7U));
   GPIOB->MODER |= (1U << (2U * 7U));
}

void tim2_init_delay_1ms(void)
{
   RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

   TIM2->CR1 = 0;
   TIM2->PSC = 16000U - 1U; /* 1 kHz */
   TIM2->ARR = 1U - 1U;     /* update every 1 ms */
   TIM2->CNT = 0;
   TIM2->EGR = TIM_EGR_UG;
   TIM2->SR &= ~TIM_SR_UIF;
   TIM2->CR1 |= TIM_CR1_CEN;
}

void delay_ms(uint32_t ms)
{
   while (ms--)
   {
      while ((TIM2->SR & TIM_SR_UIF) == 0U)
      {
      }

      TIM2->SR &= ~TIM_SR_UIF;
   }
}

void tim3_init_interrupt_500ms(void)
{
   RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

   TIM3->CR1 = 0;
   TIM3->PSC = 16000U - 1U; /* 1 kHz */
   TIM3->ARR = 500U - 1U;   /* 500 ms update */
   TIM3->CNT = 0;
   TIM3->DIER |= TIM_DIER_UIE;
   TIM3->EGR = TIM_EGR_UG;
   TIM3->SR &= ~TIM_SR_UIF;

   NVIC_EnableIRQ(TIM3_IRQn);

   TIM3->CR1 |= TIM_CR1_CEN;
}

void TIM3_IRQHandler(void)
{
   uint8_t tick_500ms = 0U;

   if (TIM3->SR & TIM_SR_UIF)
   {
      TIM3->SR &= ~TIM_SR_UIF;

      tick_500ms++;
      if (tick_500ms >= 2U)
      {
         tick_500ms = 0U;
         GPIOA->ODR ^= (1U << 5U); /* PA5 toggles every 1 s */

         /* Change PB7 blink speed to show CPU can be busy in main loop. */
         pb7_delay_ms_value = (pb7_delay_ms_value == 80U) ? 260U : 80U;
      }
   }
}

int main(void)
{
   init_leds();
   tim2_init_delay_1ms();
   tim3_init_interrupt_500ms();

   while (1)
   {
      GPIOB->ODR ^= (1U << 7U);
      delay_ms(pb7_delay_ms_value);
   }
}
