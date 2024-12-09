#ifndef _BUZZ_H_
#define _BUZZ_H_

#include "def.h"

// 부저 GPIO 설정
#define BUZZ_PIN  PORTB7
#define BUZZ_DDR  DDRB
#define BUZZ_PORT PORTB

// 부저 제어 함수
void buzzInit(void);    // 부저 초기화
void buzzRun(uint8_t duty);    // 부저 동작 (PWM duty 설정)
void buzzStop(void);    // 부저 정지

#endif
                                                                                    