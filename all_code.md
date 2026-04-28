### File: week1\week1_1.cpp

```cpp
#include "stm32f4xx_hal.h"


int main(void)
{
	RCC->AHB1ENR |= 1 << 0;
	GPIOA->MODER |= 1 << (2*5);


	while(1)
	{      
        
        
        GPIOA -> BSRR = 1 << 5;
        for(volatile uint32_t i = 0; i < 100000; i++);
        GPIOA -> BSRR = 1 << (5+16);
        for(volatile uint32_t i = 0; i < 100000; i++);

	}
}
```

### File: week1\week1_2.cpp

```cpp
#include "stm32f4xx_hal.h"

int main(void)
{
	RCC->AHB1ENR |= 1 << 0;
	GPIOA->MODER |= 1 << (2*5);

    RCC->AHB1ENR |= 1 << 2;
    GPIOC->MODER &= ~(3 << (2*13)); 
    GPIOC->PUPDR &= ~(3 << (2*13));
    
    uint16_t is_pressed; 
	while(1)
	{      
        is_pressed = GPIOC->IDR & (1<<13);
        if (is_pressed == 0) {

            GPIOA -> BSRR = 1 << 5;
        }
        else 
        {
            GPIOA -> BSRR = 1 << (5+16);

        }
		
	}
}
```

### File: week2\week2_1.cpp

```cpp
#include "stm32f4xx_hal.h"


int main(void)
{
   RCC->AHB1ENR |= 1 << 2; // enable gpioc clock

   GPIOC->MODER |= 1 << (2*6); // set pc6 to output mode
   GPIOC->MODER &= ~(3 << (2*5)); // set pc5 to input mode
   GPIOC->PUPDR &= ~(3 << (2*5)); // no pull-up, no pull-down for pc5


   uint16_t is_pressed;
   while(1)
   {       is_pressed = GPIOC->IDR & (1<<5); // read the state of the button
           if (is_pressed == 0) { // if the button is pressed
               GPIOC -> BSRR = 1 << 6; // set pc6 high
           }
           else 
           {
               GPIOC -> BSRR = 1 << (6+16); // set pc6 low
           }



   }






}

```

### File: week2\week2_2.cpp

```cpp
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
```

### File: week3\week3_1.cpp

```cpp
#include "stm32f4xx_hal.h"

// debounce ms
#define DEBOUNCE_DELAY 50

// pins
#define SW1_PIN 0 // pa0
#define SW2_PIN 1 // pa1
#define LED_PIN 0 // pb0

volatile uint8_t last_sw1_state = 0;
volatile uint8_t last_sw2_state = 0;


// init gpio
void init_gpio(void)
{
    // enable clocks
    RCC->AHB1ENR |= (1 << 0) | (1 << 1); // gen clocks

    // sw1 input 
    GPIOA->MODER &= ~(3 << (2 * SW1_PIN)); // clear mode
    GPIOA->PUPDR &= ~(3 << (2 * SW1_PIN)); // clear pupd
    GPIOA->PUPDR |= (2 << (2 * SW1_PIN));  // set pd

    // sw2 input 
    GPIOA->MODER &= ~(3 << (2 * SW2_PIN)); // clear mode
    GPIOA->PUPDR &= ~(3 << (2 * SW2_PIN)); // clear pupd
    GPIOA->PUPDR |= (2 << (2 * SW2_PIN));  // set pd

    // led output
    GPIOB->MODER &= ~(3 << (2 * LED_PIN));  // clear mode
    GPIOB->MODER |= (1 << (2 * LED_PIN));   // output mode
    GPIOB->OTYPER &= ~(1 << LED_PIN);       // push-pull
    GPIOB->OSPEEDR |= (3 << (2 * LED_PIN)); // high speed
}

// delay
void delay_ms(uint32_t ms)
{
    for (volatile uint32_t i = 0; i < ms * 1000; i++)
        ;
}

// debounce switch
uint8_t read_debounced_switch(uint8_t pin, volatile uint8_t *last_state)
{
    uint8_t current_state = (GPIOA->IDR & (1 << pin)) ? 1 : 0;

    // check state change
    if (current_state != *last_state)
    {
        delay_ms(DEBOUNCE_DELAY);
        current_state = (GPIOA->IDR & (1 << pin)) ? 1 : 0;

        if (current_state != *last_state)
        {
            *last_state = current_state;
            return (current_state == 1);
        }
    }

    return 0;
}

int main(void)
{
    // init gpio
    init_gpio();

    // led off
    GPIOB->BSRR = (1 << (LED_PIN + 16)); // led off

    while (1)
    {
        // read switches
        uint8_t sw1_pressed = read_debounced_switch(SW1_PIN, &last_sw1_state);
        uint8_t sw2_pressed = read_debounced_switch(SW2_PIN, &last_sw2_state);

        // led on if both pressed
        if (sw1_pressed && sw2_pressed)
        {
            GPIOB->BSRR = (1 << LED_PIN); // led on
        }
        else
        {
            GPIOB->BSRR = (1 << (LED_PIN + 16)); // led off
        }

        // delay
        delay_ms(10);
    }
}

```

