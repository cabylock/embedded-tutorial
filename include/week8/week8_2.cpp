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

void IR_Sensor_Init(void)
{
   RCC->AHB1ENR |= (1 << 2); // enable clock for gpioc

   GPIOC->MODER &= ~(3 << (2 * 1)); // set pc1 as input mode (00)
   GPIOC->PUPDR &= ~(3 << (2 * 1)); // clear pull-up/pull-down bits
   GPIOC->PUPDR |= (1 << (2 * 1));  // set pull-up for pc1 (helps stabilize reading)
}

int main(void)
{
   USART2_Init();
   IR_Sensor_Init();

   while (1)
   {

      if ((GPIOC->IDR & (1 << 1)) == 0)
      {
         USART2_SendString("Co vat can\r\n");
      }
      else
      {
         USART2_SendString("Khong co vat\r\n");
      }

      // delay
      for (volatile uint32_t i = 0; i < 500000; i++)
         ;
   }
}
