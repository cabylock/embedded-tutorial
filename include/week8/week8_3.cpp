#include <stm32f4xx.h>

void USART2_SendChar(char c)
{
   while ((USART2->SR & (1 << 7)) == 0)
      ;
   USART2->DR = c;
}

void USART2_SendString(const char *str)
{
   while (*str)
   {
      USART2_SendChar(*str++);
   }
}

void USART2_Init(void)
{
   RCC->AHB1ENR |= (1 << 0);  // enable gpioa clock
   RCC->APB1ENR |= (1 << 17); // enable usart2 clock

   GPIOA->MODER &= ~(3 << (2 * 2)); // clear mode pa2
   GPIOA->MODER |= (2 << (2 * 2));  // pa2 alternate function mode

   GPIOA->AFR[0] &= ~(0xF << (4 * 2)); // clear alternate function pa2
   GPIOA->AFR[0] |= (7 << (4 * 2));    // af7 for usart2 tx on pa2

   USART2->BRR = (104 << 4) | 3; // baud rate 9600 bps

   USART2->CR1 = (1 << 3) | (1 << 2) | (1 << 13); // enable tx, rx, and usart
}

void Button_Init(void)
{
   RCC->AHB1ENR |= (1 << 2); // enable clock for gpioc

   GPIOC->MODER &= ~(3 << (2 * 1)); // set pc1 as input mode (00)
   GPIOC->PUPDR &= ~(3 << (2 * 1)); // clear pull-up/pull-down bits
   GPIOC->PUPDR |= (1 << (2 * 1));  // set pull-up for pc1 (helps stabilize reading)
}

void LED_Init(void)
{
   RCC->AHB1ENR |= (1 << 1); // enable clock for gpiob

   GPIOB->MODER &= ~(3 << (2 * 0));
   GPIOB->MODER |= (1 << (2 * 0)); // set pb0 as output mode (01)
}

int main(void)
{
   USART2_Init();
   Button_Init();
   LED_Init();

   int mode = 0;
   uint8_t btn_prev = 1;
   uint32_t delay_counter = 0;

   USART2_SendString("MODE = 0: Den tat\r\n");
   GPIOB->ODR &= ~(1 << 0); // turn off led

   while (1)
   {
      uint8_t btn_curr = (GPIOC->IDR & (1 << 1)) ? 1 : 0;

      if (btn_prev == 1 && btn_curr == 0)
      {
         for (volatile uint32_t i = 0; i < 50000; i++)
            ; // delay for debounce

         if ((GPIOC->IDR & (1 << 1)) == 0)
         {
            mode++;
            if (mode > 3)
            {
               mode = 0;
            }

            switch (mode)
            {
            case 0:
               USART2_SendString("MODE = 0: Den tat\r\n");
               GPIOB->ODR &= ~(1 << 0); // off
               break;
            case 1:
               USART2_SendString("MODE = 1: Den bat\r\n");
               GPIOB->ODR |= (1 << 0); // on
               break;
            case 2:
               USART2_SendString("MODE = 2: Den nhap nhay cham\r\n");
               break;
            case 3:
               USART2_SendString("MODE = 3: Den nhap nhay nhanh\r\n");
               break;
            }
         }
      }
      btn_prev = btn_curr;

      // mode2
      if (mode == 2)
      {
         delay_counter++;
         if (delay_counter >= 300000)
         {
            GPIOB->ODR ^= (1 << 0);
            delay_counter = 0;
         }
      }
      else if (mode == 3)
      {
         delay_counter++;
         if (delay_counter >= 60000)
         {
            GPIOB->ODR ^= (1 << 0);
            delay_counter = 0;
         }
      }
      else
      {
         delay_counter = 0;
      }
   }
}