### File: week3\week3_2.cpp

```cpp
#include "stm32f4xx_hal.h"

// debounce timing
#define DEBOUNCE_DELAY 50

// pin definitions
#define SW1_PIN 0  // pa0
#define LED1_PIN 0 // pb0 (lsb)
#define LED2_PIN 1 // pb1
#define LED3_PIN 2 // pb2 (msb)

// global variables
volatile uint8_t last_sw1_state = 0;
volatile uint8_t counter = 0;


// init gpio
void init_gpio(void)
{
    // enable clock for gpioa and gpiob
    RCC->AHB1ENR |= (1 << 0) | (1 << 1); // gpioa and gpiob

    // configure sw1 (pa0) as input with pull-down
    GPIOA->MODER &= ~(3 << (2 * SW1_PIN)); // clear mode bits
    GPIOA->PUPDR &= ~(3 << (2 * SW1_PIN)); // clear pull-up/down
    GPIOA->PUPDR |= (2 << (2 * SW1_PIN));  // set pull-down

    // configure leds (pb0, pb1, pb2) as outputs
    GPIOB->MODER &= ~((3 << (2 * LED1_PIN)) | (3 << (2 * LED2_PIN)) | (3 << (2 * LED3_PIN)));
    GPIOB->MODER |= ((1 << (2 * LED1_PIN)) | (1 << (2 * LED2_PIN)) | (1 << (2 * LED3_PIN)));
    GPIOB->OTYPER &= ~((1 << LED1_PIN) | (1 << LED2_PIN) | (1 << LED3_PIN));                   // push-pull
    GPIOB->OSPEEDR |= ((3 << (2 * LED1_PIN)) | (3 << (2 * LED2_PIN)) | (3 << (2 * LED3_PIN))); // high speed
}

// simple delay function
void delay_ms(uint32_t ms)
{
    for (volatile uint32_t i = 0; i < ms * 1000; i++)
        ;
}

// update led states based on counter value
void update_leds(uint8_t value)
{
    // led1 (pb0) = bit 0
    if (value & 0x01)
    {
        GPIOB->BSRR = (1 << LED1_PIN); // turn on
    }
    else
    {
        GPIOB->BSRR = (1 << (LED1_PIN + 16)); // turn off
    }

    // led2 (pb1) = bit 1
    if (value & 0x02)
    {
        GPIOB->BSRR = (1 << LED2_PIN); // turn on
    }
    else
    {
        GPIOB->BSRR = (1 << (LED2_PIN + 16)); // turn off
    }

    // led3 (pb2) = bit 2
    if (value & 0x04)
    {
        GPIOB->BSRR = (1 << LED3_PIN); // turn on
    }
    else
    {
        GPIOB->BSRR = (1 << (LED3_PIN + 16)); // turn off
    }
}

// debounce function for switches
uint8_t read_debounced_switch(uint8_t pin, volatile uint8_t *last_state)
{
    uint8_t current_state = (GPIOA->IDR & (1 << pin)) ? 1 : 0;

    // check if state has changed
    if (current_state != *last_state)
    {
        delay_ms(DEBOUNCE_DELAY);
        current_state = (GPIOA->IDR & (1 << pin)) ? 1 : 0;

        if (current_state != *last_state)
        {
            *last_state = current_state;
            return (current_state == 1);
        }
    }

    return 0; 
}

int main(void)
{
    // init gpio
    init_gpio();

    // initial state
    update_leds(0);

    while (1)
    {
        // check for switch press
        if (read_debounced_switch(SW1_PIN, &last_sw1_state))
        {
            // increment counter (0-7, then wrap to 0)
            counter = (counter + 1) & 0x07;

            // update leds to show current count
            update_leds(counter);
        }

        // small delay
        delay_ms(10);
    }
}


```

### File: week3\week3_3.cpp

