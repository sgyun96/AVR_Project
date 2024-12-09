#ifndef _ULTRASONIC_INT_H_
#define _ULTRASONIC_INT_H_

#include "def.h"



// 초음파 센서 핀 설정
#define TRIG_DDR    DDRE     // 트리거 포트
#define ECHO_DDR    DDRE     // 에코 포트
#define TRIG_PORT   PORTE    // 트리거 핀
#define ECHO_PORT   PORTE    // 에코 핀
#define TRIG_PIN    0        // 트리거 핀 번호
#define ECHO_PIN    6        // 에코 핀 번호

// 초음파 센서 제어
void sonicInit(void);        // 초기화
void sonicTrig(void);        // 트리거 신호 발생
uint16_t sonicDist(void);    // 거리 측정
void initDistLed(void);      // 거리 LED 초기화
void updateDistLed(void);    // 거리 LED 상태 갱신

#endif