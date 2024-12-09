#include "buzz.h"

void buzzInit(void) {
    // GPIO 설정
    BUZZ_DDR |= (1 << BUZZ_PIN);
    BUZZ_PORT &= ~(1 << BUZZ_PIN);
    
    // 타이머2 PWM 설정
    TCCR2 = (1 << WGM21) | (1 << WGM20);  // 고속 PWM 모드
    TCCR2 |= (1 << CS22);                  // 분주비 64
    OCR2 = 0;                              // PWM duty 0%
}

void buzzRun(uint8_t duty) {
    OCR2 = duty;
    TCCR2 |= (1 << COM21) | (1 << COM20);  // PWM 출력 설정
}

void buzzStop(void) {
    OCR2 = 0;
    BUZZ_PORT &= ~(1 << BUZZ_PIN);
}