```cpp
#include "stm32f4xx_hal.h"

// debounce timing
#define DEBOUNCE_DELAY 50

// pin definitions
#define SW1_PIN 0 // pa0
#define LED_PIN 0 // pb0

// global variables
volatile uint8_t last_sw1_state = 0;
volatile uint8_t counter = 0;


// init gpio
void init_gpio(void)
{
    // enable clock for gpioa and gpiob
    RCC->AHB1ENR |= (1 << 0) | (1 << 1); // gpioa and gpiob

    //  sw1 (pa0)  input  pull-down
    GPIOA->MODER &= ~(3 << (2 * SW1_PIN)); // clear mode bits
    GPIOA->PUPDR &= ~(3 << (2 * SW1_PIN)); // clear pull-up/down
    GPIOA->PUPDR |= (2 << (2 * SW1_PIN));  // set pull-down

    //  led (pb0)  output
    GPIOB->MODER &= ~(3 << (2 * LED_PIN));  // clear mode bits
    GPIOB->MODER |= (1 << (2 * LED_PIN));   // set as output
    GPIOB->OTYPER &= ~(1 << LED_PIN);       // push-pull output
    GPIOB->OSPEEDR |= (3 << (2 * LED_PIN)); // high speed
}

// simple delay function
void delay_ms(uint32_t ms)
{
    for (volatile uint32_t i = 0; i < ms * 1000; i++)
        ;
}

// blink led specified number of times
void blink_led(uint8_t times)
{
    for (uint8_t i = 0; i < times; i++)
    {
        // turn on led
        GPIOB->BSRR = (1 << LED_PIN);
        delay_ms(200); // led on for 200ms

        // turn off led
        GPIOB->BSRR = (1 << (LED_PIN + 16));
        delay_ms(200); // led off for 200ms
    }
}

// debounce function for switches
uint8_t read_debounced_switch(uint8_t pin, volatile uint8_t *last_state)
{
    uint8_t current_state = (GPIOA->IDR & (1 << pin)) ? 1 : 0;

    // check if state has changed
    if (current_state != *last_state)
    {
        delay_ms(DEBOUNCE_DELAY);
        current_state = (GPIOA->IDR & (1 << pin)) ? 1 : 0;

        if (current_state != *last_state)
        {
            *last_state = current_state;
            return (current_state == 1);
        }
    }

    return 0; 
}

int main(void)
{
    // initialize gpio
    init_gpio();

    // initial state: led off
    GPIOB->BSRR = (1 << (LED_PIN + 16)); // turn off led

    while (1)
    {
        // check for switch press
        if (read_debounced_switch(SW1_PIN, &last_sw1_state))
        {
            // increment counter (1-5, then wrap to 1)
            counter++;
            if (counter > 5)
            {
                counter = 1;
            }

            // blink led counter times
            blink_led(counter);
        }

        // small delay to prevent excessive cpu usage
        delay_ms(10);
    }
}

```

### File: week4_5\week4_5_1.cpp

```cpp
#include "stm32f4xx_hal.h"

void init_button(void)
{
   RCC->AHB1ENR |= 1 << 2; //clock for gpioc

   GPIOC->MODER &= ~(3 << (2 * 13));
   GPIOC->PUPDR &= ~(3 << (2 * 13));

   GPIOC->MODER &= ~(3 << (2 * 8));
   GPIOC->PUPDR &= ~(3 << (2 * 8));
}

void init_led(void)
{
   RCC->AHB1ENR |= 1 << 0; //clock for gpioa
   GPIOA->MODER |= 1 << (2 * 5);
}

void init_interrupt(void)
{
   RCC->APB2ENR |= 1 << 14;                         // enable syscfg clock

   SYSCFG->EXTICR[(13 / 4)] |= 2 << (4 * (13 % 4)); // set exti13 to be connected to pc13
   EXTI->IMR |= 1 << 13;                            // unmask exti13
   EXTI->FTSR |= 1 << 13;                           // trigger on falling edge for exti13
   NVIC_EnableIRQ(EXTI15_10_IRQn);                  // enable exti15_10 interrupt in nvic

   SYSCFG->EXTICR[(8 / 4)] |= 2 << (4 * (8 % 4));   // set exti8 to be connected to pc8
   EXTI->IMR |= 1 << 8;                             // unmask exti8
   EXTI->FTSR |= 1 << 8;                            // trigger on falling edge for exti8
   NVIC_EnableIRQ(EXTI9_5_IRQn);                    // enable exti9_5 interrupt in nvic
}

void EXTI15_10_IRQHandler(void)
{
   if (EXTI->PR & (1 << 13))
   {                                 // check if the interrupt is from exti13
      EXTI->PR = (1 << 13);          // write 1 to clear pending bit
      GPIOA->BSRR = (1 << (5 + 16)); // turn on led 
      for(volatile int i = 0; i < 100000; i++);

   }
}

void EXTI9_5_IRQHandler(void)
{
   if (EXTI->PR & (1 << 8))
   {                        // check if the interrupt is from exti8
      EXTI->PR = (1 << 8);  // write 1 to clear pending bit
      GPIOA->BSRR = 1 << 5; // turn off led 
      for(volatile int i = 0; i < 100000; i++);
     
   }
}

int main(void)
{
   init_button();
   init_led();
   init_interrupt();
   while (1)
   {
      GPIOA->ODR ^= 1 << 5;
      for (volatile int i = 0; i < 100000; i++);
   }
}
```

