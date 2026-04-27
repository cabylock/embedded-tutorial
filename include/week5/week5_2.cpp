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
   RCC->AHB1ENR |= (1 << 0);  // Enable GPIOA clock
   RCC->APB1ENR |= (1 << 17); // Enable USART2 clock

   GPIOA->MODER &= ~(3 << (2 * 2)); // Clear mode PA2
   GPIOA->MODER |= (2 << (2 * 2));  // PA2 alternate function Mode

   GPIOA->AFR[0] &= ~(0xF << (4 * 2)); // Clear alternate function PA2
   GPIOA->AFR[0] |= (7 << (4 * 2));    // AF7 for USART2 TX on PA2

   USART2->BRR = (104 << 4) | 3; // baud rate 9600 bps

   USART2->CR1 = (1 << 3) | (1 << 2) | (1 << 13); // Enable TX, RX, and USART
}

void IR_Sensor_Init(void)
{
   RCC->AHB1ENR |= (1 << 2); // Enable clock for GPIOC

   GPIOC->MODER &= ~(3 << (2 * 1)); // Set PC1 as input mode (00)
   GPIOC->PUPDR &= ~(3 << (2 * 1)); // Clear pull-up/pull-down bits
   GPIOC->PUPDR |= (1 << (2 * 1));  // Set pull-up for PC1 (helps stabilize reading)
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

      // Delay
      for (volatile uint32_t i = 0; i < 500000; i++)
         ;
   }
}
