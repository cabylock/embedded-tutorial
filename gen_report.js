const {
  Document, Packer, Paragraph, TextRun,
  HeadingLevel, AlignmentType, BorderStyle, WidthType, ShadingType,
  LevelFormat, PageBreak
} = require('docx');
const fs = require('fs');

const FONT = "Times New Roman";
const CODE_FONT = "Courier New";

function h1(text) {
  return new Paragraph({
    heading: HeadingLevel.HEADING_1,
    children: [new TextRun({ text, font: FONT, bold: true, size: 28 })],
    spacing: { before: 300, after: 150 }
  });
}

function h2(text) {
  return new Paragraph({
    heading: HeadingLevel.HEADING_2,
    children: [new TextRun({ text, font: FONT, bold: true, size: 26 })],
    spacing: { before: 220, after: 110 }
  });
}

function p(text, opts = {}) {
  return new Paragraph({
    children: [new TextRun({ text, font: FONT, size: 22, ...opts })],
    spacing: { before: 60, after: 60 }
  });
}

function bullet(text) {
  return new Paragraph({
    numbering: { reference: "bullets", level: 0 },
    children: [new TextRun({ text, font: FONT, size: 22 })],
    spacing: { before: 40, after: 40 }
  });
}

// Remove ONLY consecutive blank lines (keep single blank lines for readability)
// Actually: remove all blank lines but keep the original code structure
function cleanCode(raw) {
  const lines = raw.split('\n');
  // Remove leading/trailing blank lines, keep inner ones (max 1 consecutive blank)
  const result = [];
  let prevBlank = false;
  for (const line of lines) {
    const isBlank = line.trim() === '';
    if (isBlank) {
      if (!prevBlank) result.push('');
      prevBlank = true;
    } else {
      result.push(line);
      prevBlank = false;
    }
  }
  // Trim leading/trailing empty
  while (result.length && result[0] === '') result.shift();
  while (result.length && result[result.length - 1] === '') result.pop();
  return result.join('\n');
}

function codeSection(filename, code) {
  const cleaned = cleanCode(code);
  const lines = cleaned.split('\n');
  const paras = [
    new Paragraph({
      children: [new TextRun({ text: `// ${filename}`, font: CODE_FONT, size: 17, color: "888888", italics: true })],
      spacing: { before: 100, after: 0 },
      shading: { fill: "F2F2F2", type: ShadingType.CLEAR }
    })
  ];
  for (const line of lines) {
    paras.push(new Paragraph({
      children: [new TextRun({ text: line === '' ? ' ' : line, font: CODE_FONT, size: 17 })],
      spacing: { before: 0, after: 0 },
      shading: { fill: "F2F2F2", type: ShadingType.CLEAR }
    }));
  }
  return paras;
}

function pageBreak() {
  return new Paragraph({ children: [new PageBreak()] });
}

// ===================== CODE GỐC (giữ nguyên định dạng) =====================

const week1_1 = `#include "stm32f4xx_hal.h"

int main(void)
{
    RCC->AHB1ENR |= 1 << 0;
    GPIOA->MODER |= 1 << (2*5);

    while(1)
    {
        GPIOA->BSRR = 1 << 5;
        for(volatile uint32_t i = 0; i < 100000; i++);
        GPIOA->BSRR = 1 << (5+16);
        for(volatile uint32_t i = 0; i < 100000; i++);
    }
}`;

const week1_2 = `#include "stm32f4xx_hal.h"

int main(void)
{
    RCC->AHB1ENR |= 1 << 0;
    GPIOA->MODER |= 1 << (2*5);

    RCC->AHB1ENR |= 1 << 2;
    GPIOC->MODER |= 0 << (2*13);
    GPIOC->PUPDR |= 0 << (2*13);

    uint16_t is_pressed;
    while(1)
    {
        is_pressed = GPIOC->IDR &= (1<<13);
        if (is_pressed == 0)
            GPIOA->BSRR = 1 << 5;
        else
            GPIOA->BSRR = 1 << (5+16);
    }
}`;

const week2_1 = `#include "stm32f4xx_hal.h"

int main(void)
{
    RCC->AHB1ENR |= 1 << 2;            // enable GPIOC clock

    GPIOC->MODER |= 1 << (2*6);        // PC6: output
    GPIOC->MODER |= 0 << (2*5);        // PC5: input
    GPIOC->PUPDR |= 0 << (2*5);        // PC5: no pull

    uint16_t is_pressed;
    while(1)
    {
        is_pressed = GPIOC->IDR & (1<<5);
        if (is_pressed == 0)
            GPIOC->BSRR = 1 << 6;
        else
            GPIOC->BSRR = 1 << (6+16);
    }
}`;

const week2_2 = `#include "stm32f4xx_hal.h"

int main(void)
{
    RCC->AHB1ENR |= 1 << 2;            // GPIOC clock
    RCC->AHB1ENR |= 1 << 1;            // GPIOB clock

    GPIOC->MODER |= 0 << (2*5);  GPIOC->PUPDR |= 0 << (2*5);  // PC5: input
    GPIOC->MODER |= 0 << (2*6);  GPIOC->PUPDR |= 0 << (2*6);  // PC6: input
    GPIOC->MODER |= 0 << (2*8);  GPIOC->PUPDR |= 0 << (2*8);  // PC8: input

    GPIOB->MODER |= 1 << (2*13);       // PB13: output
    GPIOB->MODER |= 1 << (2*14);       // PB14: output
    GPIOB->MODER |= 1 << (2*15);       // PB15: output

    uint16_t is_pressed_1, is_pressed_2, is_pressed_3;
    while(1)
    {
        is_pressed_1 = GPIOC->IDR & (1<<5);
        is_pressed_2 = GPIOC->IDR & (1<<6);
        is_pressed_3 = GPIOC->IDR & (1<<8);

        if (is_pressed_1 == 0) GPIOB->BSRR = 1 << 13;
        else                   GPIOB->BSRR = 1 << (13+16);

        if (is_pressed_2 == 0) GPIOB->BSRR = 1 << 14;
        else                   GPIOB->BSRR = 1 << (14+16);

        if (is_pressed_3 == 0) GPIOB->BSRR = 1 << 15;
        else                   GPIOB->BSRR = 1 << (15+16);
    }
}`;

const week3_1 = `#include "stm32f4xx_hal.h"

#define DEBOUNCE_DELAY  50
#define SW1_PIN         0   // PA0
#define SW2_PIN         1   // PA1
#define LED_PIN         0   // PB0

volatile uint8_t last_sw1_state = 0;
volatile uint8_t last_sw2_state = 0;

void init_gpio(void)
{
    RCC->AHB1ENR |= (1 << 0) | (1 << 1);       // GPIOA + GPIOB clock

    // SW1 – input pull-down
    GPIOA->MODER &= ~(3 << (2 * SW1_PIN));
    GPIOA->PUPDR &= ~(3 << (2 * SW1_PIN));
    GPIOA->PUPDR |=  (2 << (2 * SW1_PIN));

    // SW2 – input pull-down
    GPIOA->MODER &= ~(3 << (2 * SW2_PIN));
    GPIOA->PUPDR &= ~(3 << (2 * SW2_PIN));
    GPIOA->PUPDR |=  (2 << (2 * SW2_PIN));

    // LED – output push-pull
    GPIOB->MODER &= ~(3 << (2 * LED_PIN));
    GPIOB->MODER |=  (1 << (2 * LED_PIN));
    GPIOB->OTYPER  &= ~(1 << LED_PIN);
    GPIOB->OSPEEDR |=  (3 << (2 * LED_PIN));
}

void delay_ms(uint32_t ms)
{
    for (volatile uint32_t i = 0; i < ms * 1000; i++);
}

uint8_t read_debounced_switch(uint8_t pin, volatile uint8_t *last_state)
{
    uint8_t cur = (GPIOA->IDR & (1 << pin)) ? 1 : 0;
    if (cur != *last_state) {
        delay_ms(DEBOUNCE_DELAY);
        cur = (GPIOA->IDR & (1 << pin)) ? 1 : 0;
        if (cur != *last_state) {
            *last_state = cur;
            return (cur == 1);
        }
    }
    return 0;
}

int main(void)
{
    init_gpio();
    GPIOB->BSRR = (1 << (LED_PIN + 16));       // LED tắt ban đầu
    while (1)
    {
        uint8_t sw1 = read_debounced_switch(SW1_PIN, &last_sw1_state);
        uint8_t sw2 = read_debounced_switch(SW2_PIN, &last_sw2_state);

        if (sw1 && sw2) GPIOB->BSRR = (1 << LED_PIN);
        else            GPIOB->BSRR = (1 << (LED_PIN + 16));

        delay_ms(10);
    }
}`;

