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
