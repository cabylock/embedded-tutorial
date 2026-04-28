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