const week3_2 = `#include "stm32f4xx_hal.h"

#define DEBOUNCE_DELAY  50
#define SW1_PIN         0   // PA0
#define LED1_PIN        0   // PB0 – bit 0
#define LED2_PIN        1   // PB1 – bit 1
#define LED3_PIN        2   // PB2 – bit 2

volatile uint8_t last_sw1_state = 0;
volatile uint8_t counter = 0;

void init_gpio(void)
{
    RCC->AHB1ENR |= (1 << 0) | (1 << 1);

    GPIOA->MODER &= ~(3 << (2 * SW1_PIN));
    GPIOA->PUPDR &= ~(3 << (2 * SW1_PIN));
    GPIOA->PUPDR |=  (2 << (2 * SW1_PIN));     // pull-down

    GPIOB->MODER &= ~((3<<(2*LED1_PIN))|(3<<(2*LED2_PIN))|(3<<(2*LED3_PIN)));
    GPIOB->MODER |=  ((1<<(2*LED1_PIN))|(1<<(2*LED2_PIN))|(1<<(2*LED3_PIN)));
    GPIOB->OTYPER  &= ~((1<<LED1_PIN)|(1<<LED2_PIN)|(1<<LED3_PIN));
    GPIOB->OSPEEDR |=  ((3<<(2*LED1_PIN))|(3<<(2*LED2_PIN))|(3<<(2*LED3_PIN)));
}

void delay_ms(uint32_t ms)
{
    for (volatile uint32_t i = 0; i < ms * 1000; i++);
}

void update_leds(uint8_t value)
{
    if (value & 0x01) GPIOB->BSRR = (1 << LED1_PIN);
    else              GPIOB->BSRR = (1 << (LED1_PIN + 16));

    if (value & 0x02) GPIOB->BSRR = (1 << LED2_PIN);
    else              GPIOB->BSRR = (1 << (LED2_PIN + 16));

    if (value & 0x04) GPIOB->BSRR = (1 << LED3_PIN);
    else              GPIOB->BSRR = (1 << (LED3_PIN + 16));
}

uint8_t read_debounced_switch(uint8_t pin, volatile uint8_t *last_state)
{
    uint8_t cur = (GPIOA->IDR & (1 << pin)) ? 1 : 0;
    if (cur != *last_state) {
        delay_ms(DEBOUNCE_DELAY);
        cur = (GPIOA->IDR & (1 << pin)) ? 1 : 0;
        if (cur != *last_state) { *last_state = cur; return (cur == 1); }
    }
    return 0;
}

int main(void)
{
    init_gpio();
    update_leds(0);
    while (1)
    {
        if (read_debounced_switch(SW1_PIN, &last_sw1_state)) {
            counter = (counter + 1) & 0x07;    // 0 – 7
            update_leds(counter);
        }
        delay_ms(10);
    }
}`;

const week3_3 = `#include "stm32f4xx_hal.h"

#define DEBOUNCE_DELAY  50
#define SW1_PIN         0   // PA0
#define LED_PIN         0   // PB0

volatile uint8_t last_sw1_state = 0;
volatile uint8_t counter = 0;

void init_gpio(void)
{
    RCC->AHB1ENR |= (1 << 0) | (1 << 1);

    GPIOA->MODER &= ~(3 << (2 * SW1_PIN));
    GPIOA->PUPDR &= ~(3 << (2 * SW1_PIN));
    GPIOA->PUPDR |=  (2 << (2 * SW1_PIN));     // pull-down

    GPIOB->MODER &= ~(3 << (2 * LED_PIN));
    GPIOB->MODER |=  (1 << (2 * LED_PIN));
    GPIOB->OTYPER  &= ~(1 << LED_PIN);
    GPIOB->OSPEEDR |=  (3 << (2 * LED_PIN));
}

void delay_ms(uint32_t ms)
{
    for (volatile uint32_t i = 0; i < ms * 1000; i++);
}

void blink_led(uint8_t times)
{
    for (uint8_t i = 0; i < times; i++) {
        GPIOB->BSRR = (1 << LED_PIN);          // bật
        delay_ms(200);
        GPIOB->BSRR = (1 << (LED_PIN + 16));   // tắt
        delay_ms(200);
    }
}

uint8_t read_debounced_switch(uint8_t pin, volatile uint8_t *last_state)
{
    uint8_t cur = (GPIOA->IDR & (1 << pin)) ? 1 : 0;
    if (cur != *last_state) {
        delay_ms(DEBOUNCE_DELAY);
        cur = (GPIOA->IDR & (1 << pin)) ? 1 : 0;
        if (cur != *last_state) { *last_state = cur; return (cur == 1); }
    }
    return 0;
}

int main(void)
{
    init_gpio();
    GPIOB->BSRR = (1 << (LED_PIN + 16));
    while (1)
    {
        if (read_debounced_switch(SW1_PIN, &last_sw1_state)) {
            counter++;
            if (counter > 5) counter = 1;
            blink_led(counter);
        }
        delay_ms(10);
    }
}`;

const week4_1 = `#include "stm32f4xx_hal.h"

void init_button(void)
{
    RCC->AHB1ENR |= 1 << 2;                            // GPIOC clock
    GPIOC->MODER &= ~(3 << (2*13));
    GPIOC->PUPDR &= ~(3 << (2*13));                    // PC13: input
    GPIOC->MODER &= ~(3 << (2*8));
    GPIOC->PUPDR &= ~(3 << (2*8));                     // PC8: input
}

void init_led(void)
{
    RCC->AHB1ENR |= 1 << 0;
    GPIOA->MODER |= 1 << (2*5);                        // PA5: output
}

void init_interrupt(void)
{
    RCC->APB2ENR |= 1 << 14;                           // SYSCFG clock

    SYSCFG->EXTICR[(13/4)] |= 2 << (4*(13%4));         // EXTI13 <- PC13
    EXTI->IMR  |= 1 << 13;
    EXTI->FTSR |= 1 << 13;                             // sườn xuống
    NVIC_EnableIRQ(EXTI15_10_IRQn);

    SYSCFG->EXTICR[(8/4)] |= 2 << (4*(8%4));           // EXTI8 <- PC8
    EXTI->IMR  |= 1 << 8;
    EXTI->FTSR |= 1 << 8;
    NVIC_EnableIRQ(EXTI9_5_IRQn);
}

void EXTI15_10_IRQHandler(void)
{
    if (EXTI->PR & (1 << 13)) {
        EXTI->PR |= (1 << 13);                         // xóa pending
        GPIOA->BSRR = (1 << (5+16));                   // LED tắt (PA5 low)
        for (volatile int i = 0; i < 100000; i++);
    }
}

void EXTI9_5_IRQHandler(void)
{
    if (EXTI->PR & (1 << 8)) {
        EXTI->PR |= (1 << 8);
        GPIOA->BSRR = 1 << 5;                          // LED bật
        for (volatile int i = 0; i < 100000; i++);
    }
}

int main(void)
{
    init_button();
    init_led();
    init_interrupt();
    while (1)
    {
        GPIOA->ODR ^= 1 << 5;                          // toggle liên tục
        for (volatile int i = 0; i < 100000; i++);
    }
}`;

