#include "stm32f4xx_hal.h"

/*
 * Week 4 - Exercise 3
 * Smart traffic light:
 * - Green (PA7): 5 s
 * - Yellow (PA6): 2 s
 * - Red (PA5): 5 s
 * - Pedestrian button: PC13
 */

enum
{
   STATE_GREEN = 0,
   STATE_YELLOW,
   STATE_RED
};

volatile uint8_t traffic_state = STATE_GREEN;
volatile uint8_t state_elapsed_s = 0U;
volatile uint8_t state_duration_s = 5U;
volatile uint8_t pedestrian_waiting = 0U;
volatile uint32_t global_seconds = 0U;

void set_lights(uint8_t state)
{
   /* Turn all off first. */
   GPIOA->BSRR = (1U << (5U + 16U)) | (1U << (6U + 16U)) | (1U << (7U + 16U));

   if (state == STATE_GREEN)
   {
      GPIOA->BSRR = (1U << 7U);
   }
   else if (state == STATE_YELLOW)
   {
      GPIOA->BSRR = (1U << 6U);
   }
   else
   {
      GPIOA->BSRR = (1U << 5U);
   }
}

void init_traffic_leds(void)
{
   RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

   GPIOA->MODER &= ~((3U << (2U * 5U)) | (3U << (2U * 6U)) | (3U << (2U * 7U)));
   GPIOA->MODER |= (1U << (2U * 5U)) | (1U << (2U * 6U)) | (1U << (2U * 7U));

   set_lights(STATE_GREEN);
}

void init_pedestrian_button_pc13(void)
{
   RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
   RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

   GPIOC->MODER &= ~(3U << (2U * 13U));
   GPIOC->PUPDR &= ~(3U << (2U * 13U));

   SYSCFG->EXTICR[3] &= ~(0xFU << 4U);
   SYSCFG->EXTICR[3] |= (0x2U << 4U); /* EXTI13 <- PC13 */

   EXTI->IMR |= (1U << 13U);
   EXTI->FTSR |= (1U << 13U);
   EXTI->RTSR &= ~(1U << 13U);

   NVIC_EnableIRQ(EXTI15_10_IRQn);
}

void tim3_init_1s_interrupt(void)
{
   RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

   TIM3->CR1 = 0;
   TIM3->PSC = 16000U - 1U; /* 1 kHz */
   TIM3->ARR = 1000U - 1U;  /* 1 second */
   TIM3->CNT = 0;
   TIM3->DIER |= TIM_DIER_UIE;
   TIM3->EGR = TIM_EGR_UG;
   TIM3->SR &= ~TIM_SR_UIF;

   NVIC_EnableIRQ(TIM3_IRQn);

   TIM3->CR1 |= TIM_CR1_CEN;
}

void EXTI15_10_IRQHandler(void)
{
   if (EXTI->PR & (1U << 13U))
   {
      EXTI->PR = (1U << 13U);

      if (traffic_state == STATE_GREEN)
      {
         /* Interrupt green immediately, then run yellow for safety. */
         traffic_state = STATE_YELLOW;
         state_elapsed_s = 0U;
         state_duration_s = 2U;
         set_lights(traffic_state);
      }
      else
      {
         /* Remember the request, apply +3 s when entering next red state. */
         pedestrian_waiting = 1U;
      }
   }
}

void TIM3_IRQHandler(void)
{
   if (TIM3->SR & TIM_SR_UIF)
   {
      TIM3->SR &= ~TIM_SR_UIF;

      global_seconds++;
      state_elapsed_s++;

      if (state_elapsed_s >= state_duration_s)
      {
         state_elapsed_s = 0U;

         if (traffic_state == STATE_GREEN)
         {
            traffic_state = STATE_YELLOW;
            state_duration_s = 2U;
         }
         else if (traffic_state == STATE_YELLOW)
         {
            traffic_state = STATE_RED;
            state_duration_s = 5U;

            if (pedestrian_waiting)
            {
               state_duration_s += 3U;
               pedestrian_waiting = 0U;
            }
         }
         else
         {
            traffic_state = STATE_GREEN;
            state_duration_s = 5U;
         }

         set_lights(traffic_state);
      }
   }
}

int main(void)
{
   init_traffic_leds();
   init_pedestrian_button_pc13();
   tim3_init_1s_interrupt();

   while (1)
   {
   }
}
