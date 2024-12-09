#include "time.h"

volatile uint32_t sysTime = 0;    // 시스템 시간

void timeInit(void) 
{
    // 타이머3 설정 (16비트)
    // 16MHz / 64 분주 = 250kHz
    // CTC 모드로 설정하여 정확히 1ms 마다 인터럽트 발생
    TCCR3A = 0;                    // 노말 모드
    TCCR3B = (1 << WGM32) |       // CTC 모드
             (1 << CS31) |         // 분주비 64
             (1 << CS30);
    
    OCR3A = 250;                   // 1ms를 위한 비교값 (250kHz/1000Hz = 250)
    ETIMSK |= (1 << OCIE3A);       // 출력비교 A 인터럽트 활성화
}

// 타이머3 출력비교 A 인터럽트
ISR(TIMER3_COMPA_vect) 
{
    sysTime++;    // 정확히 1ms 마다 증가
}

uint32_t millis(void)
{
    return sysTime;
}