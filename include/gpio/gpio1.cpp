// Bài 1: LED toggle với debounce và mạch cảnh báo an toàn
// SW1 (PA0), SW2 (PA1) và LED 1 (PB0)
// LED 1 chỉ sáng khi cả 2 nút SW1 và SW2 được nhấn cùng một lúc

#include "stm32f4xx_hal.h"

// Debounce timing
#define DEBOUNCE_DELAY 50

// Pin definitions
#define SW1_PIN 0  // PA0
#define SW2_PIN 1  // PA1
#define LED_PIN 0  // PB0

// Global variables for debouncing
volatile uint32_t last_sw1_time = 0;
volatile uint32_t last_sw2_time = 0;
volatile uint8_t sw1_state = 0;
volatile uint8_t sw2_state = 0;
volatile uint8_t last_sw1_state = 0;
volatile uint8_t last_sw2_state = 0;

// Function prototypes
void init_gpio(void);
void delay_ms(uint32_t ms);
uint8_t read_debounced_switch(uint8_t pin, volatile uint32_t* last_time, volatile uint8_t* last_state);

int main(void) {
    // Initialize HAL library
    HAL_Init();

    // Initialize GPIO
    init_gpio();

    // LED initially off
    GPIOB->BSRR = (1 << (LED_PIN + 16));  // Turn off LED

    while (1) {
        // Read debounced switch states
        uint8_t sw1_pressed = read_debounced_switch(SW1_PIN, &last_sw1_time, &last_sw1_state);
        uint8_t sw2_pressed = read_debounced_switch(SW2_PIN, &last_sw2_time, &last_sw2_state);

        // LED 1 only on when BOTH switches are pressed
        if (sw1_pressed && sw2_pressed) {
            GPIOB->BSRR = (1 << LED_PIN);  // Turn on LED
        } else {
            GPIOB->BSRR = (1 << (LED_PIN + 16));  // Turn off LED
        }

        // Small delay to prevent excessive CPU usage
        delay_ms(10);
    }
}

// Initialize GPIO pins for switches and LED
void init_gpio(void) {
    // Enable clock for GPIOA and GPIOB
    RCC->AHB1ENR |= (1 << 0) | (1 << 1);  // GPIOA and GPIOB

    // Configure SW1 (PA0) as input with pull-down
    GPIOA->MODER &= ~(3 << (2 * SW1_PIN));  // Clear mode bits
    GPIOA->PUPDR &= ~(3 << (2 * SW1_PIN));  // Clear pull-up/down
    GPIOA->PUPDR |= (2 << (2 * SW1_PIN));   // Set pull-down

    // Configure SW2 (PA1) as input with pull-down
    GPIOA->MODER &= ~(3 << (2 * SW2_PIN));  // Clear mode bits
    GPIOA->PUPDR &= ~(3 << (2 * SW2_PIN));  // Clear pull-up/down
    GPIOA->PUPDR |= (2 << (2 * SW2_PIN));   // Set pull-down

    // Configure LED (PB0) as output
    GPIOB->MODER &= ~(3 << (2 * LED_PIN));  // Clear mode bits
    GPIOB->MODER |= (1 << (2 * LED_PIN));   // Set as output
    GPIOB->OTYPER &= ~(1 << LED_PIN);       // Push-pull output
    GPIOB->OSPEEDR |= (3 << (2 * LED_PIN)); // High speed
}

// Simple delay function
void delay_ms(uint32_t ms) {
    for (volatile uint32_t i = 0; i < ms * 1000; i++);
}

// Debounce function for switches
uint8_t read_debounced_switch(uint8_t pin, volatile uint32_t* last_time, uint8_t* last_state) {
    uint8_t current_state = (GPIOA->IDR & (1 << pin)) ? 1 : 0;
    uint32_t current_time = HAL_GetTick();

    // Check if state has changed
    if (current_state != *last_state) {
        *last_time = current_time;  // Reset the debounce timer
        *last_state = current_state;
    }

    // Check if debounce delay has passed
    if ((current_time - *last_time) > DEBOUNCE_DELAY) {
        // State has been stable for debounce period
        if (current_state != sw1_state && pin == SW1_PIN) {
            sw1_state = current_state;
            return (sw1_state == 1);  // Return 1 on press (active low with pull-down)
        }
        if (current_state != sw2_state && pin == SW2_PIN) {
            sw2_state = current_state;
            return (sw2_state == 1);  // Return 1 on press (active low with pull-down)
        }
    }

    return 0;  // No valid press detected
}