### File: week4_5\week4_5_2.cpp

```cpp
#include "stm32f4xx_hal.h"

volatile int32_t led_offset=1;
#define LED_INITIAL 13

void init_button(void)
{
   RCC->AHB1ENR |= 1 << 2; // clock for gpioc

   GPIOC->MODER &= ~(3 << (2 * 5));
   GPIOC->PUPDR &= ~(3 << (2 * 5));

   GPIOC->MODER &= ~(3 << (2 * 6));
   GPIOC->PUPDR &= ~(3 << (2 * 6));

   GPIOC->MODER &= ~(3 << (2 * 13));
   GPIOC->PUPDR &= ~(3 << (2 * 13));
}

void init_led(void)
{
   RCC->AHB1ENR |= 1 << 1; // clock for gpiob

   GPIOB->MODER |= 1 << (2 * 13);
   GPIOB->MODER |= 1 << (2 * 14);
   GPIOB->MODER |= 1 << (2 * 15);
}

void init_interrupt(void)
{
   RCC->APB2ENR |= 1 << 14; // enable syscfg clock

   SYSCFG->EXTICR[(5 / 4)] |= 2 << (4 * (5 % 4)); // exti5 <- pc5 (sw1)
   SYSCFG->EXTICR[(6 / 4)] |= 2 << (4 * (6 % 4)); // exti6 <- pc6 (sw2)
   SYSCFG->EXTICR[(13 / 4)] |= 2 << (4 * (13 % 4)); // exti13 <- pc13 (sw3)

   EXTI->IMR |= (1 << 5) | (1 << 6) | (1 << 13);
   EXTI->FTSR |= (1 << 5) | (1 << 6) | (1 << 13);
   NVIC_EnableIRQ(EXTI9_5_IRQn);
   NVIC_EnableIRQ(EXTI15_10_IRQn);
}

void EXTI9_5_IRQHandler(void)
{
   if (EXTI->PR & (1 << 5))
   { // sw1: vòng thuận led1 -> led2 -> led3 -> led1
      EXTI->PR = (1 << 5);
      led_offset = 1;

   }
   if (EXTI->PR & (1 << 6))
   { // sw2: vòng ngược led1 -> led3 -> led2 -> led1
      EXTI->PR = (1 << 6);
      led_offset = -1;
   }
}

void EXTI15_10_IRQHandler(void)
{
   if (EXTI->PR & (1 << 13))
   { // sw3: reset
      EXTI->PR = (1 << 13);
      led_offset = 0;
   }
}


int main (void)
{
   init_button();
   init_led();
   init_interrupt();

   uint32_t current_led = LED_INITIAL; 
   while (1)
   {
      GPIOB->ODR |= 1 << current_led;
      for (volatile int i = 0; i < 500000; i++);
      GPIOB->ODR &= ~(1 << current_led);
      current_led = LED_INITIAL + (current_led - LED_INITIAL + led_offset + 3) % 3; 
   }
}

```

### File: week4_5\week4_5_3.cpp

