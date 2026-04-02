#include "stm32f4xx_hal.h"


int main(void)
{
   RCC->AHB1ENR |= 1 << 2; // enable GPIOC clock

   GPIOC->MODER |= 1 << (2*6); // set PC6 to output mode
   GPIOC->MODER |= 0 << (2*5); // set PC5 to input mode
   GPIOC->PUPDR |= 0 << (2*5); // no pull-up, no pull-down for PC5


   uint16_t is_pressed;
   while(1)
   {       is_pressed = GPIOC->IDR & (1<<5); // read the state of the button
           if (is_pressed == 0) { // if the button is pressed
               GPIOC -> BSRR = 1 << 6; // set PC6 high
           }
           else 
           {
               GPIOC -> BSRR = 1 << (6+16); // set PC6 low
           }



   }






}