const week4_2 = `#include "stm32f4xx_hal.h"

volatile int32_t led_offset = 1;       // +1: trái, -1: phải, 0: dừng
#define LED_INITIAL  13                // PB13 = LED đầu tiên

void init_button(void)
{
    RCC->AHB1ENR |= 1 << 2;
    GPIOC->MODER &= ~(3 << (2*5));   GPIOC->PUPDR &= ~(3 << (2*5));   // PC5
    GPIOC->MODER &= ~(3 << (2*6));   GPIOC->PUPDR &= ~(3 << (2*6));   // PC6
    GPIOC->MODER &= ~(3 << (2*13));  GPIOC->PUPDR &= ~(3 << (2*13));  // PC13
}

void init_led(void)
{
    RCC->AHB1ENR |= 1 << 1;
    GPIOB->MODER |= 1 << (2*13);     // PB13
    GPIOB->MODER |= 1 << (2*14);     // PB14
    GPIOB->MODER |= 1 << (2*15);     // PB15
}

void init_interrupt(void)
{
    RCC->APB2ENR |= 1 << 14;
    SYSCFG->EXTICR[(5/4)]  |= 2 << (4*(5%4));   // EXTI5  <- PC5
    SYSCFG->EXTICR[(6/4)]  |= 2 << (4*(6%4));   // EXTI6  <- PC6
    SYSCFG->EXTICR[(13/4)] |= 2 << (4*(13%4));  // EXTI13 <- PC13
    EXTI->IMR  |= (1<<5)|(1<<6)|(1<<13);
    EXTI->FTSR |= (1<<5)|(1<<6)|(1<<13);
    NVIC_EnableIRQ(EXTI9_5_IRQn);
    NVIC_EnableIRQ(EXTI15_10_IRQn);
}

void EXTI9_5_IRQHandler(void)
{
    if (EXTI->PR & (1 << 5)) { EXTI->PR |= (1<<5); led_offset =  1; }  // SW1: sang trái
    if (EXTI->PR & (1 << 6)) { EXTI->PR |= (1<<6); led_offset = -1; }  // SW2: sang phải
}

void EXTI15_10_IRQHandler(void)
{
    if (EXTI->PR & (1 << 13)) { EXTI->PR |= (1<<13); led_offset = 0; } // SW3: dừng
}

int main(void)
{
    init_button();
    init_led();
    init_interrupt();
    uint32_t cur = LED_INITIAL;
    while (1)
    {
        GPIOB->ODR |= 1 << cur;
        for (volatile int i = 0; i < 500000; i++);
        GPIOB->ODR &= ~(1 << cur);
        cur = LED_INITIAL + (cur - LED_INITIAL + led_offset + 3) % 3;
    }
}`;

const week4_3 = `#include "stm32f4xx_hal.h"

// SW1=PC0, SW2=PC1, SW3=PC2 | LED1=PB13, LED2=PB14, LED3=PB15

void delay_loop(uint32_t count)
{
    for (volatile uint32_t i = 0; i < count; i++);
}

void init_button(void)
{
    RCC->AHB1ENR |= 1 << 2;
    GPIOC->MODER &= ~(3<<(2*0)); GPIOC->PUPDR &= ~(3<<(2*0));  // PC0
    GPIOC->MODER &= ~(3<<(2*1)); GPIOC->PUPDR &= ~(3<<(2*1));  // PC1
    GPIOC->MODER &= ~(3<<(2*2)); GPIOC->PUPDR &= ~(3<<(2*2));  // PC2
}

void init_led(void)
{
    RCC->AHB1ENR |= 1 << 1;
    GPIOB->MODER &= ~(3<<(2*13)); GPIOB->MODER |= 1<<(2*13);
    GPIOB->MODER &= ~(3<<(2*14)); GPIOB->MODER |= 1<<(2*14);
    GPIOB->MODER &= ~(3<<(2*15)); GPIOB->MODER |= 1<<(2*15);
}

void init_interrupt(void)
{
    RCC->APB2ENR |= 1 << 14;
    SYSCFG->EXTICR[0] |= 2 << (4*0);   // PC0 -> EXTI0
    SYSCFG->EXTICR[0] |= 2 << (4*1);   // PC1 -> EXTI1
    SYSCFG->EXTICR[0] |= 2 << (4*2);   // PC2 -> EXTI2
    EXTI->IMR  |= (1<<0)|(1<<1)|(1<<2);
    EXTI->FTSR |= (1<<0)|(1<<1)|(1<<2);
    NVIC_SetPriority(EXTI2_IRQn, 0);    // SW3: cao nhất
    NVIC_SetPriority(EXTI1_IRQn, 1);    // SW2
    NVIC_SetPriority(EXTI0_IRQn, 2);    // SW1: thấp nhất
    NVIC_EnableIRQ(EXTI0_IRQn);
    NVIC_EnableIRQ(EXTI1_IRQn);
    NVIC_EnableIRQ(EXTI2_IRQn);
}

void EXTI0_IRQHandler(void)   // SW1 – LED1 nhấp nháy chậm (2 lần ~1s)
{
    if (EXTI->PR & (1<<0)) {
        EXTI->PR = (1<<0);
        for (int i = 0; i < 2; i++) {
            GPIOB->ODR |=  (1<<13); delay_loop(500000);
            GPIOB->ODR &= ~(1<<13); delay_loop(500000);
        }
    }
}

void EXTI1_IRQHandler(void)   // SW2 – LED2 nhấp nháy nhanh (4 lần)
{
    if (EXTI->PR & (1<<1)) {
        EXTI->PR = (1<<1);
        for (int i = 0; i < 4; i++) {
            GPIOB->ODR |=  (1<<14); delay_loop(500000);
            GPIOB->ODR &= ~(1<<14); delay_loop(500000);
        }
    }
}

void EXTI2_IRQHandler(void)   // SW3 – LED3 bật 3s (6 lần ~0.5s)
{
    if (EXTI->PR & (1<<2)) {
        EXTI->PR = (1<<2);
        for (int i = 0; i < 6; i++) {
            GPIOB->ODR |=  (1<<15); delay_loop(500000);
            GPIOB->ODR &= ~(1<<15); delay_loop(500000);
        }
    }
}

int main(void)
{
    init_button();
    init_led();
    init_interrupt();
    while (1) { }
}`;

