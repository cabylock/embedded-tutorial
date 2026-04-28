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