```cpp
#include "stm32f4xx_hal.h"

// cấu hình sw1, sw2, sw3 trên pc0, pc1, pc2
// led 1, 2, 3 trên pb13, pb14, pb15

void delay_loop(uint32_t count)
{
   for (volatile uint32_t i = 0; i < count; i++)
      ;
}

void init_button(void)
{
   RCC->AHB1ENR |= 1 << 2; // clock for gpioc

   // sw1 on pc0
   GPIOC->MODER &= ~(3 << (2 * 0));
   GPIOC->PUPDR &= ~(3 << (2 * 0));

   // sw2 on pc1
   GPIOC->MODER &= ~(3 << (2 * 1));
   GPIOC->PUPDR &= ~(3 << (2 * 1));

   // sw3 on pc2
   GPIOC->MODER &= ~(3 << (2 * 2));
   GPIOC->PUPDR &= ~(3 << (2 * 2));
}

void init_led(void)
{
   RCC->AHB1ENR |= 1 << 1; // clock for gpiob

   // led 1 on pb13
   GPIOB->MODER &= ~(3 << (2 * 13));
   GPIOB->MODER |= 1 << (2 * 13);

   // led 2 on pb14
   GPIOB->MODER &= ~(3 << (2 * 14));
   GPIOB->MODER |= 1 << (2 * 14);

   // led 3 on pb15
   GPIOB->MODER &= ~(3 << (2 * 15));
   GPIOB->MODER |= 1 << (2 * 15);
}

void init_interrupt(void)
{
   RCC->APB2ENR |= 1 << 14;

   // sw1 -> pc0 -> exti0
   SYSCFG->EXTICR[0] &= ~(0xF << (4 * 0));
   SYSCFG->EXTICR[0] |= 2 << (4 * 0); // 2 means gpioc

   // sw2 -> pc1 -> exti1
   SYSCFG->EXTICR[0] &= ~(0xF << (4 * 1));
   SYSCFG->EXTICR[0] |= 2 << (4 * 1);

   // sw3 -> pc2 -> exti2
   SYSCFG->EXTICR[0] &= ~(0xF << (4 * 2));
   SYSCFG->EXTICR[0] |= 2 << (4 * 2);

   EXTI->IMR |= (1 << 0) | (1 << 1) | (1 << 2);
   EXTI->FTSR |= (1 << 0) | (1 << 1) | (1 << 2);

   // thiết lập mức ưu tiên ngắt trong nvic theo thứ tự: mức ưu tiên của sw3 > mức ưu tiên của sw2 > mức ưu tiên của sw1
   NVIC_SetPriority(EXTI2_IRQn, 0); // sw3 cao nhất
   NVIC_SetPriority(EXTI1_IRQn, 1); // sw2
   NVIC_SetPriority(EXTI0_IRQn, 2); // sw1 thấp nhất

   NVIC_EnableIRQ(EXTI0_IRQn);
   NVIC_EnableIRQ(EXTI1_IRQn);
   NVIC_EnableIRQ(EXTI2_IRQn);
}

void EXTI0_IRQHandler(void)
{
   if (EXTI->PR & (1 << 0))
   {
      EXTI->PR = (1 << 0); // write 1 to clear pending bit
      for (int i = 0; i < 2; i++)
      {
         GPIOB->ODR |= (1 << 13);
         delay_loop(500000);
         GPIOB->ODR &= ~(1 << 13);
         delay_loop(500000);
      }
   }
}

void EXTI1_IRQHandler(void)
{
   if (EXTI->PR & (1 << 1))
   {
      EXTI->PR = (1 << 1);
      for (int i = 0; i < 4; i++)
      {
         GPIOB->ODR |= (1 << 14);
         delay_loop(500000);
         GPIOB->ODR &= ~(1 << 14);
         delay_loop(500000);
      }
   }
}

void EXTI2_IRQHandler(void)
{
   if (EXTI->PR & (1 << 2))
   {
      EXTI->PR = (1 << 2);
      for (int i = 0; i < 6; i++)
      {
         GPIOB->ODR |= (1 << 15);
         delay_loop(500000);
         GPIOB->ODR &= ~(1 << 15);
         delay_loop(500000);
      }
   }
}

int main(void)
{
   init_button();
   init_led();
   init_interrupt();

   while (1)
   {
   }
}

```

### File: week6_7\week6_7_1.cpp

```cpp
#include "stm32f4xx_hal.h"


void init_led_pa5(void)
{
   RCC->AHB1ENR |= (1 << 0);

   GPIOA->MODER &= ~(3 << (2 * 5));
   GPIOA->MODER |= (1 << (2 * 5));
}

void toggle_led_pa5(void)
{
   GPIOA->ODR ^= (1 << 5);
}

void tim2_config_period_ms(uint32_t period_ms, uint8_t downcounter)
{
   RCC->APB1ENR |= (1 << 0);

   TIM2->CR1 = 0;
   TIM2->PSC = 16000 - 1; /* 16 mhz / 16000 = 1 khz -> 1 ms/tick */
   TIM2->ARR = period_ms - 1;

   if (downcounter)
   {
      TIM2->CR1 |= (1 << 4);
      TIM2->CNT = TIM2->ARR;
   }
   else
   {
      TIM2->CR1 &= ~(1 << 4);
      TIM2->CNT = 0;
   }

   TIM2->EGR = (1 << 0);
   TIM2->SR &= ~(1 << 0);
   TIM2->CR1 |= (1 << 0);
}

void delay_period_tim2(void)
{
   while ((TIM2->SR & (1 << 0)) == 0)
   {
   }

   TIM2->SR &= ~(1 << 0);
}

int main(void)
{
   init_led_pa5();

   while (1)
   {
      tim2_config_period_ms(1000, 0);
      for (uint8_t i = 0; i < 5; i++)
      {
         delay_period_tim2();
         toggle_led_pa5();
      }

      tim2_config_period_ms(2000, 1);
      for (uint8_t i = 0; i < 5; i++)
      {
         delay_period_tim2();
         toggle_led_pa5();
      }
   }
}

```

### File: week6_7\week6_7_2.cpp