const week6_1 = `#include "stm32f4xx_hal.h"

void init_led_pa5(void)
{
    RCC->AHB1ENR |= (1 << 0);
    GPIOA->MODER &= ~(3 << (2*5));
    GPIOA->MODER |=  (1 << (2*5));         // PA5: output
}

void toggle_led_pa5(void) { GPIOA->ODR ^= (1 << 5); }

void tim2_config_period_ms(uint32_t period_ms, uint8_t downcounter)
{
    RCC->APB1ENR |= (1 << 0);
    TIM2->CR1  = 0;
    TIM2->PSC  = 16000 - 1;                // 16 MHz / 16000 = 1 kHz  → 1ms/tick
    TIM2->ARR  = period_ms - 1;

    if (downcounter) {
        TIM2->CR1 |=  (1 << 4);            // DIR = 1 (down)
        TIM2->CNT  = TIM2->ARR;
    } else {
        TIM2->CR1 &= ~(1 << 4);            // DIR = 0 (up)
        TIM2->CNT  = 0;
    }

    TIM2->EGR = (1 << 0);                  // tạo update event
    TIM2->SR &= ~(1 << 0);                 // xóa UIF
    TIM2->CR1 |= (1 << 0);                 // bật timer
}

void delay_period_tim2(void)
{
    while ((TIM2->SR & (1 << 0)) == 0) {}  // chờ UIF
    TIM2->SR &= ~(1 << 0);                 // xóa UIF
}

int main(void)
{
    init_led_pa5();
    while (1)
    {
        tim2_config_period_ms(1000, 0);    // upcounter 1s
        for (uint8_t i = 0; i < 5; i++) {
            delay_period_tim2();
            toggle_led_pa5();
        }
        tim2_config_period_ms(2000, 1);    // downcounter 2s
        for (uint8_t i = 0; i < 5; i++) {
            delay_period_tim2();
            toggle_led_pa5();
        }
    }
}`;

const week6_2 = `#include "stm32f4xx_hal.h"

void init_leds(void)
{
    RCC->AHB1ENR |= (1<<0)|(1<<1);
    GPIOA->MODER &= ~(3<<(2*5)); GPIOA->MODER |= (1<<(2*5));  // PA5
    GPIOB->MODER &= ~(3<<(2*7)); GPIOB->MODER |= (1<<(2*7));  // PB7
}

void delay_ms(uint32_t ms)
{
    RCC->APB1ENR |= (1 << 0);
    TIM2->PSC = 16000 - 1;         // 1 kHz
    TIM2->ARR = ms - 1;
    TIM2->CNT = 0;
    TIM2->SR  = 0;
    TIM2->CR1 |= (1 << 0);         // start
    while (!(TIM2->SR & (1 << 0)));
    TIM2->CR1 &= ~(1 << 0);        // stop
    TIM2->SR  &= ~(1 << 0);        // xóa UIF
}

void tim3_init_1000ms(void)
{
    RCC->APB1ENR |= (1 << 1);
    TIM3->CR1  = 0;
    TIM3->PSC  = 16000 - 1;        // 1 kHz
    TIM3->ARR  = 1000 - 1;         // ngắt mỗi 1000ms
    TIM3->CNT  = 0;
    TIM3->DIER |= (1 << 0);        // bật UIE
    TIM3->EGR   = (1 << 0);
    TIM3->SR   &= ~(1 << 0);
    NVIC_EnableIRQ(TIM3_IRQn);
    TIM3->CR1 |= (1 << 0);
}

void TIM3_IRQHandler(void)
{
    if (TIM3->SR & (1 << 0)) {
        TIM3->SR &= ~(1 << 0);
        GPIOA->ODR ^= (1 << 5);    // toggle LED PA5 mỗi 1s
    }
}

int main(void)
{
    init_leds();
    tim3_init_1000ms();
    while (1)
    {
        GPIOB->ODR ^= (1 << 7);    // LED PB7 nhấp nháy
        delay_ms(500);
    }
}`;

const week6_3 = `#include "stm32f4xx_hal.h"

enum { STATE_GREEN = 0, STATE_YELLOW, STATE_RED };

volatile uint8_t traffic_state     = STATE_GREEN;
volatile uint8_t state_elapsed_s   = 0;
volatile uint8_t state_duration_s  = 5;
volatile uint8_t pedestrian_waiting = 0;

void set_lights(uint8_t state)
{
    GPIOA->BSRR = (1<<(5+16))|(1<<(6+16))|(1<<(7+16));  // tắt cả
    if      (state == STATE_GREEN)  GPIOA->BSRR = (1 << 7);
    else if (state == STATE_YELLOW) GPIOA->BSRR = (1 << 6);
    else                            GPIOA->BSRR = (1 << 5);
}

void init_traffic_leds(void)
{
    RCC->AHB1ENR |= (1 << 0);
    GPIOA->MODER &= ~((3<<(2*5))|(3<<(2*6))|(3<<(2*7)));
    GPIOA->MODER |=  ((1<<(2*5))|(1<<(2*6))|(1<<(2*7)));
    set_lights(STATE_GREEN);
}

void init_pedestrian_button_pc13(void)
{
    RCC->AHB1ENR  |= (1 << 2);
    RCC->APB2ENR  |= (1 << 14);
    GPIOC->MODER  &= ~(3 << (2*13));
    GPIOC->PUPDR  &= ~(3 << (2*13));
    SYSCFG->EXTICR[3] &= ~(0xF << 4);
    SYSCFG->EXTICR[3] |=  (0x2 << 4);  // EXTI13 <- PC13
    EXTI->IMR  |= (1 << 13);
    EXTI->FTSR |= (1 << 13);
    NVIC_EnableIRQ(EXTI15_10_IRQn);
}

void tim3_init_1s_interrupt(void)
{
    RCC->APB1ENR |= (1 << 1);
    TIM3->CR1  = 0;
    TIM3->PSC  = 16000 - 1;
    TIM3->ARR  = 1000 - 1;
    TIM3->CNT  = 0;
    TIM3->DIER |= (1 << 0);
    TIM3->EGR   = (1 << 0);
    TIM3->SR   &= ~(1 << 0);
    NVIC_EnableIRQ(TIM3_IRQn);
    TIM3->CR1 |= (1 << 0);
}

void EXTI15_10_IRQHandler(void)
{
    if (EXTI->PR & (1 << 13)) {
        EXTI->PR = (1 << 13);
        if (traffic_state == STATE_GREEN) {
            traffic_state    = STATE_YELLOW;
            state_elapsed_s  = 0;
            state_duration_s = 2;
            set_lights(STATE_YELLOW);
        } else {
            pedestrian_waiting = 1;         // ghi nhớ yêu cầu
        }
    }
}

void TIM3_IRQHandler(void)
{
    if (TIM3->SR & (1 << 0)) {
        TIM3->SR &= ~(1 << 0);
        state_elapsed_s++;
        if (state_elapsed_s >= state_duration_s) {
            state_elapsed_s = 0;
            if (traffic_state == STATE_GREEN) {
                traffic_state = STATE_YELLOW; state_duration_s = 2;
            } else if (traffic_state == STATE_YELLOW) {
                traffic_state = STATE_RED;    state_duration_s = 5;
                if (pedestrian_waiting) { state_duration_s += 3; pedestrian_waiting = 0; }
            } else {
                traffic_state = STATE_GREEN;  state_duration_s = 5;
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
    while (1) { }
}`;

