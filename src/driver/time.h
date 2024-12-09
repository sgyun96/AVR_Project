#ifndef _TIME_H_
#define _TIME_H_

#include "def.h"

extern volatile uint32_t sysTime;    // 시스템 시간 (ms)

void timeInit(void);    // 타이머 초기화
uint32_t millis(void);      // 현재 시간 반환

#endif