```cpp
#include "stm32f4xx_hal.h"


void init_leds(void)
{
   RCC->AHB1ENR |= (1 << 0) | (1 << 1);

   GPIOA->MODER &= ~(3 << (2 * 5));
   GPIOA->MODER |= (1 << (2 * 5));

   GPIOB->MODER &= ~(3 << (2 * 7));
   GPIOB->MODER |= (1 << (2 * 7));
}

void delay_ms(uint32_t ms)
{
   RCC->APB1ENR |= (1 << 0); // enable tim2

   TIM2->PSC = 16000 - 1; // 1ms tick (16mhz / 16000 = 1khz)
   TIM2->ARR = ms - 1;

   TIM2->CNT = 0;
   TIM2->SR = 0; // clear flag

   TIM2->CR1 |= (1 << 0); // start

   while (!(TIM2->SR & (1 << 0)))
      ; // wait uif

   TIM2->CR1 &= ~(1 << 0); // stop timer
   TIM2->SR &= ~(1 << 0);  // clear flag
}

void tim3_init_1000ms(void)
{
   RCC->APB1ENR |= (1 << 1);

   TIM3->CR1 = 0;
   TIM3->PSC = 16000 - 1; /* 1 khz */
   TIM3->ARR = 1000 - 1;  /* 1000 ms update */
   TIM3->CNT = 0;
   TIM3->DIER |= (1 << 0);
   TIM3->EGR = (1 << 0);
   TIM3->SR &= ~(1 << 0);

   NVIC_EnableIRQ(TIM3_IRQn);

   TIM3->CR1 |= (1 << 0);
}

void TIM3_IRQHandler(void)
{

   if (TIM3->SR & (1 << 0))
   {
      TIM3->SR &= ~(1 << 0);

      GPIOA->ODR ^= (1 << 5); /* pa5 toggles every 1 s */
   }
}

int main(void)
{
   init_leds();
   tim3_init_1000ms();

   while (1)
   {
      GPIOB->ODR ^= (1 << 7);
      delay_ms(500);
   }
}

```

### File: week6_7\week6_7_3.cpp

```cpp
#include "stm32f4xx_hal.h"



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
   /* turn all off first. */
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
   SYSCFG->EXTICR[3] |= (0x2 << 4); /* exti13 <- pc13 */

   EXTI->IMR |= (1 << 13);
   EXTI->FTSR |= (1 << 13);
   EXTI->RTSR &= ~(1 << 13);

   NVIC_EnableIRQ(EXTI15_10_IRQn);
}

void tim3_init_1s_interrupt(void)
{
   RCC->APB1ENR |= (1 << 1);

   TIM3->CR1 = 0;
   TIM3->PSC = 16000 - 1; /* 1 khz */
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
         /* interrupt green immediately, then run yellow for safety. */
         traffic_state = STATE_YELLOW;
         state_elapsed_s = 0;
         state_duration_s = 2;
         set_lights(traffic_state);
      }
      else
      {
         /* remember the request, apply +3 s when entering next red state. */
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

```

### File: week6_7\week6_7_4.cpp

```cpp
#include "stm32f4xx_hal.h"



volatile uint8_t waiting_falling_edge = 0;
volatile uint8_t measurement_ready = 0;
volatile uint32_t t_rise = 0;
volatile uint32_t t_fall = 0;
volatile uint32_t duration_us = 0;
volatile uint32_t distance_cm = 0;

void init_led_pa5(void)
{
   RCC->AHB1ENR |= (1 << 0);
   GPIOA->MODER &= ~(3 << (2 * 5));
   GPIOA->MODER |= (1 << (2 * 5));
}

void init_hcsr04_pins(void)
{
   RCC->AHB1ENR |= (1 << 0);

   /* pa0 -> tim2_ch1 (af1) */
   GPIOA->MODER &= ~(3 << (2 * 0));
   GPIOA->MODER |= (2 << (2 * 0));
   GPIOA->AFR[0] &= ~(0xF << (4 * 0));
   GPIOA->AFR[0] |= (0x1 << (4 * 0));

   /* pa1 -> trigger output */
   GPIOA->MODER &= ~(3 << (2 * 1));
   GPIOA->MODER |= (1 << (2 * 1));
   GPIOA->BSRR = (1 << (1 + 16));
}

void tim2_init_input_capture(void)
{
   RCC->APB1ENR |= (1 << 0);

   TIM2->CR1 = 0;
   TIM2->PSC = 16 - 1; /* 16 mhz / 16 = 1 mhz -> 1 us/tick */
   TIM2->ARR = 0xFFFFFFFF;
   TIM2->CNT = 0;

   TIM2->CCMR1 &= ~(3 << 0);
   TIM2->CCMR1 |= (1 << 0); /* cc1 mapped to ti1 */

   TIM2->CCER &= ~((1 << 1) | (1 << 3)); /* rising edge */
   TIM2->CCER |= (1 << 0);

   TIM2->DIER |= (1 << 1);

   TIM2->EGR = (1 << 0);
   TIM2->SR = 0;

   NVIC_EnableIRQ(TIM2_IRQn);

   TIM2->CR1 |= (1 << 0);
}

void hcsr04_send_trigger(void)
{
   uint32_t start;

   GPIOA->BSRR = (1 << 1);
   start = TIM2->CNT;

   while ((uint32_t)(TIM2->CNT - start) < 10)
   {
   }

   GPIOA->BSRR = (1 << (1 + 16));
}

void TIM2_IRQHandler(void)
{
   if (TIM2->SR & (1 << 1))
   {
      uint32_t captured = TIM2->CCR1;
      TIM2->SR &= ~(1 << 1);

      if (waiting_falling_edge == 0)
      {
         t_rise = captured;
         waiting_falling_edge = 1;

         /* next capture on falling edge. */
         TIM2->CCER |= (1 << 1);
      }
      else
      {
         t_fall = captured;

         if (t_fall >= t_rise)
         {
            duration_us = t_fall - t_rise;
         }
         else
         {
            duration_us = (0xFFFFFFFF - t_rise) + t_fall + 1;
         }

         distance_cm = (uint32_t)((float)duration_us / 2.0f / 29.412f);

         waiting_falling_edge = 0;
         measurement_ready = 1;

         /* return to rising edge capture. */
         TIM2->CCER &= ~(1 << 1);
      }
   }
}

int main(void)
{
   HAL_Init();

   init_led_pa5();
   init_hcsr04_pins();
   tim2_init_input_capture();

   while (1)
   {
      hcsr04_send_trigger();
      HAL_Delay(60);

      if (measurement_ready)
      {
         measurement_ready = 0;

         /* simple visual feedback: object closer than 20 cm -> led on */
         if (distance_cm < 20)
         {
            GPIOA->BSRR = (1 << 5);
         }
         else
         {
            GPIOA->BSRR = (1 << (5 + 16));
         }
      }
   }
}

```