const week6_4 = `#include "stm32f4xx_hal.h"

volatile uint8_t  waiting_falling_edge = 0;
volatile uint8_t  measurement_ready    = 0;
volatile uint32_t t_rise = 0, t_fall = 0;
volatile uint32_t duration_us = 0, distance_cm = 0;

void init_led_pa5(void)
{
    RCC->AHB1ENR |= (1 << 0);
    GPIOA->MODER &= ~(3 << (2*5));
    GPIOA->MODER |=  (1 << (2*5));
}

void init_hcsr04_pins(void)
{
    RCC->AHB1ENR |= (1 << 0);
    // PA0: TIM2_CH1 (AF1) – ECHO
    GPIOA->MODER  &= ~(3 << (2*0)); GPIOA->MODER  |= (2 << (2*0));
    GPIOA->AFR[0] &= ~(0xF << (4*0)); GPIOA->AFR[0] |= (0x1 << (4*0));
    // PA1: GPIO output – TRIGGER
    GPIOA->MODER  &= ~(3 << (2*1)); GPIOA->MODER  |= (1 << (2*1));
    GPIOA->BSRR    = (1 << (1+16));
}

void tim2_init_input_capture(void)
{
    RCC->APB1ENR |= (1 << 0);
    TIM2->CR1  = 0;
    TIM2->PSC  = 16 - 1;            // 1 MHz → 1 µs/tick
    TIM2->ARR  = 0xFFFFFFFF;
    TIM2->CNT  = 0;
    TIM2->CCMR1 &= ~(3 << 0); TIM2->CCMR1 |= (1 << 0);  // CC1 <- TI1
    TIM2->CCER  &= ~((1<<1)|(1<<3)); TIM2->CCER |= (1<<0); // rising edge, enable
    TIM2->DIER |= (1 << 1);         // CC1IE
    TIM2->EGR   = (1 << 0); TIM2->SR = 0;
    NVIC_EnableIRQ(TIM2_IRQn);
    TIM2->CR1 |= (1 << 0);
}

void hcsr04_send_trigger(void)
{
    uint32_t start;
    GPIOA->BSRR = (1 << 1); start = TIM2->CNT;
    while ((uint32_t)(TIM2->CNT - start) < 10) {}  // giữ HIGH 10µs
    GPIOA->BSRR = (1 << (1+16));
}

void TIM2_IRQHandler(void)
{
    if (TIM2->SR & (1 << 1)) {
        uint32_t captured = TIM2->CCR1;
        TIM2->SR &= ~(1 << 1);
        if (waiting_falling_edge == 0) {
            t_rise = captured;
            waiting_falling_edge = 1;
            TIM2->CCER |= (1 << 1);     // chuyển sang sườn xuống
        } else {
            t_fall = captured;
            duration_us = (t_fall >= t_rise) ? (t_fall - t_rise)
                                              : (0xFFFFFFFF - t_rise + t_fall + 1);
            distance_cm = (uint32_t)((float)duration_us / 2.0f / 29.412f);
            waiting_falling_edge = 0;
            measurement_ready    = 1;
            TIM2->CCER &= ~(1 << 1);    // trở về sườn lên
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
        if (measurement_ready) {
            measurement_ready = 0;
            if (distance_cm < 20) GPIOA->BSRR = (1 << 5);
            else                  GPIOA->BSRR = (1 << (5+16));
        }
    }
}`;

const week6_5 = `#include "stm32f4xx_hal.h"

void init_pa5_tim2_ch1(void)
{
    RCC->AHB1ENR |= (1 << 0);
    GPIOA->MODER  &= ~(3 << (2*5));
    GPIOA->MODER  |=  (2 << (2*5));    // PA5: alternate function
    GPIOA->AFR[0] &= ~(0xF << (4*5));
    GPIOA->AFR[0] |=  (0x1 << (4*5));  // AF1 = TIM2_CH1
}

void tim2_init_pwm_1khz(void)
{
    RCC->APB1ENR |= (1 << 0);
    TIM2->CR1   = 0;
    TIM2->PSC   = 16 - 1;              // 1 MHz
    TIM2->ARR   = 1000 - 1;            // 1 kHz
    TIM2->CCR1  = 0;
    TIM2->CCMR1 &= ~((3<<0)|(7<<4));
    TIM2->CCMR1 |=  (6 << 4);          // OC1M = 110: PWM Mode 1
    TIM2->CCMR1 |=  (1 << 3);          // OC1PE = 1
    TIM2->CCER  |=  (1 << 0);          // CC1E
    TIM2->CR1   |=  (1 << 7);          // ARPE
    TIM2->EGR    = (1 << 0);
    TIM2->CR1   |=  (1 << 0);
}

void set_pwm_duty_percent(uint32_t duty)
{
    if (duty > 100) duty = 100;
    TIM2->CCR1 = ((TIM2->ARR + 1) * duty) / 100;
}

int main(void)
{
    HAL_Init();
    init_pa5_tim2_ch1();
    tim2_init_pwm_1khz();
    int32_t duty = 0, step = 2;
    while (1)
    {
        set_pwm_duty_percent((uint32_t)duty);
        for (volatile int i = 0; i < 10000; i++);
        duty += step;
        if (duty >= 100) { duty = 100; step = -2; }
        else if (duty <= 0) { duty = 0; step =  2; }
    }
}`;

const week6_6 = `#include "stm32f4xx_hal.h"

volatile uint8_t  waiting_falling_edge = 0;
volatile uint8_t  measurement_ready    = 0;
volatile uint32_t t_rise = 0, t_fall = 0;
volatile uint32_t duration_us = 0, distance_cm = 0;

void init_hcsr04_pins(void)
{
    RCC->AHB1ENR |= (1<<0)|(1<<1);
    // PA0: TIM2_CH1 (AF1) – ECHO
    GPIOA->MODER  &= ~(3<<(2*0)); GPIOA->MODER  |= (2<<(2*0));
    GPIOA->AFR[0] &= ~(0xF<<(4*0)); GPIOA->AFR[0] |= (0x1<<(4*0));
    // PB6: GPIO output – TRIGGER
    GPIOB->MODER  &= ~(3<<(2*6)); GPIOB->MODER  |= (1<<(2*6));
    GPIOB->BSRR    = (1<<(6+16));
}

void init_pwm_led_pa6(void)
{
    RCC->AHB1ENR |= (1 << 0);
    // PA6: TIM3_CH1 (AF2)
    GPIOA->MODER  &= ~(3<<(2*6)); GPIOA->MODER  |= (2<<(2*6));
    GPIOA->AFR[0] &= ~(0xF<<(4*6)); GPIOA->AFR[0] |= (0x2<<(4*6));
}

void tim2_init_input_capture(void)     // như Bài 4, PSC=15 → 1µs/tick
{
    RCC->APB1ENR |= (1<<0);
    TIM2->CR1=0; TIM2->PSC=16-1; TIM2->ARR=0xFFFFFFFF; TIM2->CNT=0;
    TIM2->CCMR1 &= ~(3<<0); TIM2->CCMR1 |= (1<<0);
    TIM2->CCER  &= ~((1<<1)|(1<<3)); TIM2->CCER |= (1<<0);
    TIM2->DIER |= (1<<1); TIM2->EGR=(1<<0); TIM2->SR=0;
    NVIC_EnableIRQ(TIM2_IRQn); TIM2->CR1|=(1<<0);
}

void tim3_init_pwm_1khz(void)
{
    RCC->APB1ENR |= (1 << 1);
    TIM3->CR1   = 0;
    TIM3->PSC   = 16 - 1;  TIM3->ARR = 1000 - 1;  TIM3->CCR1 = 0;
    TIM3->CCMR1 &= ~((3<<0)|(7<<4));
    TIM3->CCMR1 |=  (6<<4)|(1<<3);     // PWM Mode 1, OC1PE
    TIM3->CCER  |=  (1<<0);
    TIM3->CR1   |=  (1<<7);
    TIM3->EGR    = (1<<0); TIM3->CR1 |= (1<<0);
}

uint32_t distance_to_duty_percent(uint32_t d_cm)
{
    const uint32_t near_cm = 5, far_cm = 50;
    if (d_cm <= near_cm) return 100;
    if (d_cm >= far_cm)  return 0;
    return ((far_cm - d_cm) * 100) / (far_cm - near_cm);
}

void set_pwm_duty_percent(uint32_t duty)
{
    if (duty > 100) duty = 100;
    TIM3->CCR1 = ((TIM3->ARR + 1) * duty) / 100;
}

void hcsr04_send_trigger(void)
{
    uint32_t start;
    GPIOB->BSRR = (1<<6); start = TIM2->CNT;
    while ((uint32_t)(TIM2->CNT - start) < 10) {}
    GPIOB->BSRR = (1<<(6+16));
}

void TIM2_IRQHandler(void)
{
    if (TIM2->SR & (1<<1)) {
        uint32_t captured = TIM2->CCR1; TIM2->SR &= ~(1<<1);
        if (waiting_falling_edge == 0) {
            t_rise = captured; waiting_falling_edge = 1;
            TIM2->CCER |= (1<<1);
        } else {
            t_fall = captured;
            duration_us = (t_fall >= t_rise) ? (t_fall-t_rise)
                                              : (0xFFFFFFFF-t_rise+t_fall+1);
            distance_cm = (uint32_t)((float)duration_us / 2.0f / 29.412f);
            waiting_falling_edge = 0; measurement_ready = 1;
            TIM2->CCER &= ~(1<<1);
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
        if (measurement_ready) {
            measurement_ready = 0;
            set_pwm_duty_percent(distance_to_duty_percent(distance_cm));
        }
    }
}`;

