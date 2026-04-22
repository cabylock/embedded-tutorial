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
volatile uint8_t state_elapsed_s = 0;
volatile uint8_t state_duration_s = 5;
volatile uint8_t pedestrian_waiting = 0;
volatile uint32_t global_seconds = 0;

void set_lights(uint8_t state)
{
   /* Turn all off first. */
   GPIOA->BSRR = (1 << (5 + 16)) | (1 << (6 + 16)) | (1 << (7 + 16));

   if (state == STATE_GREEN)
   {
      GPIOA->BSRR = (1 << 7);
   }
   else if (state == STATE_YELLOW)
   {
      GPIOA->BSRR = (1 << 6);
   }
   else
   {
      GPIOA->BSRR = (1 << 5);
   }
}

void init_traffic_leds(void)
{
   RCC->AHB1ENR |= (1 << 0);

   GPIOA->MODER &= ~((3 << (2 * 5)) | (3 << (2 * 6)) | (3 << (2 * 7)));
   GPIOA->MODER |= (1 << (2 * 5)) | (1 << (2 * 6)) | (1 << (2 * 7));

   set_lights(STATE_GREEN);
}

void init_pedestrian_button_pc13(void)
{
   RCC->AHB1ENR |= (1 << 2);
   RCC->APB2ENR |= (1 << 14);

   GPIOC->MODER &= ~(3 << (2 * 13));
   GPIOC->PUPDR &= ~(3 << (2 * 13));

   SYSCFG->EXTICR[3] &= ~(0xF << 4);
   SYSCFG->EXTICR[3] |= (0x2 << 4); /* EXTI13 <- PC13 */

   EXTI->IMR |= (1 << 13);
   EXTI->FTSR |= (1 << 13);
   EXTI->RTSR &= ~(1 << 13);

   NVIC_EnableIRQ(EXTI15_10_IRQn);
}

void tim3_init_1s_interrupt(void)
{
   RCC->APB1ENR |= (1 << 1);

   TIM3->CR1 = 0;
   TIM3->PSC = 16000 - 1; /* 1 kHz */
   TIM3->ARR = 1000 - 1;  /* 1 second */
   TIM3->CNT = 0;
   TIM3->DIER |= (1 << 0);
   TIM3->EGR = (1 << 0);
   TIM3->SR &= ~(1 << 0);

   NVIC_EnableIRQ(TIM3_IRQn);

   TIM3->CR1 |= (1 << 0);
}

void EXTI15_10_IRQHandler(void)
{
   if (EXTI->PR & (1 << 13))
   {
      EXTI->PR = (1 << 13);

      if (traffic_state == STATE_GREEN)
      {
         /* Interrupt green immediately, then run yellow for safety. */
         traffic_state = STATE_YELLOW;
         state_elapsed_s = 0;
         state_duration_s = 2;
         set_lights(traffic_state);
      }
      else
      {
         /* Remember the request, apply +3 s when entering next red state. */
         pedestrian_waiting = 1;
      }
   }
}

void TIM3_IRQHandler(void)
{
   if (TIM3->SR & (1 << 0))
   {
      TIM3->SR &= ~(1 << 0);

      global_seconds++;
      state_elapsed_s++;

      if (state_elapsed_s >= state_duration_s)
      {
         state_elapsed_s = 0;

         if (traffic_state == STATE_GREEN)
         {
            traffic_state = STATE_YELLOW;
            state_duration_s = 2;
         }
         else if (traffic_state == STATE_YELLOW)
         {
            traffic_state = STATE_RED;
            state_duration_s = 5;

            if (pedestrian_waiting)
            {
               state_duration_s += 3;
               pedestrian_waiting = 0;
            }
         }
         else
         {
            traffic_state = STATE_GREEN;
            state_duration_s = 5;
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
