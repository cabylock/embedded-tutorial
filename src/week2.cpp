#include"week2.h"
#include "stm32f4xx_hal.h"


void week2_1(void)
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


void week2_2(void)
{
   RCC->AHB1ENR |= 1 << 2; // enable GPIOC clock
   RCC->AHB1ENR |= 1 << 1; // enable GPIOB clock

   GPIOC->MODER |= 0 << (2*5); // set PC5 to input mode
   GPIOC->PUPDR |= 0 << (2*5); // no pull-up, no pull-down for PC5

   GPIOC->MODER |= 0 << (2*6); // set PC6 to input mode
   GPIOC->PUPDR |= 0 << (2*6); // no pull-up, no pull-down for PC6

   GPIOC->MODER |= 0 << (2*8); // set PC8 to input mode
   GPIOC->PUPDR |= 0 << (2*8); // no pull-up, no pull-down for PC8



   GPIOB->MODER |= 1 << (2*13); // set PB13 to output mode

   GPIOB->MODER |= 1 << (2*14); // set PB14 to output mode

   GPIOB->MODER |= 1 << (2*15); // set PB15 to output mode


   uint16_t is_pressed_1;
   uint16_t is_pressed_2;
   uint16_t is_pressed_3;


   while(1)
   {       is_pressed_1 = GPIOC->IDR & (1<<5); // read the state of the first button
           is_pressed_2 = GPIOC->IDR & (1<<6); // read the state of the second button
           is_pressed_3 = GPIOC->IDR & (1<<8); // read the state of the third button

           if (is_pressed_1 == 0) { // if the first button is pressed
               GPIOB -> BSRR = 1 << 13; // set PB13 high
           }
           else 
           {
               GPIOB -> BSRR = 1 << (13+16); // set PB13 low
           }

           if (is_pressed_2 == 0) { // if the second button is pressed
               GPIOB -> BSRR = 1 << 14; // set PB14 high
           }
           else 
           {
               GPIOB -> BSRR = 1 << (14+16); // set PB14 low
           }

           if (is_pressed_3 == 0) { // if the third button is pressed
               GPIOB -> BSRR = 1 << 15; // set PB15 high
           }
           else 
           {
               GPIOB -> BSRR = 1 << (15+16); // set PB15 low
           }
   }
}