const week8_1 = `#include <stm32f4xx.h>

void USART2_SendChar(char c)
{
    while ((USART2->SR & (1 << 7)) == 0);  // chờ TXE
    USART2->DR = c;
}

void USART2_SendString(const char *str)
{
    while (*str) USART2_SendChar(*str++);
}

void USART2_Init(void)
{
    RCC->AHB1ENR |= (1 << 0);      // GPIOA clock
    RCC->APB1ENR |= (1 << 17);     // USART2 clock

    GPIOA->MODER  |=  (2 << (2*2));    // PA2: alternate function
    GPIOA->AFR[0] |=  (7 << (4*2));    // AF7 = USART2_TX

    USART2->BRR = (104 << 4) | 3;      // 9600 bps @ 16 MHz
    USART2->CR1 = (1<<3)|(1<<2)|(1<<13); // TE | RE | UE
}

int main(void)
{
    USART2_Init();
    while (1)
    {
        USART2_SendString("Hello, USART2!\\r\\n");
        for (volatile uint32_t i = 0; i < 10000; i++);
    }
}`;

const week8_2 = `#include <stm32f4xx.h>

void USART2_SendChar(char c)
{
    while ((USART2->SR & (1 << 7)) == 0);
    USART2->DR = c;
}

void USART2_SendString(const char *str)
{
    while (*str) USART2_SendChar(*str++);
}

void USART2_Init(void)
{
    RCC->AHB1ENR |= (1 << 0);
    RCC->APB1ENR |= (1 << 17);
    GPIOA->MODER  &= ~(3 << (2*2)); GPIOA->MODER  |= (2 << (2*2));
    GPIOA->AFR[0] &= ~(0xF << (4*2)); GPIOA->AFR[0] |= (7 << (4*2));
    USART2->BRR = (104 << 4) | 3;
    USART2->CR1 = (1<<3)|(1<<2)|(1<<13);
}

void IR_Sensor_Init(void)
{
    RCC->AHB1ENR |= (1 << 2);          // GPIOC clock
    GPIOC->MODER  &= ~(3 << (2*1));    // PC1: input
    GPIOC->PUPDR  &= ~(3 << (2*1));
    GPIOC->PUPDR  |=  (1 << (2*1));    // pull-up
}

int main(void)
{
    USART2_Init();
    IR_Sensor_Init();
    while (1)
    {
        if ((GPIOC->IDR & (1 << 1)) == 0)
            USART2_SendString("Co vat can\\r\\n");
        else
            USART2_SendString("Khong co vat\\r\\n");
        for (volatile uint32_t i = 0; i < 500000; i++);
    }
}`;

const week8_3 = `#include <stm32f4xx.h>

void USART2_SendChar(char c)  { while((USART2->SR&(1<<7))==0); USART2->DR=c; }
void USART2_SendString(const char *s) { while(*s) USART2_SendChar(*s++); }

void USART2_Init(void)
{
    RCC->AHB1ENR |= (1<<0); RCC->APB1ENR |= (1<<17);
    GPIOA->MODER  &= ~(3<<(2*2)); GPIOA->MODER  |= (2<<(2*2));
    GPIOA->AFR[0] &= ~(0xF<<(4*2)); GPIOA->AFR[0] |= (7<<(4*2));
    USART2->BRR = (104<<4)|3;
    USART2->CR1 = (1<<3)|(1<<2)|(1<<13);
}

void Button_Init(void)
{
    RCC->AHB1ENR |= (1 << 2);
    GPIOC->MODER  &= ~(3 << (2*1));    // PC1: input
    GPIOC->PUPDR  &= ~(3 << (2*1));
    GPIOC->PUPDR  |=  (1 << (2*1));    // pull-up
}

void LED_Init(void)
{
    RCC->AHB1ENR |= (1 << 1);
    GPIOB->MODER &= ~(3 << (2*0));
    GPIOB->MODER |=  (1 << (2*0));     // PB0: output
}

int main(void)
{
    USART2_Init();
    Button_Init();
    LED_Init();
    int mode = 0;
    uint8_t  btn_prev = 1;
    uint32_t delay_counter = 0;
    USART2_SendString("MODE = 0: Den tat\\r\\n");
    GPIOB->ODR &= ~(1 << 0);
    while (1)
    {
        uint8_t btn_curr = (GPIOC->IDR & (1 << 1)) ? 1 : 0;
        if (btn_prev == 1 && btn_curr == 0) {
            for (volatile uint32_t i = 0; i < 50000; i++);   // debounce
            if ((GPIOC->IDR & (1 << 1)) == 0) {
                mode++;
                if (mode > 3) mode = 0;
                switch (mode) {
                    case 0: USART2_SendString("MODE = 0: Den tat\\r\\n");
                            GPIOB->ODR &= ~(1<<0); break;
                    case 1: USART2_SendString("MODE = 1: Den bat\\r\\n");
                            GPIOB->ODR |=  (1<<0); break;
                    case 2: USART2_SendString("MODE = 2: Nhap nhay cham\\r\\n"); break;
                    case 3: USART2_SendString("MODE = 3: Nhap nhay nhanh\\r\\n"); break;
                }
            }
        }
        btn_prev = btn_curr;
        if (mode == 2) {
            delay_counter++;
            if (delay_counter >= 300000) { GPIOB->ODR ^= (1<<0); delay_counter = 0; }
        } else if (mode == 3) {
            delay_counter++;
            if (delay_counter >= 60000)  { GPIOB->ODR ^= (1<<0); delay_counter = 0; }
        } else {
            delay_counter = 0;
        }
    }
}`;

