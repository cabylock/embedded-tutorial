// Bài 2: Bộ đếm Nhị phân (Binary Counter)
// SW1 (PA0) và 3 LED (PB0, PB1, PB2)
// Mỗi lần nhấn SW1 tăng bộ đếm nhị phân 1 đơn vị (000 -> 111 -> 000)

#include "stm32f4xx_hal.h"

// Debounce timing
#define DEBOUNCE_DELAY 50

// Pin definitions
#define SW1_PIN 0   // PA0
#define LED1_PIN 0  // PB0 (LSB)
#define LED2_PIN 1  // PB1
#define LED3_PIN 2  // PB2 (MSB)

// Global variables
volatile uint32_t last_sw1_time = 0;
volatile uint8_t last_sw1_state = 0;
volatile uint8_t sw1_state = 0;
volatile uint8_t counter = 0;

// Function prototypes
void init_gpio(void);
void delay_ms(uint32_t ms);
void update_leds(uint8_t value);
uint8_t read_debounced_switch(uint8_t pin, volatile uint32_t* last_time, volatile uint8_t* last_state);

int main(void) {
    // Initialize HAL library
    HAL_Init();

    // Initialize GPIO
    init_gpio();

    // Initial state: all LEDs off (000)
    update_leds(0);

    while (1) {
        // Check for switch press
        if (read_debounced_switch(SW1_PIN, &last_sw1_time, &last_sw1_state)) {
            // Increment counter (0-7, then wrap to 0)
            counter = (counter + 1) & 0x07;

            // Update LEDs to show current count
            update_leds(counter);
        }

        // Small delay to prevent excessive CPU usage
        delay_ms(10);
    }
}

// Initialize GPIO pins for switch and LEDs
void init_gpio(void) {
    // Enable clock for GPIOA and GPIOB
    RCC->AHB1ENR |= (1 << 0) | (1 << 1);  // GPIOA and GPIOB

    // Configure SW1 (PA0) as input with pull-down
    GPIOA->MODER &= ~(3 << (2 * SW1_PIN));  // Clear mode bits
    GPIOA->PUPDR &= ~(3 << (2 * SW1_PIN));  // Clear pull-up/down
    GPIOA->PUPDR |= (2 << (2 * SW1_PIN));   // Set pull-down

    // Configure LEDs (PB0, PB1, PB2) as outputs
    GPIOB->MODER &= ~((3 << (2 * LED1_PIN)) | (3 << (2 * LED2_PIN)) | (3 << (2 * LED3_PIN)));
    GPIOB->MODER |= ((1 << (2 * LED1_PIN)) | (1 << (2 * LED2_PIN)) | (1 << (2 * LED3_PIN)));
    GPIOB->OTYPER &= ~((1 << LED1_PIN) | (1 << LED2_PIN) | (1 << LED3_PIN));  // Push-pull
    GPIOB->OSPEEDR |= ((3 << (2 * LED1_PIN)) | (3 << (2 * LED2_PIN)) | (3 << (2 * LED3_PIN)));  // High speed
}

// Simple delay function
void delay_ms(uint32_t ms) {
    for (volatile uint32_t i = 0; i < ms * 1000; i++);
}

// Update LED states based on counter value
void update_leds(uint8_t value) {
    // LED1 (PB0) = bit 0
    if (value & 0x01) {
        GPIOB->BSRR = (1 << LED1_PIN);  // Turn on
    } else {
        GPIOB->BSRR = (1 << (LED1_PIN + 16));  // Turn off
    }

    // LED2 (PB1) = bit 1
    if (value & 0x02) {
        GPIOB->BSRR = (1 << LED2_PIN);  // Turn on
    } else {
        GPIOB->BSRR = (1 << (LED2_PIN + 16));  // Turn off
    }

    // LED3 (PB2) = bit 2
    if (value & 0x04) {
        GPIOB->BSRR = (1 << LED3_PIN);  // Turn on
    } else {
        GPIOB->BSRR = (1 << (LED3_PIN + 16));  // Turn off
    }
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
    }

    return 0;  // No valid press detected
}