### File: week6_7\week6_7_5.cpp

```cpp
#include "stm32f4xx_hal.h"

void init_pa5_tim2_ch1(void)
{
   RCC->AHB1ENR |= (1 << 0);

   GPIOA->MODER &= ~(3 << (2 * 5));
   GPIOA->MODER |= (2 << (2 * 5)); /* alternate function */

   GPIOA->AFR[0] &= ~(0xF << (4 * 5));
   GPIOA->AFR[0] |= (0x1 << (4 * 5)); /* af1 = tim2 */
}

void tim2_init_pwm_1khz(void)
{
   RCC->APB1ENR |= (1 << 0);

   TIM2->CR1 = 0;
   TIM2->PSC = 16 - 1;   /* 16 mhz / 16 = 1 mhz */
   TIM2->ARR = 1000 - 1; /* 1 mhz / 1000 = 1 khz */
   TIM2->CCR1 = 0;

   TIM2->CCMR1 &= ~(3 << 0); 
   TIM2->CCMR1 &= ~(7 << 4);
   TIM2->CCMR1 |= (6 << 4); /* pwm mode 1 */
   TIM2->CCMR1 |= (1 << 3);

   TIM2->CCER |= (1 << 0);
   TIM2->CR1 |= (1 << 7);

   TIM2->EGR = (1 << 0);
   TIM2->CR1 |= (1 << 0);
}

void set_pwm_duty_percent(uint32_t duty_percent)
{
   if (duty_percent > 100)
   {
      duty_percent = 100;
   }

   TIM2->CCR1 = ((TIM2->ARR + 1) * duty_percent) / 100;
}

int main(void)
{
   HAL_Init();

   init_pa5_tim2_ch1();
   tim2_init_pwm_1khz();

   int32_t duty = 0;
   int32_t step = 2;

   while (1)
   {
      set_pwm_duty_percent((uint32_t)duty);
      for(volatile int i = 0 ; i<10000; i++);

      duty += step;

      if (duty >= 100)
      {
         duty = 100;
         step = -2;
      }
      else if (duty <= 0)
      {
         duty = 0;
         step = 2;
      }
   }
}

```

### File: week6_7\week6_7_6.cpp

