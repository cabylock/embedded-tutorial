#include "stm32f4xx_hal.h"


int main(void)
{
   RCC->AHB1ENR |= 1 << 2; // enable gpioc clock
   RCC->AHB1ENR |= 1 << 1; // enable gpiob clock

   GPIOC->MODER &= ~(3 << (2*5)); // set pc5 to input mode
   GPIOC->PUPDR &= ~(3 << (2*5)); // no pull-up, no pull-down for pc5

   GPIOC->MODER &= ~(3 << (2*6)); // set pc6 to input mode
   GPIOC->PUPDR &= ~(3 << (2*6)); // no pull-up, no pull-down for pc6

   GPIOC->MODER &= ~(3 << (2*8)); // set pc8 to input mode
   GPIOC->PUPDR &= ~(3 << (2*8)); // no pull-up, no pull-down for pc8



   GPIOB->MODER |= 1 << (2*13); // set pb13 to output mode

   GPIOB->MODER |= 1 << (2*14); // set pb14 to output mode

   GPIOB->MODER |= 1 << (2*15); // set pb15 to output mode


   uint16_t is_pressed_1;
   uint16_t is_pressed_2;
   uint16_t is_pressed_3;


   while(1)
   {       is_pressed_1 = GPIOC->IDR & (1<<5); // read the state of the first button
           is_pressed_2 = GPIOC->IDR & (1<<6); // read the state of the second button
           is_pressed_3 = GPIOC->IDR & (1<<8); // read the state of the third button

           if (is_pressed_1 == 0) { // if the first button is pressed
               GPIOB -> BSRR = 1 << 13; // set pb13 high
           }
           else 
           {
               GPIOB -> BSRR = 1 << (13+16); // set pb13 low
           }

           if (is_pressed_2 == 0) { // if the second button is pressed
               GPIOB -> BSRR = 1 << 14; // set pb14 high
           }
           else 
           {
               GPIOB -> BSRR = 1 << (14+16); // set pb14 low
           }

           if (is_pressed_3 == 0) { // if the third button is pressed
               GPIOB -> BSRR = 1 << 15; // set pb15 high
           }
           else 
           {
               GPIOB -> BSRR = 1 << (15+16); // set pb15 low
           }
   }
}