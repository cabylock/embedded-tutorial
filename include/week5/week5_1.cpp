#include<stm32f4xx.h>



void USART2_SendChar(char c)
{
   while ((USART2->SR & (1 << 7)) == 0);

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
   RCC->AHB1ENR |= (1 << 0); //GPIOA
   RCC->APB1ENR |= (1 << 17); //USART2

   GPIOA->MODER |= (2 << (2 * 2)); //PA2 alternate function
   GPIOA->AFR[0] |= (7 << (4 * 2)); //AF7 for USART2

   USART2->BRR = (104 << 4) | 3 ; // baud rate 9600 bps

   USART2->CR1 = (1 << 3) | (1 << 2) | (1 << 13);
}


int main(void)
{
   USART2_Init();

   while (1)
   {
      USART2_SendString("Hello, USART2!\r\n");
      for (volatile uint32_t i = 0; i < 10000; i++);
   }
}
