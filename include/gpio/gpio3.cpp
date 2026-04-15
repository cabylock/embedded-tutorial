// Bài 3: Đếm từ 1 đến 5 và LED nhấp nháy tương ứng
// SW1 (PA0) và LED (PB0)
// Mỗi lần nhấn SW1 tăng bộ đếm 1 (1-5, rồi quay lại 1)
// LED nhấp nháy số lần tương ứng với giá trị bộ đếm

#include "stm32f4xx_hal.h"

// Debounce timing
#define DEBOUNCE_DELAY 50

// Pin definitions
#define SW1_PIN 0   // PA0
#define LED_PIN 0   // PB0

// Global variables
volatile uint32_t last_sw1_time = 0;
volatile uint8_t last_sw1_state = 0;
volatile uint8_t sw1_state = 0;
volatile uint8_t counter = 0;

// Function prototypes
void init_gpio(void);
void delay_ms(uint32_t ms);
void blink_led(uint8_t times);
uint8_t read_debounced_switch(uint8_t pin, volatile uint32_t* last_time, volatile uint8_t* last_state);

int main(void) {
    // Initialize HAL library
    HAL_Init();

    // Initialize GPIO
    init_gpio();

    // Initial state: LED off
    GPIOB->BSRR = (1 << (LED_PIN + 16));  // Turn off LED

    while (1) {
        // Check for switch press
        if (read_debounced_switch(SW1_PIN, &last_sw1_time, &last_sw1_state)) {
            // Increment counter (1-5, then wrap to 1)
            counter++;
            if (counter > 5) {
                counter = 1;
            }

            // Blink LED counter times
            blink_led(counter);
        }

        // Small delay to prevent excessive CPU usage
        delay_ms(10);
    }
}

// Initialize GPIO pins for switch and LED
void init_gpio(void) {
    // Enable clock for GPIOA and GPIOB
    RCC->AHB1ENR |= (1 << 0) | (1 << 1);  // GPIOA and GPIOB

    // Configure SW1 (PA0) as input with pull-down
    GPIOA->MODER &= ~(3 << (2 * SW1_PIN));  // Clear mode bits
    GPIOA->PUPDR &= ~(3 << (2 * SW1_PIN));  // Clear pull-up/down
    GPIOA->PUPDR |= (2 << (2 * SW1_PIN));   // Set pull-down

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

// Blink LED specified number of times
void blink_led(uint8_t times) {
    for (uint8_t i = 0; i < times; i++) {
        // Turn on LED
        GPIOB->BSRR = (1 << LED_PIN);
        delay_ms(200);  // LED on for 200ms

        // Turn off LED
        GPIOB->BSRR = (1 << (LED_PIN + 16));
        delay_ms(200);  // LED off for 200ms
    }
}

// Debounce function for switches
uint8_t read_debounced_switch(uint8_t pin, volatile uint32_t* last_time, volatile uint8_t* last_state) {
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
    }

    return 0;  // No valid press detected
}