// ===================== BUILD DOCUMENT =====================
const doc = new Document({
  styles: {
    default: { document: { run: { font: FONT, size: 22 } } },
    paragraphStyles: [
      { id: "Heading1", name: "Heading 1", basedOn: "Normal", next: "Normal", quickFormat: true,
        run: { size: 32, bold: true, font: FONT, color: "1F3864" },
        paragraph: { spacing: { before: 360, after: 180 }, outlineLevel: 0 } },
      { id: "Heading2", name: "Heading 2", basedOn: "Normal", next: "Normal", quickFormat: true,
        run: { size: 26, bold: true, font: FONT, color: "2E74B5" },
        paragraph: { spacing: { before: 240, after: 120 }, outlineLevel: 1 } },
    ]
  },
  numbering: {
    config: [{
      reference: "bullets",
      levels: [{ level: 0, format: LevelFormat.BULLET, text: "\u2022",
        alignment: AlignmentType.LEFT,
        style: { paragraph: { indent: { left: 720, hanging: 360 } } } }]
    }]
  },
  sections: [{
    properties: {
      page: {
        size: { width: 11906, height: 16838 },
        margin: { top: 1134, right: 1134, bottom: 1134, left: 1701 }
      }
    },
    children: [
      // COVER
      new Paragraph({ alignment: AlignmentType.CENTER, spacing: { before: 1440 },
        children: [new TextRun({ text: "BÁO CÁO THỰC HÀNH", font: FONT, bold: true, size: 48, color: "1F3864" })] }),
      new Paragraph({ alignment: AlignmentType.CENTER, spacing: { before: 240 },
        children: [new TextRun({ text: "VI ĐIỀU KHIỂN STM32F401RE", font: FONT, bold: true, size: 36, color: "2E74B5" })] }),
      new Paragraph({ alignment: AlignmentType.CENTER, spacing: { before: 480, after: 240 },
        children: [new TextRun({ text: "Lập trình GPIO – Ngắt – Timer – UART", font: FONT, size: 28, italics: true })] }),
      pageBreak(),

      // ======= WEEK 1 =======
      h1("TUẦN 1 – LẬP TRÌNH GPIO CƠ BẢN"),
      h2("Bài 1 – Nhấp nháy LED (PA5)"),
      p("Mô tả: Bật/tắt LED tích hợp PA5 bằng thanh ghi BSRR, dùng vòng lặp trống làm trễ."),
      p("Thuật toán:"),
      bullet("Bật clock GPIOA qua RCC->AHB1ENR bit 0."),
      bullet("Cấu hình PA5 là output (MODER[11:10] = 01)."),
      bullet("Trong while(1): set BSRR bit 5 → bật; delay; set BSRR bit 21 → tắt; delay."),
      p("Source code:", { bold: true }),
      ...codeSection("week1/week1_1.cpp", week1_1),

      new Paragraph({ spacing: { before: 200 } }),
      h2("Bài 2 – Điều khiển LED bằng nút USER (PC13)"),
      p("Mô tả: Đọc trạng thái nút USER (PC13, active-low) để điều khiển LED PA5."),
      p("Thuật toán:"),
      bullet("Bật clock GPIOA và GPIOC."),
      bullet("PA5: output; PC13: input (pull-up nội)."),
      bullet("Vòng lặp chính: đọc IDR[13] – nếu = 0 thì bật LED, ngược lại tắt."),
      p("Source code:", { bold: true }),
      ...codeSection("week1/week1_2.cpp", week1_2),
      pageBreak(),

      // ======= WEEK 2 =======
      h1("TUẦN 2 – LẬP TRÌNH GPIO NHIỀU CHÂN"),
      h2("Bài 1 – Nút bấm ngoài điều khiển LED"),
      p("Mô tả: Nút bấm ngoài PC5 điều khiển LED tại PC6."),
      p("Thuật toán:"),
      bullet("Bật clock GPIOC. PC5: input (no-pull); PC6: output."),
      bullet("Đọc IDR[5]; bằng 0 → bật LED, ngược lại → tắt."),
      p("Source code:", { bold: true }),
      ...codeSection("week2/week2_1.cpp", week2_1),

      new Paragraph({ spacing: { before: 200 } }),
      h2("Bài 2 – Ba nút bấm điều khiển ba LED độc lập"),
      p("Mô tả: Mỗi nút (PC5, PC6, PC8) điều khiển một LED (PB13, PB14, PB15) độc lập."),
      p("Thuật toán:"),
      bullet("Bật clock GPIOB + GPIOC. Cấu hình 3 input (PC5/6/8) và 3 output (PB13/14/15)."),
      bullet("Đọc ba nút riêng biệt, cập nhật LED tương ứng trong mỗi iteration."),
      p("Source code:", { bold: true }),
      ...codeSection("week2/week2_2.cpp", week2_2),
      pageBreak(),

      // ======= WEEK 3 =======
      h1("TUẦN 3 – LAB 3: GPIO VỚI CHỐNG DỘI PHÍM"),
      h2("Bài 1 – Cảnh báo an toàn hai nút SW1 & SW2"),
      p("Mô tả: LED PB0 chỉ sáng khi đồng thời nhấn SW1 (PA0) và SW2 (PA1). Debounce 50 ms."),
      p("Thuật toán:"),
      bullet("PA0, PA1: input pull-down; PB0: output push-pull."),
      bullet("read_debounced_switch(): phát hiện thay đổi → delay 50 ms → đọc lại xác nhận."),
      bullet("Bật LED khi cả hai hàm trả về 1 (cả hai nút đang được nhấn)."),
      p("Source code:", { bold: true }),
      ...codeSection("week3/week3_1.cpp", week3_1),

      new Paragraph({ spacing: { before: 200 } }),
      h2("Bài 2 – Bộ đếm nhị phân 3 LED"),
      p("Mô tả: SW1 (PA0) tăng bộ đếm 0–7 mỗi lần nhấn; ba LED PB0/1/2 hiển thị giá trị nhị phân."),
      p("Thuật toán:"),
      bullet("counter = (counter + 1) & 0x07 mỗi lần phát hiện sườn lên hợp lệ."),
      bullet("update_leds(): kiểm tra từng bit 0/1/2 của counter để bật/tắt LED1/2/3."),
      p("Source code:", { bold: true }),
      ...codeSection("week3/week3_2.cpp", week3_2),

      new Paragraph({ spacing: { before: 200 } }),
      h2("Bài 3 – Đếm và nhấp nháy LED theo số lần nhấn"),
      p("Mô tả: Bộ đếm 1–5 (vòng lại sau 5). Mỗi lần nhấn SW1 hợp lệ, LED PB0 nhấp nháy đúng số lần."),
      p("Thuật toán:"),
      bullet("counter++ trong khoảng 1–5; vượt 5 thì reset về 1."),
      bullet("blink_led(counter): bật/tắt LED counter lần, mỗi pha 200 ms."),
      p("Source code:", { bold: true }),
      ...codeSection("week3/week3_3.cpp", week3_3),
      pageBreak(),

      // ======= WEEK 4-5 =======
      h1("TUẦN 4–5 – LAB 4: NGẮT NGOÀI (EXTI)"),
      h2("Bài 1 – EXTI hai chân và quan sát ưu tiên"),
      p("Mô tả: LED PA5 toggle trong main. PC13 → ISR EXTI15_10: tắt LED 3 s; PC8 → ISR EXTI9_5: bật LED 3 s."),
      p("Cấu hình:"),
      bullet("SYSCFG kết nối PC13→EXTI13, PC8→EXTI8."),
      bullet("Bật IMR, FTSR (sườn xuống), NVIC cho cả hai đường."),
      bullet("Nhấn đồng thời để quan sát ngắt nào được ưu tiên xử lý trước."),
      p("Source code:", { bold: true }),
      ...codeSection("week4_5/week4_5_1.cpp", week4_1),

      new Paragraph({ spacing: { before: 200 } }),
      h2("Bài 2 – Dịch LED vòng tròn bằng EXTI"),
      p("Mô tả: SW1 (PC5) dịch trái, SW2 (PC6) dịch phải, SW3 (PC13) dừng. LED sáng tuần tự PB13→PB14→PB15."),
      p("Thuật toán:"),
      bullet("Biến toàn cục led_offset: +1 (trái), −1 (phải), 0 (dừng)."),
      bullet("ISR chỉ cập nhật led_offset. Main tính LED tiếp theo: (cur − BASE + offset + 3) % 3."),
      p("Source code:", { bold: true }),
      ...codeSection("week4_5/week4_5_2.cpp", week4_2),

      new Paragraph({ spacing: { before: 200 } }),
      h2("Bài 3 – Ngắt lồng nhau với NVIC Priority"),
      p("Mô tả: Ba ngắt EXTI0/1/2 (SW1/2/3) với ưu tiên SW3 > SW2 > SW1. Minh họa preemption."),
      p("Thiết lập ưu tiên:"),
      bullet("SW3 – EXTI2: mức 0 (cao nhất). LED PB15 bật liên tục ~3 s."),
      bullet("SW2 – EXTI1: mức 1. LED PB14 nhấp nháy 4 lần."),
      bullet("SW1 – EXTI0: mức 2 (thấp nhất). LED PB13 nhấp nháy 2 lần."),
      p("Kiểm chứng: Nhấn SW2 trong khi SW1 đang chạy → SW2 cắt ngang. Sau SW2 xong, SW1 tiếp tục."),
      p("Source code:", { bold: true }),
      ...codeSection("week4_5/week4_5_3.cpp", week4_3),
      pageBreak(),

      // ======= WEEK 6-7 =======
      h1("TUẦN 6–7 – BÀI TẬP TIMER"),
      h2("Bài 1 – Định thời Timer (Up/Down Counter)"),
      p("Mô tả: TIM2 tạo delay chính xác. LED PA5 xen kẽ chu kỳ 1 s (upcounter) và 2 s (downcounter)."),
      p("Cấu hình TIM2: PSC = 16000–1 → 1 kHz (1 ms/tick). ARR = period_ms – 1."),
      bullet("Upcounter: CR1 DIR=0, đếm từ 0 lên ARR."),
      bullet("Downcounter: CR1 DIR=1, đếm từ ARR xuống 0."),
      bullet("delay_period_tim2(): polling cờ UIF, xóa sau khi bắt."),
      p("Source code:", { bold: true }),
      ...codeSection("week6_7/week6_7_1.cpp", week6_1),

      new Paragraph({ spacing: { before: 200 } }),
      h2("Bài 2 – Timer chế độ ngắt"),
      p("Mô tả: TIM3 sinh ngắt mỗi 1 s để toggle LED PA5. Main dùng delay_ms (polling TIM2) nhấp nháy LED PB7."),
      p("Quan sát: LED PA5 không bị ảnh hưởng dù CPU đang bị treo bởi delay trong main."),
      p("Cấu hình: TIM3 PSC=16000–1, ARR=999, bật UIE + NVIC. ISR xóa UIF rồi toggle PA5."),
      p("Source code:", { bold: true }),
      ...codeSection("week6_7/week6_7_2.cpp", week6_2),

      new Paragraph({ spacing: { before: 200 } }),
      h2("Bài 3 – Đèn giao thông thông minh"),
      p("Mô tả: XANH (PA7) 5 s → VÀNG (PA6) 2 s → ĐỎ (PA5) 5 s. Nút USER PC13 yêu cầu qua đường."),
      p("Xử lý nút:"),
      bullet("Đang XANH: lập tức chuyển sang VÀNG → ĐỎ."),
      bullet("Đang ĐỎ/VÀNG: đặt cờ pedestrian_waiting; vào ĐỎ tiếp theo cộng thêm 3 s."),
      p("Source code:", { bold: true }),
      ...codeSection("week6_7/week6_7_3.cpp", week6_3),

      new Paragraph({ spacing: { before: 200 } }),
      h2("Bài 4 – Input Capture đo khoảng cách HC-SR04"),
      p("Mô tả: TIM2_CH1 (PA0) bắt xung ECHO. PA1 phát TRIGGER 10 µs. Tính khoảng cách và cập nhật LED PA5."),
      p("Công thức: distance_cm = duration_us / 2 / 29.412"),
      bullet("Sườn lên: lưu t_rise, đổi capture sang sườn xuống."),
      bullet("Sườn xuống: lưu t_fall, tính duration, chuyển về sườn lên."),
      p("Source code:", { bold: true }),
      ...codeSection("week6_7/week6_7_4.cpp", week6_4),

      new Paragraph({ spacing: { before: 200 } }),
      h2("Bài 5 – Output Compare tạo xung PWM"),
      p("Mô tả: TIM2_CH1 (PA5) PWM Mode 1, tần số 1 kHz. Duty cycle tự động tăng 0 % → 100 % → 0 % tạo hiệu ứng fade."),
      p("Cấu hình: PSC=15 → 1 MHz; ARR=999 → 1 kHz. CCMR1 OC1M=110, OC1PE=1. Bước duty ±2 %."),
      p("Source code:", { bold: true }),
      ...codeSection("week6_7/week6_7_5.cpp", week6_5),

      new Paragraph({ spacing: { before: 200 } }),
      h2("Bài 6 – HC-SR04 + PWM điều khiển độ sáng LED"),
      p("Mô tả: Input Capture (TIM2) đo khoảng cách, PWM (TIM3_CH1, PA6) điều chỉnh độ sáng. Vật càng gần → LED càng sáng."),
      p("Ánh xạ: ≤ 5 cm → 100 %; ≥ 50 cm → 0 %; khoảng giữa tuyến tính."),
      p("Source code:", { bold: true }),
      ...codeSection("week6_7/week6_7_6.cpp", week6_6),
      pageBreak(),

      // ======= WEEK 8 =======
      h1("TUẦN 8 – UART / USART2"),
      h2("Bài 1 – Gửi chuỗi qua USART2"),
      p("Mô tả: Khởi tạo USART2 trên PA2 (TX) với baud rate 9600 bps. Gửi định kỳ chuỗi \"Hello, USART2!\"."),
      p("Cấu hình:"),
      bullet("PA2: AF Mode, AF7 (USART2_TX)."),
      bullet("BRR = (104 << 4) | 3 → 9600 bps với PCLK = 16 MHz."),
      bullet("CR1: bật TE (bit 3), RE (bit 2), UE (bit 13)."),
      p("Source code:", { bold: true }),
      ...codeSection("week8/week8_1.cpp", week8_1),

      new Paragraph({ spacing: { before: 200 } }),
      h2("Bài 2 – Cảm biến IR báo vật cản qua UART"),
      p("Mô tả: Cảm biến IR nối PC1 (active-low). Gửi trạng thái qua USART2 mỗi ~500 ms."),
      p("Thuật toán:"),
      bullet("PC1: input pull-up. IDR[1] = 0 → \"Co vat can\"; = 1 → \"Khong co vat\"."),
      p("Source code:", { bold: true }),
      ...codeSection("week8/week8_2.cpp", week8_2),

      new Paragraph({ spacing: { before: 200 } }),
      h2("Bài 3 – Điều khiển LED đa chế độ qua UART"),
      p("Mô tả: Nút PC1 chuyển đổi 4 chế độ LED (PB0) và in tên chế độ ra USART2."),
      p("Các chế độ:"),
      bullet("Mode 0 – LED tắt."),
      bullet("Mode 1 – LED sáng liên tục."),
      bullet("Mode 2 – LED nhấp nháy chậm (ngưỡng 300 000 vòng)."),
      bullet("Mode 3 – LED nhấp nháy nhanh (ngưỡng 60 000 vòng)."),
      p("Debounce inline: delay 50 000 vòng sau sườn xuống, đọc lại để xác nhận."),
      p("Source code:", { bold: true }),
      ...codeSection("week8/week8_3.cpp", week8_3),
    ]
  }]
});

Packer.toBuffer(doc).then(buf => {
  fs.writeFileSync('BaoCao_STM32_v2.docx', buf);
  console.log('Done');
});