```cpp
#include "stm32f4xx_hal.h"



volatile uint8_t waiting_falling_edge = 0;
volatile uint8_t measurement_ready = 0;
volatile uint32_t t_rise = 0;
volatile uint32_t t_fall = 0;
volatile uint32_t duration_us = 0;
volatile uint32_t distance_cm = 0;

void init_hcsr04_pins(void)
{
   RCC->AHB1ENR |= (1 << 0) | (1 << 1);

   /* pa0 -> tim2_ch1 (echo) */
   GPIOA->MODER &= ~(3 << (2 * 0));
   GPIOA->MODER |= (2 << (2 * 0));
   GPIOA->AFR[0] &= ~(0xF << (4 * 0));
   GPIOA->AFR[0] |= (0x1 << (4 * 0));

   /* pb6 -> trigger output */
   GPIOB->MODER &= ~(3 << (2 * 6));
   GPIOB->MODER |= (1 << (2 * 6));
   GPIOB->BSRR = (1 << (6 + 16));
}

void init_pwm_led_pa6(void)
{
   RCC->AHB1ENR |= (1 << 0);

   /* pa6 -> tim3_ch1 (af2) */
   GPIOA->MODER &= ~(3 << (2 * 6));
   GPIOA->MODER |= (2 << (2 * 6));
   GPIOA->AFR[0] &= ~(0xF << (4 * 6));
   GPIOA->AFR[0] |= (0x2 << (4 * 6));
}

void tim2_init_input_capture(void)
{
   RCC->APB1ENR |= (1 << 0);

   TIM2->CR1 = 0;
   TIM2->PSC = 16 - 1; /* 1 mhz -> 1 us resolution */
   TIM2->ARR = 0xFFFFFFFF;
   TIM2->CNT = 0;

   TIM2->CCMR1 &= ~((3 << 0));
   TIM2->CCMR1 |= (1 << 0); /* cc1 from ti1 */

   TIM2->CCER &= ~((1 << 1) | (1 << 3)); /* rising edge */
   TIM2->CCER |= (1 << 0);

   TIM2->DIER |= (1 << 1);
   TIM2->EGR = (1 << 0);
   TIM2->SR = 0;

   NVIC_EnableIRQ(TIM2_IRQn);

   TIM2->CR1 |= (1 << 0);
}

void tim3_init_pwm_1khz(void)
{
   RCC->APB1ENR |= (1 << 1);

   TIM3->CR1 = 0;
   TIM3->PSC = 16 - 1;   /* 1 mhz */
   TIM3->ARR = 1000 - 1; /* 1 khz */
   TIM3->CCR1 = 0;

   TIM3->CCMR1 &= ~((3 << 0) | (7 << 4));
   TIM3->CCMR1 |= (6 << 4); /* pwm mode 1 */
   TIM3->CCMR1 |= (1 << 3);

   TIM3->CCER |= (1 << 0);
   TIM3->CR1 |= (1 << 7);

   TIM3->EGR = (1 << 0);
   TIM3->CR1 |= (1 << 0);
}

void hcsr04_send_trigger(void)
{
   uint32_t start;

   GPIOB->BSRR = (1 << 6);
   start = TIM2->CNT;

   while ((uint32_t)(TIM2->CNT - start) < 10)
   {
   }

   GPIOB->BSRR = (1 << (6 + 16));
}

uint32_t distance_to_duty_percent(uint32_t d_cm)
{
   const uint32_t near_cm = 5;
   const uint32_t far_cm = 50;

   if (d_cm <= near_cm)
   {
      return 100;
   }

   if (d_cm >= far_cm)
   {
      return 0;
   }

   return ((far_cm - d_cm) * 100) / (far_cm - near_cm);
}

void set_pwm_duty_percent(uint32_t duty_percent)
{
   if (duty_percent > 100)
   {
      duty_percent = 100;
   }

   TIM3->CCR1 = ((TIM3->ARR + 1) * duty_percent) / 100;
}

void TIM2_IRQHandler(void)
{
   if (TIM2->SR & (1 << 1))
   {
      uint32_t captured = TIM2->CCR1;
      TIM2->SR &= ~(1 << 1);

      if (waiting_falling_edge == 0)
      {
         t_rise = captured;
         waiting_falling_edge = 1;
         TIM2->CCER |= (1 << 1); /* falling edge next */
      }
      else
      {
         t_fall = captured;

         if (t_fall >= t_rise)
         {
            duration_us = t_fall - t_rise;
         }
         else
         {
            duration_us = (0xFFFFFFFF - t_rise) + t_fall + 1;
         }

         distance_cm = (uint32_t)((float)duration_us / 2.0f / 29.412f);
         measurement_ready = 1;

         waiting_falling_edge = 0;
         TIM2->CCER &= ~(1 << 1); /* back to rising edge */
      }
   }
}

int main(void)
{
   HAL_Init();

   init_hcsr04_pins();
   init_pwm_led_pa6();
   tim2_init_input_capture();
   tim3_init_pwm_1khz();

   while (1)
   {
      hcsr04_send_trigger();
      HAL_Delay(60);

      if (measurement_ready)
      {
         uint32_t duty;

         measurement_ready = 0;
         duty = distance_to_duty_percent(distance_cm);
         set_pwm_duty_percent(duty);
      }
   }
}

```

### File: week8\week8_1.cpp

```cpp
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
   RCC->AHB1ENR |= (1 << 0); //gpioa
   RCC->APB1ENR |= (1 << 17); //usart2

   GPIOA->MODER |= (2 << (2 * 2)); //pa2 alternate function
   GPIOA->AFR[0] |= (7 << (4 * 2)); //af7 for usart2

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

```

### File: week8\week8_2.cpp

```cpp
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

```

### File: week8\week8_3.cpp

```cpp
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

```

